-- .tab=4

-- シリアルポートなし (debug)
NoSio = "vsd20070421_142959.log"
-- NoSio = true

-- バイナリログ
-- bBinLog = true

--- def, enum
MODE_LAPTIME	= 0
MODE_GYMKHANA	= 1
MODE_ZERO_FOUR	= 2
MODE_ZERO_ONE	= 3
MODE_NUM		= 4

MAIN_MODE_NORMAL	= 0
MAIN_MODE_MSGWINDOW	= 1
MAIN_MODE_DEL		= 2

H8HZ			= 16030000
SERIAL_DIVCNT	= 16		-- シリアル出力を行う周期
LOG_FREQ		= ( H8HZ / 65536 / SERIAL_DIVCNT )

-- スピード * 100/Taco 比
-- ELISE
-- ギア比 * マージンじゃなくて，ave( ギアn, ギアn+1 ) に変更
GEAR_RATIO1 = 1.2381712993947
GEAR_RATIO2 = 1.82350889069989
GEAR_RATIO3 = 2.37581451065366
GEAR_RATIO4 = 2.95059529470571

-- たぶん，ホイル一周が30パルス
--PULSE_PAR_1KM	= ( 172155 / 11.410 )	-- ELISE
PULSE_PAR_1KM	= ( 68774.48913 / 4.597593609 )	-- ELISE(補正後)

ITOA_RADIX_BIT	= 7
ITOA_DIGIT_NUM	= (( 32 + ITOA_RADIX_BIT - 1 ) / ITOA_RADIX_BIT )

--ACC_1G_X	= 6762.594337	-- X軸
ACC_1G_X	= 6659.691379	-- Z軸
ACC_1G_Y	= 6591.556755

-- シフトインジケータの表示
TachoBar1 = { 0, 5100, 5500, 5900, 6300, 6300 }
TachoBar2 = { 0, 5900, 6100, 6300, 6500, 6500 }
-- TachoBar1 = { 0, 1500, 2000, 2500, 3000, 3500 }
-- TachoBar2 = TachoBar1;

-- config
FirmWare			= "vsd.mot"	-- ファームウェア
GymkhanaStartMargin	= 0.5 * ( PULSE_PAR_1KM / 1000 )	-- スタートまでの移動距離
StartGThrethold		= 500	-- 車がスタートしたとみなすGセンサの数値

--- gloval vars --------------------------------------------------------------

-- ---
RxBuf = ""

Tacho		= 0
Speed		= 0
Mileage		= 0
MileagePrev	= 0
GSensorX	= 0
GSensorY	= 0
IRSensor	= 0

fpLog	= nil;

-- 画面モード・動作モード
VSDMode	= MODE_LAPTIME
MainMode = MAIN_MODE_NORMAL
RedrawLap = 2

-- ラップタイム記録
LapTimeTable = {}
BestLap		= nil
BestLapDiff	= nil
LapTimePrev	= nil
LapTimeRaw	= 0

OS = os.getenv( "OS" )

--- Image 拡張 ---------------------------------------------------------------

Image.drawRect = function( this, x, y, w, h, Color )
	this:drawLine( x,		y,		x + w,	y,		Color )
	this:drawLine( x,		y,		x,		y + h,	Color )
	this:drawLine( x + w,	y,		x + w,	y + h,	Color )
	this:drawLine( x,		y + h,	x + w,	y + h,	Color )
end

screen.drawRect = Image.drawRect

--- file 拡張 ----------------------------------------------------------------

Write16 = function( fp, Num )
	Num = math.floor( math.fmod( Num, 0x10000 ))
	fp:write(
		string.char( math.fmod( Num, 0x100 )) ..
		string.char( math.floor( Num / 0x100 ))
	)
end

--- コンソール風出力 ---------------------------------------------------------

Console = {
	Color = nil,
	x = 0,
	y = 0,
}

Console.print = function( this, str, Color )
	screen:print( this.x, this.y, str, Color or this.Color )
	this.y = this.y + 8
