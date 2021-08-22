------------------------------------------------------------------------------
--		VSD -- vehicle data logger system  Copyright(C) by DDS
--		$Id$
------------------------------------------------------------------------------
-- .tab=4

--- def, enum
MODE_LAPTIME	= 0
MODE_GYMKHANA	= 1
MODE_ZERO_FOUR	= 2
MODE_ZERO_ONE	= 3
MODE_NUM		= 4

H8HZ			= 16030000
SERIAL_DIVCNT	= 16		-- ���ꥢ����Ϥ�Ԥ�����
LOG_FREQ		= 16

-- ���ԡ��� * 100/Taco ��
-- ELISE
-- ������ * �ޡ����󤸤�ʤ��ơ�ave( ����n, ����n+1 ) ���ѹ�
GEAR_RATIO1 = 1.2381712993947
GEAR_RATIO2 = 1.82350889069989
GEAR_RATIO3 = 2.37581451065366
GEAR_RATIO4 = 2.95059529470571

-- ���֤󡤥ۥ�������30�ѥ륹
PULSE_PER_1KM	= 15473.76689	-- ELISE(CE28N)

ACC_1G_X	= 6762.594337
ACC_1G_Y	= 6667.738702
ACC_1G_Z	= 6842.591839

-- ���եȥ��󥸥�������ɽ��
TachoBar = { 334, 200, 150, 118, 97 }
RevLimit = 6500

-- config
FirmWare			= "vsd_rom.mot"	-- �ե����०����
StartGThrethold		= 500	-- �֤��������Ȥ����Ȥߤʤ�G���󥵤ο���
GymkhanaStartMargin	= 0.5 * ( PULSE_PER_1KM / 1000 )	-- �������ȤޤǤΰ�ư��Υ
SectorCntMax		= 1		-- �ޥ��ͥåȿ�

--dofile( "sio_emulation.lua" )
dofile( "config.lua" )

--- gloval vars --------------------------------------------------------------

Tacho			= 0
Speed			= 0
Mileage			= 0
MileageWA		= 0
MileageWAPrev	= 0
MileageWACnt	= 0
GSensorY		= 0
GSensorX		= 0
IRSensor		= 0
LogCnt			= 0

-- ���̥⡼�ɡ�ư��⡼��
VSDMode	= MODE_LAPTIME
RedrawLap	= 2

-- ��åץ����൭Ͽ
LapTimeTable	= {}
BestLap			= nil
BestLapDiff		= nil
LapTimePrev		= nil
LapTimeRaw		= 0
SectorCnt		= 0

OS = os.getenv( "OS" )

------------------------------------------------------------------------------
-- Utility -------------------------------------------------------------------
------------------------------------------------------------------------------

--- �ե�����ꥹ�ȥ��å� -----------------------------------------------------