end

Console.SetPos = function( this, x, y )
	this.x, this.y = x, y
end

Console.Open = function( this, w, h, x, y, Color )
	x = ( x or ( 480 - ( w + 2 ) * 8 ) / 2 / 8 ) * 8
	y = ( y or ( 272 - ( h + 2 ) * 8 ) / 2 / 8 ) * 8
	
	screen:fillRect(
		x, y,
		( w + 2 ) * 8, ( h + 2 ) * 8,
		Color or ColorLapBG
	)
	
	screen:drawRect(
		x + 4, y + 4, ( w + 1 ) * 8, ( h + 1 ) * 8,
		Color or ColorInfo
	)
	
	this:SetPos( x + 8, y + 8 )
end

--- load firmware ------------------------------------------------------------

function LoadFirmware()
	
	System.sioWrite( "z\r" )
	screen.waitVblankStart( 6 )
	System.sioWrite( "l\r" )
	
	local fpFirm = io.open( FirmWare, "rb" )
	System.sioWrite( string.gsub( fpFirm:read( "*a" ), "\r\n", "\r" ))
	fpFirm:close()
	
	screen.waitVblankStart( 6 )
	System.sioWrite( "g\r" )
	
	-- バッファクリア
	local pos
	System.sioRead()
	RxBuf = ""
	
	-- オープニングメッセージスキップ
	TimeoutCnt = 1000
	repeat
		RxBuf = RxBuf .. System.sioRead()
		pos = RxBuf:find( "\r\nT", 1, true )
		TimeoutCnt = TimeoutCnt - 1
		assert( TimeoutCnt ~= 0, "VSD initialize failed" )
	until pos
	
	RxBuf = RxBuf:sub( pos + 2 )
	
	-- ログファイル リオープン
	if fpLog then fpLog:close() end
	LogFile = os.date( "vsd%Y%m%d_%H%M%S.log" )
	fpLog = io.open( LogFile, "ab" )
	fpLog:setvbuf( "full", 1024 )
	
	-- VSD モード設定
	System.sioWrite( "3Gs" )
end

--- ダミーシリアル入力 -------------------------------------------------------

if( NoSio ) then
	DummySioTimer = Timer.new()
	DummySioTimer:start()
	
	TSC = Timer.new() TSC:start()
	DebugPrevKey = 0
	
	function ItoA( Num )
		local Ret = ""
		local Digit = 0
		repeat
			Ret = string.char( math.fmod( Num, 128 ) + 0x40 ) .. Ret
			Num = math.floor( Num / 128 )
		until Num == 0
		
		return Ret
	end
	
	if( type( NoSio ) == "string" ) then
		fpIn = io.open( NoSio, "r" )
		assert( fpIn, "Can't open file:" .. NoSio )
		
		-- ログ再生
		System.sioRead = function ()
			local Ret = ""
			local Line = nil
			local Params = {}
			local LapTimeStr = ""
			
			if( DummySioTimer:time() > ( 1000 / 15 )) then
				DummySioTimer:reset()
				DummySioTimer:start()
				
				-- 行頭が数値でなければ，有効な行ではない
				repeat
					Line = fpIn:read()
				until( 0x30 <= Line:byte() and Line:byte() <= 0x39 )
				
				-- ラップタイムあり?
				local result, tmp, min, sec = Line:find( "LAP.*(%d+):([%d%.]+)" )
				if( result ) then
					LapTimePrev = 0
					LapTimeStr = "L" .. ItoA( math.floor(( min * 60 + sec ) * H8HZ / 0x10000 + 0.5 )) .. " "
				end
				
				-- 
				for w in Line:gmatch( "[^%s]+" ) do
					Params[ #Params + 1 ] = tonumber( w )
				end
				
				Ret = string.format(
					"T%s S%s M%s g%s %s\r\n",
					ItoA( Params[ 1 ] ),
					ItoA( math.floor( Params[ 2 ] * 100 )),
					ItoA( math.floor( math.fmod( Params[ 3 ] / 1000 * PULSE_PAR_1KM, 0x10000 ))),
					ItoA( Params[ 4 ] + Params[ 5 ] * 0x10000 ),
					LapTimeStr
				)
				
				return Ret
			end
			
			return ""
		end
	else
		
		-- 自動ログ生成
		System.sioRead = function ()
			local Ret = ""
			if( DummySioTimer:time() > ( 1000 / 15 )) then
				DummySioTimer:reset()
				DummySioTimer:start()
				Ret = string.format(
					"T%s S%s g%s ",
					ItoA( math.fmod( TSC:time(), 6800 ) / 2 + 3400 ),
					ItoA( math.fmod( TSC:time(), 20000 )),
					ItoA(
						( 32000 + math.floor( 6000 * math.sin( TSC:time() / 150 * 17 ))) * 0x10000 +
						32000 + math.floor( 6000 * math.cos( TSC:time() / 150 * 23 ))
					)
				)
				
				if( not DebugPrevKey and Controls.read():l()) then
					Ret = Ret .. "L" .. ItoA( TSC:time() / 1000 * ( H8HZ / 65536 )) .. " "
				end
				DebugPrevKey = Controls.read():l()
				
				return Ret .. "\r\n"
			end
			
			return ""
		end
	end
	System.sioWrite = function ( str )
		fpLog:write( "<<SIO output:" .. str .. "\r\n" )
	end
end -- NoSio

--- ログデータ取得 -----------------------------------------------------------

function GetLogData()
	RxBuf = RxBuf .. System.sioRead()
	
	if( RxBuf == "" ) then return end
	Len = RxBuf:len()
	
	--printTerminal( RxBuf )
	--printTerminal( "\n" )
	
	idx = nil
	
	-- ホワイトスペースをスキップ
	Ret = nil
	NextIdx = nil
	
	for i = 1, Len do
		if( RxBuf:byte( i ) == 0xD ) then Ret = true end
		if( RxBuf:byte( i ) > 0x20 ) then
			NextIdx = i
			break
		end
	end
	
	if( NextIdx == nil ) then
		RxBuf = ""
		return
	elseif( NextIdx > 1 ) then
		RxBuf = RxBuf:sub( NextIdx )
	end
	
	if( Ret ) then return nil, nil, true end
	
	-- ホワイトスペースを検索
	for i = 1, Len do
		if( RxBuf:byte( i ) <= 0x20 ) then
			idx = i
			break
		end
	end
	
	-- パラメータが1個なかった
	if( idx == nil ) then return end
	
	-- パラメータ発見
	Cmd = RxBuf:sub( 1, 1 )
	Num = 0;
	
	for i = 2, idx - 1 do
		Num = Num * 128 + RxBuf:byte( i ) - 0x40
	end
	
	-- ホワイトスペースをスキップ
	Ret = nil
	NextIdx = nil
	
	for i = idx, Len do
		if( RxBuf:byte( i ) == 0xD ) then Ret = true end
		if( RxBuf:byte( i ) > 0x20 ) then
			NextIdx = i
			break
		end
	end
	
	if( NextIdx ) then
		RxBuf = RxBuf:sub( NextIdx )
	else
		RxBuf = ""
	end
	
	-- printTerminal( string.format( ">>%s %d %d (%d)\n", Cmd, Num, NextIdx, RxBuf:len()))
	return Cmd, Num, Ret
end

--- FormatLapTime ------------------------------------------------------------

function FormatLapTime( Time, Ch )
	return string.format(
		'%2d%s%06.3f',
		math.floor( Time / 60 ),
		( Ch or '"' ),
		math.fmod( Time, 60 )
	)
end

--- main ---------------------------------------------------------------------

Console:SetPos( 0, 0 )
-- Console:print( "loading firmware" ); screen.flip()

-- sio 初期化・ファームロード
if( NoSio ) then
	-- ログファイル リオープン
	LogFile = "vsd.log"
	fpLog = io.open( os.date( LogFile ), "wb" )
else
	System.sioInit( 38400 )
	LoadFirmware()
end

-- Console:print( "loading font" ); screen.flip()

-- フォント初期化
FontSpeed = Font.createMonoSpaced()
FontSpeed:setPixelSizes( 0, 100 )
ColorSpeed = Color.new( 102, 255, 255 )
ColorMeter = ColorSpeed

FontHist = Font.createMonoSpaced()
FontHist:setPixelSizes( 22, 25 )
ColorHist = ColorSpeed

FontLap = Font.createMonoSpaced()
FontLap:setPixelSizes( 0, 45 )
ColorLap = ColorSpeed
ColorLapBad = Color.new( 255, 80, 0 );

-- 背景ロード
-- Console:print( "loading tacho image" ); screen.flip()

fpImg = io.open( "vsd_tacho.png", "rb" ); ImgData = fpImg:read( "*a" ); fpImg:close()
ImageTacho = Image.loadFromMemory( ImgData )

ImageSpeed = {}

for i = 1, 5 do
	-- Console:print( "loading speed image" .. i ); screen.flip()
	fpImg = io.open( "vsd_speed" .. i .. ".png", "rb" ); ImgData = fpImg:read( "*a" ); fpImg:close()
	ImageSpeed[ i ] = Image.loadFromMemory( ImgData )
end

-- Console:print( "loading G image" ); screen.flip()
fpImg = io.open( "vsd_g.png", "rb" ); ImgData = fpImg:read( "*a" ); fpImg:close()
ImageG = Image.loadFromMemory( ImgData )

-- Console:print( "init completed." ); screen.flip()

-- 画面パラメータ
TachoCx			= 60
TachoCy			= 60
TachoMeterR		= 48
TachoMeterStart	= math.pi / 4
TachoMeterMaxRev= 8000

GMeterCx	= 366 + 106 / 2 - ( 480 - ImageG:width())
GMeterCy	= TachoCy
GMeterIndicatorSize	= 2
GMeterR		= 106 / 4

SpeedY		= 38
SpeedH		= 113
FontSize	= FontSpeed:getTextSize( "888" )
SpeedX		= 240 - FontSize.width / 2
SpeedY		= SpeedH / 2 + SpeedY + FontSize.height / 2

HistX	= 22
HistY	= 181
HistW	= 174
HistH	= 84

LapX	= 219
LapY	= HistY
LapW	= 242
LapH	= HistH
LapDiffX	= LapX + ( LapW - FontHist:getTextSize( '-01"28.555' ).width ) / 2
LapClockX	= LapX + ( LapW - FontHist:getTextSize( '12:34' ).width )
ColorLapBG = Color.new( 51, 51, 51 );
RefreshFlag = nil

ColorInfo = Color.new( 0, 160, 160 );

LapChartW	= 58
LapChartH	= 32

-- Gセンサキャリブレーション

GSensorCaribCntMax = 15
GSensorCaribCnt = GSensorCaribCntMax
GSensorCx	= 0
GSensorCy	= 0

Console.Color = ColorInfo

-- サウンドロード

SndBestLap = Sound.load( "best_lap.wav" )
SndNewLap  = Sound.load( "new_lap.wav" )

--- ラップタイムウィンドウ描画 -----------------------------------------------

function DrawLap()
	local str
	local Color
	
	-- ラップタイム履歴
	screen:clear()
	screen:fillRect( HistX, HistY, HistW, HistH, ColorLapBG )
	screen:fillRect( LapX, LapY, LapW, LapH, ColorLapBG )
	
	if( BestLap ~= nil ) then
		str = 'Fst ' .. FormatLapTime( BestLap )
	else
		str = 'Fst --"--.---', ColorHist
	end
	screen:fontPrint( FontHist, HistX, HistY + HistH / 3 * 1, str, ColorHist )
	
	if( #LapTimeTable >= 2 ) then
		screen:fontPrint(
			FontHist, HistX, HistY + HistH / 3 * 2,
			string.format( "%3d ", #LapTimeTable - 1 ) .. FormatLapTime( LapTimeTable[ #LapTimeTable - 1 ] ), ColorHist
		)
	end
	
	if( #LapTimeTable >= 3 ) then
		screen:fontPrint(
			FontHist, HistX, HistY + HistH / 3 * 3,
			string.format( "%3d ", #LapTimeTable - 2 ) .. FormatLapTime( LapTimeTable[ #LapTimeTable - 2 ] ), ColorHist
		)
	end
	
	-- ラップタイム
	if( #LapTimeTable > 0 ) then
		str = FormatLapTime( LapTimeTable[ #LapTimeTable ] )
	else
		str = '--"--.---'
	end
	screen:fontPrint( FontLap, LapX, LapY + LapH / 3 * 1.2, str, ColorLap )
	
	if( BestLapDiff == nil ) then
		str = '  -"--.---'
		Color = ColorLap
	elseif( BestLapDiff < 0 ) then
		str = "-" .. FormatLapTime( -BestLapDiff )
		Color = ColorLap
	else
		str = "+" .. FormatLapTime( BestLapDiff )
		Color = ColorLapBad
	end
	screen:fontPrint( FontHist, LapDiffX, LapY + LapH / 3 * 2, str, Color )
	
	-- その他 info 描画
	if    ( VSDMode == MODE_LAPTIME		) then str = "LAP"
	elseif( VSDMode == MODE_GYMKHANA	) then str = "GYMKA"
	elseif( VSDMode == MODE_ZERO_FOUR	) then str = "0-400 "
	elseif( VSDMode == MODE_ZERO_ONE	) then str = "0-100 "
	end
	str = str .. #LapTimeTable
	if( LapTimePrev == nil ) then str = str .. " RDY" end
	
	screen:fontPrint( FontHist, LapX, LapY + LapH, str, ColorInfo )
	screen:fontPrint( FontHist, LapClockX, LapY + LapH, os.date( "%k:%M" ), ColorInfo )
end

--- メーター類描画 -----------------------------------------------------------

Blink = nil

function DrawMeters()
	local TachoBar
	local BarLv
	
	-- スピードメーター
	if(( Speed >= 30000 ) and ( Tacho == 0 )) then
		-- キャリブレーション表示
		BarLv = 5
		Blink = nil
	else
		-- LED の表示 LV を求める
		if( Speed >= 7000 ) then
			TachoBar = TachoBar2
		else
			TachoBar = TachoBar1
		end
		
		for i = 6, 1, -1 do
			if( Tacho >= TachoBar[ i ] ) then
				BarLv = i
				break
			end
		end
		
		if( BarLv == 6 ) then
			BarLv = 5
			if( Blink ) then BarLv = 1; end
			Blink = not Blink
		else
			Blink = nil
		end
	end
	
	screen:blit( ImageTacho:width(), 0, ImageSpeed[ BarLv ] )
	screen:fontPrint( FontSpeed, SpeedX, SpeedY, string.format( "%3d", Speed / 100 ), ColorSpeed )
	
	-- タコメータの描画
	screen:blit( 0, 0, ImageTacho )
	TachoRad = Tacho / TachoMeterMaxRev * 2 * math.pi + TachoMeterStart
	screen:drawLine(
		TachoCx, TachoCy,
		TachoCx + TachoMeterR * math.cos( TachoRad ),
		TachoCy + TachoMeterR * math.sin( TachoRad ),
		ColorMeter
	)
	screen:print( TachoCx + 20, TachoCy + 10, string.format( "%4d", Tacho ), ColorMeter )
	
	-- Gセンサ描画
	
	if( GxTmp ) then
		ImageG:fillRect(
			GxTmp, GyTmp,
			GMeterIndicatorSize,
			GMeterIndicatorSize,
			ColorLapBG
		)
	end
	
	if( GSensorCaribCnt == 0 ) then
		GxTmp, GyTmp = 
			GMeterCx +  ( GSensorY - GSensorCy ) / ACC_1G_Y * GMeterR - GMeterIndicatorSize / 2,
			GMeterCy + -( GSensorX - GSensorCx ) / ACC_1G_X * GMeterR - GMeterIndicatorSize / 2
		
		ImageG:fillRect(
			GxTmp, GyTmp,
			GMeterIndicatorSize,
			GMeterIndicatorSize,
			ColorMeter
		)
	end
	screen:blit( 480 - ImageG:width(), 0, ImageG )
	
	-- その他の情報
	if( bDispInfo ) then
		Console:Open( 10, 4, 47, 15 )
	--	Console:print( os.date( "%y/%m/%d" ))
	--	Console:print( os.date( "%H:%M:%S" ))
		Console:print( string.format( "%8.3fkm", Mileage / PULSE_PAR_1KM ))
	--	Console:print( AutoSaveTimer:time())
	--	Console:print( DebugRefresh )
	end
end

--- ラップチャート表示 -------------------------------------------------------

function DrawLapChart()
	local Color
	
	Console:Open( LapChartW, LapChartH )
	Console:print( "Lap  Time" )
	Console:print( "---------------" )
	local y = Console.y
	
	if( #LapTimeTable > 0 ) then
		for i = 1, #LapTimeTable do
			
			if( BestLap and LapTimeTable[ i ] == BestLap ) then
				Color = ColorLapBad
			else
				Color = ColorInfo
			end
			
			Console:print(
				string.format( "%3d %s", i, FormatLapTime( LapTimeTable[ i ] )), Color
			)
			if( math.fmod( i, LapChartH - 2 ) == 0 ) then
				Console:SetPos( Console.x + 15 * 8, y )
			end
		end
	else
		Console:print( "No results." )
	end
	screen.flip()
end

--- HELP 表示 ----------------------------------------------------------------

function DrawHelp()
	Console:Open( 26, 13 )
	Console:print( "Usage:" )
	Console:print( "-------" )
	Console:print( "UP:      delete best lap" )
	Console:print( "DOWN:    lap chart" )
	Console:print( "LEFT:    change mode" )
	Console:print( "RIGHT:   change mode" )
--	Console:print( "LTRIG:" )
	Console:print( "RTRIG:   restart" )
	Console:print( "TRIANGLE:calibration" )
	Console:print( "START:   exit" )
	Console:print( "SELECT:  this help" )
	Console:print( "-------" )
	Console:print( "OS:" .. ( OS or "PSP" ))
	Console:print( "memory:" .. math.floor( collectgarbage( "count" )) .. "KB used" )
	
	screen.flip()
end

--- Delete bestlap 表示 ------------------------------------------------------

function DrawDeleteLap()
	Console:Open( 16, 3 )
	Console:print( "Delete best lap?" )
	Console:print( FormatLapTime( BestLap ))
	Console:print( "   O:ok X:cancel" )
	screen.flip()
end

--- シリアルデータ処理 -------------------------------------------------------

function ProcessSio()
	local Cmd = nil
	local Ret = nil
	
	repeat
		LapTimeStr = ""
		Cmd, Num, Ret = GetLogData()
		
		-- ログのコマンド別処理
		
		if	   Cmd == "T" then Tacho	= Num
		elseif Cmd == "S" then Speed	= Num
		elseif Cmd == "M" then
			if( Num < MileagePrev ) then
				Mileage	= Mileage + Num - MileagePrev + 0x10000
			else
				Mileage	= Mileage + Num - MileagePrev
			end
			MileagePrev = Num
			
		elseif Cmd == "g" then
			GSensorX	= math.fmod( Num, 0x10000 )
			GSensorY	= math.floor( Num / 0x10000 )
			
			if( GSensorCaribCnt > 0 ) then
				GSensorCaribCnt = GSensorCaribCnt - 1
				GSensorCx = GSensorCx + GSensorX
				GSensorCy = GSensorCy + GSensorY
				
				if( GSensorCaribCnt == 0 ) then
					GSensorCx = GSensorCx / GSensorCaribCntMax
					GSensorCy = GSensorCy / GSensorCaribCntMax
				end
			end
		elseif Cmd == "I" then IRSensor	= Num
		elseif Cmd == "L" then
			-- チェックポイントを通過済みならば，周回タイムを求める
			local bBestLap = false
			LapTimeLaw = Num
			
			if( LapTimePrev ~= nil ) then
				local LapTimeDiff = (( Num - LapTimePrev ) / ( H8HZ / 65536 ));
				LapTimeTable[ #LapTimeTable + 1 ] = LapTimeDiff;
				LapTimeStr = "\tLAP" .. #LapTimeTable .. " " .. FormatLapTime( LapTimeDiff, ':' );
				-- ベストラップか?
				if( BestLap ) then BestLapDiff = LapTimeDiff - BestLap end
				if( BestLap == nil or LapTimeDiff < BestLap ) then
					if( BestLap ) then
						bBestLap = true
					end
					BestLap = LapTimeDiff
				end
			end
			
			-- LapTimeMode または，計測スタートなら，clk をリセットする
		--	if( VSDMode == MODE_LAPTIME or LapTimePrev == nil ) then
				LapTimePrev = Num;
				if( VSDMode ~= MODE_LAPTIME or #LapTimeTable == 0 ) then
					LapTimeStr = "\tLAP" .. ( #LapTimeTable + 1 ) .. " start"
				end
		--	else
		--		LapTimePrev = nil;
		--	end
			
			if( bBestLap ) then
				-- ベストラップサウンド
				SndBestLap:play()
			else
				-- ラップサウンド
				SndNewLap:play()
			end
			
			RedrawLap = 2
		end
		
		-- ログに改行が付いたので，可視化ログに出力
		
		if( Ret ) then
		--	if( type( NoSio ) ~= "string" ) then
				if( bBinLog ) then
					-- バイナリログ
					Write16( fpLog, Tacho )
					Write16( fpLog, Speed )
					Write16( fpLog, Mileage )
					Write16( fpLog, GSensorX )	-- 前後G
					Write16( fpLog, GSensorY )	-- 左右G
					Write16( fpLog, IRSensor )
					Write16( fpLog, LapTimeRaw )
					Write16( fpLog, LapTimeRaw / 0x10000 )
				else
					-- テキストログ
					fpLog:write( string.format(
						"%u\t%.2f\t%.2f\t%u\t%u\t%u",
						Tacho, Speed / 100, Mileage / PULSE_PAR_1KM * 1000,
						GSensorX, GSensorY, IRSensor
					))
					
					fpLog:write( LapTimeStr .. "\r\n" )
				end
		--	end
			
			RefreshFlag = true
			DebugRefresh = DebugRefresh + 1
		end
	until Cmd == nil
end

--- VSD モード設定 -----------------------------------------------------------

function SetVSDMode( mode )
	mode = math.fmod( mode + MODE_NUM, MODE_NUM )
	if( NoSio ) then fpLog:write( string.format( "%d-->%d\n", VSDMode, mode )) end
	
	if( mode == MODE_LAPTIME ) then
		System.sioWrite( "l" )
	elseif( mode == MODE_GYMKHANA	) then
		System.sioWrite( string.format( "%Xg", GymkhanaStartMargin ))
	elseif( mode == MODE_ZERO_FOUR	) then
		System.sioWrite( string.format( "%Xf", StartGThrethold ))
	elseif( mode == MODE_ZERO_ONE	) then
		System.sioWrite( string.format( "%Xo", StartGThrethold ))
	end
	
	LapTimePrev = nil
	RedrawLap = 2
	
	return mode
end

--- キーパッド ---------------------------------------------------------------

Ctrl = {}

Ctrl.Prev = Controls.read()
Ctrl.Now  = Controls.read()

Ctrl.Read = function( this )
	this.Prev = this.Now
	this.Now = Controls.read()
	return this.Now:buttons() ~= this.Prev:buttons()
end

Ctrl.Pushed	= function( this, key )
	return ( not this.Prev[ key ]( this.Prev )) and this.Now[ key ]( this.Now )
end

--- メインループ -------------------------------------------------------------

DebugRefresh = 0
CtrlPrev = Controls.read()
PrevMin = 99

AutoSaveTimer = Timer.new()
AutoSaveTimer:start()

while true do
	
	-- シリアルデータ処理
	ProcessSio()
	
	-- キー入力処理
	Ctrl:Read()
	
	-- 時間更新
	if( PrevMin ~= os.date( "*t" ).min ) then
		 PrevMin = os.date( "*t" ).min
		 RedrawLap = 2
	end
	
	-- autosave
	if( AutoSaveTimer:time() >= 60 * 1000 ) then
		AutoSaveTimer:reset()
		AutoSaveTimer:start()
		
		-- ログファイル リオープン
		if fpLog then fpLog:close() end
		fpLog = io.open( LogFile, "ab" )
		fpLog:setvbuf( "full", 1024 )
	end
	
	--
	if( MainMode == MAIN_MODE_NORMAL ) then
		if Ctrl:Pushed( "r" ) then
			-- リスタート
			SetVSDMode( VSDMode )
		elseif Ctrl:Pushed( "right" ) then
			VSDMode = SetVSDMode( VSDMode + 1 )
		elseif Ctrl:Pushed( "left" ) then
			VSDMode = SetVSDMode( VSDMode - 1 )
		elseif Ctrl:Pushed( "down" ) then
			-- ラップチャート
			DrawLapChart()
			MainMode = MAIN_MODE_MSGWINDOW
		elseif Ctrl:Pushed( "up" ) and BestLap ~= nil then
			-- 最速ラップ削除
			DrawDeleteLap()
			MainMode = MAIN_MODE_DEL
		elseif Ctrl:Pushed( "select" ) then
			-- help
			DrawHelp()
			bDispInfo = not bDispInfo
			MainMode = MAIN_MODE_MSGWINDOW
		elseif Ctrl:Pushed( "triangle" ) then
			-- calibration
			System.sioWrite( "c" )
		elseif( RefreshFlag ~= nil or RedrawLap > 0 ) then
			-- 通常の画面処理
			RefreshFlag = nil
			DebugRefresh = DebugRefresh - 1
			
			if( RedrawLap > 0 ) then
				DrawLap()
				RedrawLap = RedrawLap - 1
			end
			DrawMeters()
			screen:flip()
		end
	elseif( MainMode == MAIN_MODE_MSGWINDOW ) then
		-- メッセージウィンドウ表示中
		if Ctrl:Pushed( "cross" ) then
			MainMode = MAIN_MODE_NORMAL
			RedrawLap = 2
		end
	elseif( MainMode == MAIN_MODE_DEL ) then
		-- 最速ラップ削除
		local NewBestLap = nil
		
		if Ctrl:Pushed( "circle" ) then
			for i = 1, #LapTimeTable do
				if( LapTimeTable[ i ] == BestLap ) then
					LapTimeTable[ i ] = 599.999
				elseif( NewBestLap == nil or ( LapTimeTable[ i ] < NewBestLap and LapTimeTable[ i ] < 599 )) then
					NewBestLap = LapTimeTable[ i ]
				end
			end
			BestLap = NewBestLap
			MainMode = MAIN_MODE_NORMAL
			RedrawLap = 2
		elseif Ctrl:Pushed( "cross" ) then
			MainMode = MAIN_MODE_NORMAL
			RedrawLap = 2
		end
	end
	
	if Ctrl:Pushed( "start" ) then break end
	if( OS ) then screen.waitVblankStart() end
end

fpLog:close()