function ListupFiles( Ext )
	local RetFiles = {}
	local files = System.listDirectory()
	
	for i = 1, #files do
		if( files[ i ].name:sub( -#Ext ):lower() == Ext ) then
			RetFiles[ #RetFiles + 1 ] = files[ i ].name
		end
	end
	
	return RetFiles
end

--- �����ѥå� ---------------------------------------------------------------

Ctrl = {}

Ctrl.Prev = Controls.read()
Ctrl.Now  = Controls.read()

Ctrl.Read = function( this )
	this.Prev = this.Now
	this.Now = Controls.read()
	-- return this.Now:buttons() ~= this.Prev:buttons()
end

Ctrl.Pushed	= function( this, key )
	return ( not this.Prev[ key ]( this.Prev )) and this.Now[ key ]( this.Now )
end

--- config ���� --------------------------------------------------------------

function SaveConfig()
	fpCfg = io.open( "config.lua", "wb" )
	fpCfg:write(
		"GymkhanaStartMargin=" .. GymkhanaStartMargin .. "\n" ..
		"SectorCntMax=" .. SectorCntMax .. "\n" ..
		'FirmWare="' .. FirmWare .. '"\n'
	)
	fpCfg:close()
end

function SetupMagnet( Cnt )
	SectorCntMax = Cnt
	SaveConfig()
end

function SetupStartDist( Dist )
	GymkhanaStartMargin = Dist * ( PULSE_PER_1KM / 1000 )
	SaveConfig()
end

function SetupFirmware( Name )
	FirmWare = Name
	SaveConfig()
	LoadFirmware()
end

--- ���ե����� �ꥪ���ץ� --------------------------------------------------

function OpenLog( bAppend )
	if fpLog then fpLog:close() end
	
	if(( not bAppend ) or ( not LogFile )) then
		LogFile = os.date( "vsd%Y%m%d_%H%M%S.log" )
	end
	
	fpLog = io.open( LogFile, "ab" )
	fpLog:setvbuf( "full", 1024 )
	
	LogCnt = 0
end

--- ���󥽡��������� ---------------------------------------------------------

Console = {
	Color		= Color.new( 0, 160, 160 ),
	ColorDbg	= Color.new( 255, 255, 255 ),
	x = 0,
	y = 0,
}

Console.print = function( this, str, Color )
	screen:print( this.x, this.y, str, Color or this.Color )
	this.y = this.y + 8
end

Console.DbgPrint = function( this, str, Color )
	if( this.y >= 272 ) then this.y = 0 end
	
	screen.flip()
	Console:print( str, Color or this.ColorDbg )
	screen.flip()
	if( OS ) then
		screen.waitVblankStart()
	end
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

------------------------------------------------------------------------------
-- Display Driver ------------------------------------------------------------
------------------------------------------------------------------------------

Console:SetPos( 0, 0 )
Console:DbgPrint( "Loading font" );

-- �ե���Ƚ����
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
ColorLapBad = Color.new( 255, 80, 0 )

ColorMenuCursor = ColorLapBad

-- �طʥ���
Console:DbgPrint( "Loading image" );

fpImg = io.open( "vsd_tacho.png", "rb" ); ImgData = fpImg:read( "*a" ); fpImg:close()
ImageTacho = Image.loadFromMemory( ImgData )

ImageSpeed = {}

for i = 1, 5 do
	-- Console:DbgPrint( "Loading speed image" .. i );
	fpImg = io.open( "vsd_speed" .. i .. ".png", "rb" ); ImgData = fpImg:read( "*a" ); fpImg:close()
	ImageSpeed[ i ] = Image.loadFromMemory( ImgData )
end

-- Console:DbgPrint( "Loading G image" );
fpImg = io.open( "vsd_g.png", "rb" ); ImgData = fpImg:read( "*a" ); fpImg:close()
ImageG = Image.loadFromMemory( ImgData )

-- ���̥ѥ�᡼��
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
ColorLapBG	= Color.new( 51, 51, 51 )
RefreshFlag = nil

ColorInfo	= Color.new( 0, 160, 160 )

LapChartW	= 58
LapChartH	= 32

--- Image ��ĥ ---------------------------------------------------------------

Image.drawRect = function( this, x, y, w, h, Color )
	this:drawLine( x,		y,		x + w,	y,		Color )
	this:drawLine( x,		y,		x,		y + h,	Color )
	this:drawLine( x + w,	y,		x + w,	y + h,	Color )
	this:drawLine( x,		y + h,	x + w,	y + h,	Color )
end

screen.drawRect = Image.drawRect

--- ��åץ����०����ɥ����� -----------------------------------------------

function DrawLap()
	local str
	local Color
	
	-- ��åץ���������
	screen:clear()
	screen:fillRect( HistX, HistY, HistW, HistH, ColorLapBG )
	screen:fillRect( LapX, LapY, LapW, LapH, ColorLapBG )
	
	if( BestLap ) then
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
	
	-- ��åץ�����
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
	
	-- ����¾ info ����
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

--- �᡼���������� -----------------------------------------------------------

Blink = nil

function DrawMeters()
	local BarLv
	local Gear
	local GearRatio
	
	-- ���������
	GearRatio = Speed / Tacho
	if    ( GearRatio < GEAR_RATIO1 ) then Gear = 1
	elseif( GearRatio < GEAR_RATIO2 ) then Gear = 2
	elseif( GearRatio < GEAR_RATIO3 ) then Gear = 3
	elseif( GearRatio < GEAR_RATIO4 ) then Gear = 4
	else								   Gear = 5
	end
	
	-- ���ԡ��ɥ᡼����
	if(( Speed >= 30000 ) and ( Tacho == 0 )) then
		-- �����֥졼�����ɽ��
		BarLv = 5
		Blink = nil
	else
		-- LED ��ɽ�� LV �����
		BarLv = math.floor(( Tacho - RevLimit ) / TachoBar[ Gear ] ) + 5
		
		if( BarLv >= 5 ) then
			BarLv = 5
			if( Blink ) then BarLv = 1; end
			Blink = not Blink
		elseif( BarLv < 1 ) then
			BarLv = 1
			Blink = nil
		else
			Blink = nil
		end
	end
	
	screen:blit( ImageTacho:width(), 0, ImageSpeed[ BarLv ] )
	screen:fontPrint( FontSpeed, SpeedX, SpeedY, string.format( "%3d", Speed / 100 ), ColorSpeed )
	
	-- �����᡼��������
	screen:blit( 0, 0, ImageTacho )
	TachoRad = Tacho / TachoMeterMaxRev * 2 * math.pi + TachoMeterStart
	screen:drawLine(
		TachoCx, TachoCy,
		TachoCx + TachoMeterR * math.cos( TachoRad ),
		TachoCy + TachoMeterR * math.sin( TachoRad ),
		ColorMeter
	)
	screen:print( TachoCx + 20, TachoCy + 10, string.format( "%4d", Tacho ), ColorMeter )
	
	-- G��������
	
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
			GMeterCx + GSensorX * GMeterR - GMeterIndicatorSize / 2,
			GMeterCy - GSensorY * GMeterR - GMeterIndicatorSize / 2
		
		ImageG:fillRect(
			GxTmp, GyTmp,
			GMeterIndicatorSize,
			GMeterIndicatorSize,
			ColorMeter
		)
	end
	screen:blit( 480 - ImageG:width(), 0, ImageG )
	
	-- ����¾�ξ���
	if( bDispInfo ) then
		Console:Open( 10, 4, 47, 15 )
	--	Console:print( os.date( "%y/%m/%d" ))
		Console:print( string.format( "%8.3fkm", Mileage / PULSE_PER_1KM ))
		Console:print( string.format( "Sector:%d", SectorCnt ))
		Console:print( string.format( "GPS:%d", GPS_Valid ))
	end
end

--- ��åץ��㡼��ɽ�� -------------------------------------------------------

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
	while( not Ctrl:Pushed( "cross" )) do
		DoIntervalProc()
		Ctrl:Read()
	end
	RedrawLap = 2
end

------------------------------------------------------------------------------
-- VSD HW Driver -------------------------------------------------------------
------------------------------------------------------------------------------

RxBuf	= ""

-- G���󥵥����֥졼�����

GSensorCaribCntMax = 15
GSensorCaribCnt = GSensorCaribCntMax
GSensorCx	= 0
GSensorCy	= 0

-- ������ɥ���

Console:DbgPrint( "Loading sound" );
SndBestLap = Sound.load( "best_lap.wav" )
SndNewLap  = Sound.load( "new_lap.wav" )

--- load firmware ------------------------------------------------------------

function LoadFirmware()
	
	local pos
	
	-- ���ե����� �ꥪ���ץ�
	if not Controls.read():r() then
		OpenLog()
	end
	
	-- SIO �����
	Console:DbgPrint( ".Init SIO" )
	
	if( not bSIOActive ) then
		System.sioInit( 38400 )
		bSIOActive = true
	end
	
	-- firmware ����
	
	repeat
		local fpFirm = io.open( FirmWare, "rb" )
		
		if( fpFirm ) then
			Console:DbgPrint( ".Transferring firmware" )
			
			System.sioWrite( "F15EF117*z\r" )
			screen.waitVblankStart( 6 )
			System.sioWrite( "l\r" )
			
			System.sioWrite( string.gsub( fpFirm:read( "*a" ), "\r\n", "\r" ))
			fpFirm:close()
		end
		
		screen.waitVblankStart( 6 )
		System.sioWrite( "g\r" )
		screen.waitVblankStart( 6 )
		System.sioWrite( "F15EF117*1S" )
		
		-- �Хåե����ꥢ
		System.sioRead()
		RxBuf = ""
		
		-- �����ץ˥󥰥�å����������å�
		local TimeoutCnt = 120
		Console:DbgPrint( ".waiting sync. code" )
		
		repeat
			RxBuf = System.sioRead()
			pos = RxBuf:find( "\255", 1, true )
			TimeoutCnt = TimeoutCnt - 1
			
			if( TimeoutCnt == 0 ) then
				Console:DbgPrint( ".Timeout. Retrying... [X] to cancel" )
				if( Controls.read():cross()) then return false end
				
				pos = 0	-- ��¦�롼�פ�ȴ�������
			end
			
			screen.waitVblankStart( 1 )
		until pos
	until pos > 0
	
	RxBuf = RxBuf:sub( pos + 1 )
	
	-- VSD �⡼������
	System.sioWrite( "s1a" )
	
	return true
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

--- ���ꥢ��ǡ�����16bit �������� -------------------------------------------

RxBufPos = 1

function SerialUnpack( str )
	local Ret
	
	if( str:byte( RxBufPos ) == 0xFE ) then
		RxBufPos = RxBufPos + 1
		Ret = str:byte( RxBufPos ) + 0xFE
	else
		Ret = str:byte( RxBufPos )
	end
	RxBufPos = RxBufPos + 1
	
	if( str:byte( RxBufPos ) == 0xFE ) then
		RxBufPos = RxBufPos + 1
		Ret = Ret + ( str:byte( RxBufPos ) + 0xFE ) * 256
	else
		Ret = Ret + str:byte( RxBufPos ) * 256
	end
	RxBufPos = RxBufPos + 1
	
	return Ret
end

--- ���ꥢ��ǡ������� -------------------------------------------------------

function ProcessSio()
	
	local LapTimeStr = ""
	
	if( RxBufPos == 1 ) then
		-- �����Ĥ�ǡ�����̵���Τǡ�Sio ����꡼�ɤ���
		RxBuf = RxBuf .. System.sioRead()
		
		-- \xFF �򸡺���̵����в��⤻�� return
		LogPos = RxBuf:find( "\255", 1, true )
		if( not LogPos ) then return end
		
		RxBufPos = 1
		Tacho = SerialUnpack( RxBuf )
		Speed = SerialUnpack( RxBuf )
		
		RefreshFlag			= true
		
		-- ư���������Ͽ����  carib �ǤⳫ�Ϥ���Ϥ�
		if( Speed > 0 ) then bStartLog = true end
		
		return
	end
	
	-- Speed/Tacho �Ͻ����ѡ��Ĥ���������
	
	MileageWAPrev = MileageWA
	
	MileageWA = SerialUnpack( RxBuf )
	IRSensor  = SerialUnpack( RxBuf )
	GSensorX  = SerialUnpack( RxBuf )
	GSensorY  = SerialUnpack( RxBuf )
	
	if( MileageWAPrev > MileageWA ) then
		MileageWACnt = MileageWACnt + 1
	end
	
	Mileage = MileageWA + MileageWACnt * 0x10000
	
	-- G ���󥵡����� --------------------------------------------------------
	
	if( GSensorCaribCnt > 0 ) then
		GSensorCaribCnt = GSensorCaribCnt - 1
		GSensorCx = GSensorCx + GSensorX
		GSensorCy = GSensorCy + GSensorY
		
		if( GSensorCaribCnt == 0 ) then
			GSensorCx = GSensorCx / GSensorCaribCntMax
			GSensorCy = GSensorCy / GSensorCaribCntMax
		end
		
		GSensorX	= 0
		GSensorY	= 0
	else
		GSensorX	= -( GSensorX - GSensorCx ) / ACC_1G_Y
		GSensorY	=  ( GSensorY - GSensorCy ) / ACC_1G_Z
	end
	
	-- ��åץ�������� ------------------------------------------------------
	
	if( RxBuf:byte( RxBufPos ) ~= 0xFF ) then
		local LapTime = SerialUnpack( RxBuf )
		local tmp     = SerialUnpack( RxBuf )
		
		LapTime = LapTime + tmp * 0x10000
		local LapTimeDiff
		
		SectorCnt = SectorCnt + 1
		if SectorCnt >= SectorCntMax then
			SectorCnt = 0
			
			-- �����å��ݥ���Ȥ��̲�Ѥߤʤ�С����󥿥�������
			local bBestLap = false
			
			if( LapTimePrev ) then
				LapTimeDiff = ( LapTime - LapTimePrev ) / 256
				LapTimeTable[ #LapTimeTable + 1 ] = LapTimeDiff
				LapTimeStr = "\tLAP" .. #LapTimeTable .. " " .. FormatLapTime( LapTimeDiff, ':' )
				-- �٥��ȥ�åפ�?
				if( BestLap ) then BestLapDiff = LapTimeDiff - BestLap end
				if( BestLap == nil or LapTimeDiff < BestLap ) then
					if( BestLap ) then
						bBestLap = true
					end
					BestLap = LapTimeDiff
				end
			else
				-- �������ȥ饤���Ϥ���̲ᤷ���Τǡ��ޡ���������
				LapTimeStr = "\tLAP" .. ( #LapTimeTable + 1 ) .. " start"
			end
			
			LapTimePrev = LapTime
			
			if( bBestLap ) then
				-- �٥��ȥ�åץ������
				SndBestLap:play()
			else
				-- ��åץ������
				SndNewLap:play()
			end
			
			RedrawLap = 2
		elseif( LapTimePrev ) then
			-- ���������̲�
			LapTimeDiff = ( LapTime - LapTimePrev ) / 256
			LapTimeStr = "\tSector" .. SectorCnt .. " " .. FormatLapTime( LapTimeDiff, ':' )
		end
	end
	
	-- ���˲��Ԥ��դ����Τǡ��Ļ벽���˽��� ------------------------------
	
	--if( type( NoSio ) ~= "string" ) then
	if( fpLog and bStartLog ) then
		-- �ƥ����ȥ�
		fpLog:write( string.format(
			"%u\t%.2f\t%.2f\t%.4f\t%.4f\t%u",
			Tacho, Speed / 100, Mileage / PULSE_PER_1KM * 1000,
			GSensorY, GSensorX, IRSensor
		))
		
		-- GPS ��
		if( GetGPSData()) then
			fpLog:write( string.format(
				"\tGPS\t%.10f\t%.10f\t%.10f\t%.10f",
				GPS_Lati,
				GPS_Long,
				GPS_Speed,
				GPS_Bearing
			))
		end
		
		-- ��åץ�����
		fpLog:write( LapTimeStr .. "\r\n" )
		
		LogCnt = LogCnt + 1
	end
	
	--DebugRefresh = DebugRefresh + 1
	
	RxBuf = RxBuf:sub( LogPos + 1 )
	RxBufPos = 1
end

--- VSD �⡼������ -----------------------------------------------------------

function SetVSDMode( mode )
	if( bSIOActive ) then
		mode = math.fmod( mode + MODE_NUM, MODE_NUM )
		if( NoSio ) then fpLog:write( string.format( "%d-->%d\n", VSDMode, mode )) end
		
		if( mode == MODE_LAPTIME ) then
			System.sioWrite( "l" )
		elseif( mode == MODE_GYMKHANA	) then
			System.sioWrite( string.format( "%Xg", GymkhanaStartMargin + 0.5 ))
		elseif( mode == MODE_ZERO_FOUR	) then
			System.sioWrite( string.format( "%Xf", StartGThrethold ))
		elseif( mode == MODE_ZERO_ONE	) then
			System.sioWrite( string.format( "%Xo", StartGThrethold ))
		end
		
		LapTimePrev = nil
		RedrawLap = 2
		SectorCnt = 0
	end
	
	return mode
end

--- ���ꥢ�륢���ƥ��ֳ�ǧ ---------------------------------------------------

bSIOActive = false

------------------------------------------------------------------------------
--- ��˥塼���� -------------------------------------------------------------
------------------------------------------------------------------------------

--- Delete bestlap ɽ�� ------------------------------------------------------

function DeleteLap()
	if( BestLap == nil ) then return end
	
	-- ��®��å׺��
	local NewBestLap = nil
	
	for i = 1, #LapTimeTable do
		if( LapTimeTable[ i ] == BestLap ) then
			LapTimeTable[ i ] = 599.999
		elseif( NewBestLap == nil or ( LapTimeTable[ i ] < NewBestLap and LapTimeTable[ i ] < 599 )) then
			NewBestLap = LapTimeTable[ i ]
		end
	end
	BestLap = NewBestLap
end

--- toggle info window -------------------------------------------------------

function ToggleInfo()
	bDispInfo = not bDispInfo
end

--- VSD ���ڥ���륳�ޥ�� ---------------------------------------------------

function SendCmd_IR()
	System.sioWrite( "0ai" )
end

function SendCmd_Mileage()
	System.sioWrite( "0aM" )
end

--- ��˥塼 -----------------------------------------------------------------

function DoMenu( Item, x, y )
	local MenuID = 1
	local Color
	local BreakMenu
	local left
	local top
	
	BreakMenu = false
	if(( y ) and ( 32 - #Item < y )) then
		y = 32 - #Item
	end
	
	left = x
	top  = y
	
	while( not BreakMenu ) do
		screen.flip()
		Console:Open( Item.width, #Item, x, y )
		if( left == nil ) then
			left = Console.x / 8
			top  = Console.y / 8
		end
		
		-- �����ȥ�
		--if( type( Item.title ) == "string" ) then
		--	Console.y = Console.y - 8
		--	Console:print( Item.title )
		--end
		
		-- �����ƥ�
		
		for i = 1, #Item do
			Color = nil
			if( i == MenuID ) then Color = ColorMenuCursor end
			
			if( type( Item[ i ] ) == "table" ) then
				Console:print( Item[ i ].title, Color )
			else
				Console:print( Item[ i ], Color )
			end
		end
		
		screen.flip()
		while( 1 ) do
			DoIntervalProc()
			Ctrl:Read()
			
			if( Ctrl:Pushed( "up" )) then
				MenuID = MenuID - 1
				if( MenuID <= 0 ) then MenuID = #Item end
				break
			elseif( Ctrl:Pushed( "down" )) then
				MenuID = MenuID + 1
				if( MenuID > #Item ) then MenuID = 1 end
				break
			elseif( Ctrl:Pushed( "circle" )) then
				if( type( Item[ MenuID ] ) == "table" ) then
					if( type( Item[ MenuID ][ 1 ] ) == "function" ) then
						-- ��ü function �ƤӽФ�
						Item[ MenuID ][ 1 ]()
					elseif( not DoMenu( Item[ MenuID ], left + 1, top + MenuID )) then
						-- ���֥�˥塼Ÿ��
						break	-- ���֤��ߤʤΤǡ����Υ�˥塼��Ƴ�
					end
				elseif( type( Item.proc ) == "function" ) then
					-- ���� proc �ƤӽФ�
					Item.proc( Item[ MenuID ] )
				end
				do return true end
			elseif( Ctrl:Pushed( "cross" )) then
				do return false end
			end
		end
	end
end

-- firm �ꥹ�ȥ��å�

FirmList		= ListupFiles( ".mot" )
FirmList.title	= "Firmware"
FirmList.width	= 15
FirmList.proc	= SetupFirmware

MainMenu = {
	title = "Main menu";
	width = 20;
	{
		title = "Magnet setting";
		width = 5;
		proc = SetupMagnet;
		1, 2, 3, 4, 5
	},
	{
		title = "Start distance";
		width = 5;
		proc = SetupStartDist;
		     0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9,
		1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9,
		2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9,
		3.0
	},
	
	FirmList,
	
	{ title = "Re-open log";	OpenLog },
	{
		title = "Delete fastest lap";
		width = 13;
		{
			title = "O:ok X:cancel";
			DeleteLap
		}
	},
	{ title = "Toggle info window";	ToggleInfo },
	{
		title = "Special command";
		width = 20;
		{ title = "Disp IR cnt";  SendCmd_IR },
		{ title = "Disp mileage"; SendCmd_Mileage },
	},
	{
		title = "Help";
		width = 20;
	--	"UP:      delete best lap",
		"DOWN:    lap chart",
		"LEFT:    change mode",
		"RIGHT:   change mode",
	--	"LTRIG:",
		"RTRIG:   restart",
		"CIRCLE:  main menu",
		"TRIANGLE:calibration",
		"START:   exit",
	--	"SELECT:  this help",
	--	"-------",
	--	"OS:" .. ( OS or "PSP" ),
	}
}

------------------------------------------------------------------------------
--- GPS ----------------------------------------------------------------------
------------------------------------------------------------------------------

if( not UsbGps ) then
	UsbGps = {}
	UsbGps.open			= function() return 0 end
	UsbGps.close		= function() end
	UsbGps.get_data		= function() return 0 end
	UsbGps.set_init_loc	= function() end
end

GPS_PrevSec = 99;
GPS_Valid = 0;

function GetGPSData()
	
	local tmp
	
	GPS_Valid,		-- valid
	tmp,			-- year
	tmp,			-- month
	tmp,			-- date
	tmp,			-- hour
	tmp,			-- minute
	GPS_Second,		-- second
	GPS_Lati,		-- lati
	GPS_Long,		-- long
	tmp,			-- altitude
	GPS_Speed,		-- speed
	GPS_Bearing		-- bearing
	= UsbGps.get_data()
	
	if( GPS_Valid >= 1 and GPS_PrevSec ~= GPS_Second ) then
		-- GPS �ǡ�������
		GPS_PrevSec = GPS_Second
		return true
	end
	
	return false
end

------------------------------------------------------------------------------
--- �ᥤ��롼�� -------------------------------------------------------------
------------------------------------------------------------------------------

-- ������֤��Ȥ˽�������롼���� --------------------------------------------

function DoIntervalProc()
	if( OS ) then screen.waitVblankStart() end
	
	if( bSIOActive ) then
		-- ���ꥢ��ǡ�������
		ProcessSio()
		
		-- �����������������ä��顤autosave �ѥ��ե����� �ꥪ���ץ�
		-- ���������̹������ʳ�
		if( RefreshFlag == nil and fpLog and LogCnt >= 60 * LOG_FREQ ) then
			OpenLog( true )
		end
	elseif( GetGPSData()) then
		-- GPS �Υǡ����ǡ����ԡ���ɽ������
		Tacho	= Tacho + GPS_Valid
		Speed	= math.floor( GPS_Speed * 100 + 0.5 )
		RefreshFlag = true
	end
end

-- �ᥤ����� ----------------------------------------------------------------

Console:DbgPrint( "Loading firmware" );
-- sio ��������ե��������
if( NoSio ) then
	-- ���ե����� �ꥪ���ץ�
	LogFile = "vsd.log"
	fpLog = io.open( os.date( LogFile ), "wb" )
	bSIOActive = true
elseif not Controls.read():l() then
	if( not LoadFirmware()) then
		-- return	-- Fail ���Ƥ��̾���̤���äơ�config �Ǥ���褦�ˤ���
	end
end

-- DebugRefresh = 0
CtrlPrev = Controls.read()
PrevMin = 99

Console:DbgPrint( "Init GPS" );
UsbGps.open()
UsbGps.set_init_loc( 0 )

while true do
	DoIntervalProc()
	
	if( RefreshFlag or RedrawLap > 0 ) then
		-- �̾�β��̽���
		if( PrevMin ~= os.date( "*t" ).min ) then
			-- ���ֹ���
			PrevMin = os.date( "*t" ).min
			RedrawLap = 2
		end
		
		RefreshFlag = nil
		-- DebugRefresh = DebugRefresh - 1
		
		if( RedrawLap > 0 ) then
			DrawLap()
			RedrawLap = RedrawLap - 1
		end
		DrawMeters()
		screen:flip()
	else
		Ctrl:Read()
		if Ctrl:Pushed( "r" ) then
			-- �ꥹ������
			SetVSDMode( VSDMode )
		elseif Ctrl:Pushed( "right" ) then
			VSDMode = SetVSDMode( VSDMode + 1 )
		elseif Ctrl:Pushed( "left" ) then
			VSDMode = SetVSDMode( VSDMode - 1 )
		elseif Ctrl:Pushed( "down" ) then
			-- ��åץ��㡼��
			DrawLapChart()
		elseif Ctrl:Pushed( "circle" ) then
			DoMenu( MainMenu )
			RedrawLap = 2
		elseif Ctrl:Pushed( "triangle" ) then
			-- calibration
			if( bSIOActive ) then System.sioWrite( "c" ) end
		elseif Ctrl:Pushed( "start" ) then
			break
		end
	end
end

if( fpLog ) then
	fpLog:close()
	fpLog = nil
end
UsbGps.close()
