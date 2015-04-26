//*** include ****************************************************************

function Include( file ){
	__Include( file, __CScript );
}

//****************************************************************************

Include( "_user_config.js" );
Vsd = new __VSD_System__( __CVsdFilter );
Log = new __VSD_SystemLog__( __CVsdFilter );

DRAW_FILL			= 1 << 0;
DRAW_NOCLOSE		= 1 << 1;
DRAW_LOADMAP		= 1 << 2;
FONT_BOLD			= 1 << 0;
FONT_ITALIC			= 1 << 1;
FONT_OUTLINE		= 1 << 2;
FONT_FIXED			= 1 << 3;
FONT_NOANTIALIAS	= 1 << 4;
ALIGN_LEFT			= 0;
ALIGN_TOP			= 0;
ALIGN_HCENTER		= 1 << 0;
ALIGN_RIGHT			= 1 << 1;
ALIGN_VCENTER		= 1 << 2;
ALIGN_BOTTOM		= 1 << 3;
DRAW_MAP_START		= 1 << 4;
LMS_HORIZONTAL		= 0;
LMS_VERTICAL		= 1 << 4
GRAPH_HTILE			= 1 << 30;
GRAPH_VTILE			= 1 << 31;
IMG_INET_ASYNC				= 1 << 0;
IMG_STATUS_LOAD_COMPLETE	= 0;
IMG_STATUS_LOAD_INCOMPLETE	= 1;
IMG_STATUS_LOAD_FAILED		= 2;
SEEK_SET	= 0;
SEEK_CUR	= 1;
SEEK_END	= 2;

function GetGear( GearRatio ){
	var Gear;
	for( Gear = 1; Gear < GEAR_RATIO.length; ++Gear ){
		if( GearRatio > ( GEAR_RATIO[ Gear - 1 ] + GEAR_RATIO[ Gear ] ) / 2 ){
			break;
		}
	}
	return Gear;
}

function DisposeAll(){
	if( typeof this[ "Dispose" ] == 'function' ){
		Dispose();
	}
	
	for( var obj in this ){
		if(
			typeof this[ obj ] == 'object' &&
			typeof this[ obj ].Dispose == 'function'
		){
			this[ obj ].Dispose();
			this[ obj ] = null;
		}
	}
}

//*** グラフ描画 *************************************************************

Vsd.MakeGraphParam = function( params ){
	for( var i = 0; i < params.length; ){
		if( Vsd[ params[ i ]] === undefined ){
			params.splice( i, 3 );	// データがない要素を削除
		}else{
			i += 3;
		}
	}
}

Vsd.DrawGraph = function( x1, y1, x2, y2, font, flags, params ){
	// グラフを表示しない条件
	if( !Vsd.Config_graph ) return;
	
	if( params.Adjust === undefined ){
		Vsd.MakeGraphParam( params );
		params.Adjust = true;
	}
	
	var GrpNum = ~~( params.length / 3 );
	var Width  = x2 - x1 + 1;
	var Height = y2 - y1 + 1;
	
	var X1 = x1;
	var X2 = ( flags & GRAPH_HTILE ) ? x1 - 1 : x2;
	var Y1 = y1;
	var Y2 = ( flags & GRAPH_VTILE ) ? y1 - 1 : y2;
	
	for( var i = 0; i < GrpNum; ++i ){
		if( flags & GRAPH_HTILE ){
			X1 = X2 + 1;
			X2 = x1 - 1 + ~~( Width * ( i + 1 ) / GrpNum );
		}else if( flags & GRAPH_VTILE ){
			Y1 = Y2 + 1;
			Y2 = y1 - 1 + ~~( Height * ( i + 1 ) / GrpNum );
		}
		
		Vsd.DrawGraphSingle(
			X1, Y1, X2, Y2,
			params[ i * 3 + 0 ],	// key
			params[ i * 3 + 1 ],	// unit
			font,					// font
			params[ i * 3 + 2 ]		// color
		);
	}
}

//*** Google Map 描画 ********************************************************

Vsd.DrawGoogleMaps = function( param ){
	if( Log.Longitude === undefined ) return;
	
	// 一番最初の地図データを同期モードで取得
	if( param.MapImg === undefined ){
		param.GMapURL = "http://maps.googleapis.com/maps/api/staticmap?sensor=false&language=ja" +
			( GoogleAPIKey[ 0 ] != '' ? "&key=" + GoogleAPIKey[ 0 ] : '' ) +
			"&maptype=" + param.Maptype +
			"&zoom=" + param.Zoom +
			"&size=" + ~~param.Width + "x" + ~~param.Height +
			"&center=";
		
		param.MapImg		= new Image( param.GMapURL + Log.Latitude + "," + Log.Longitude );
		param.Dir			= Log.Direction;
		param.Time			= Vsd.DateTime;
		param.DispLong		= param.Long 	= Log.Longitude;
		param.DispLati		= param.Lati 	= Log.Latitude;
		param.Distance		= Log.Distance;
	}
	
	// 次のマップデータ取得が完了した
	if(
		param.MapImgNext !== undefined &&
		param.MapImgNext.Status == IMG_STATUS_LOAD_COMPLETE
	){
		param.MapImg.Dispose();
		param.MapImg		= param.MapImgNext;
		param.MapImgNext	= undefined;
		param.Dir			= param.NextDir;
		param.DispLong		= param.Long;
		param.DispLati		= param.Lati;
	}
	
	// ピクセル移動量を計算  参考: http://hosohashi.blog59.fc2.com/blog-entry-5.html
	var PixLong = ( Log.Longitude - param.DispLong ) / 180 * ( 1 << ( 7 + param.Zoom ));
	var sin_pic = Math.sin( Math.PI / 180 * param.DispLati );
	var sin_now = Math.sin( Math.PI / 180 * Log.Latitude );
	var PixLati = (
		Math.log(( 1 + sin_pic ) / ( 1 - sin_pic )) -
		Math.log(( 1 + sin_now ) / ( 1 - sin_now ))
	) / Math.PI * ( 1 << ( 7 - 1 + param.Zoom ));
	
	Vsd.PutImage( param.X, param.Y, param.MapImg );
	
	Vsd.DrawCarIndicator(
		param.X + param.Width  / 2 + PixLong,
		param.Y + param.Height / 2 + PixLati,
		Log.Direction, param.IndicatorSize, param.IndicatorColor
	);
	
	// 次のマップデータを非同期モードで取得
	if(
		param.MapImgNext === undefined &&
		Math.abs( param.Time - Vsd.DateTime ) >= param.UpdateTime &&
		( PixLong * PixLong + PixLati * PixLati ) >= param.UpdateDistance * param.UpdateDistance
	){
		param.MapImgNext = new Image(
			param.GMapURL + Log.Latitude + "," + Log.Longitude,
			IMG_INET_ASYNC
		);
		param.Time			= Vsd.DateTime;
		param.Long			= Log.Longitude;
		param.Lati			= Log.Latitude;
		param.NextDir		= Log.Direction;
	}
}

// 自車マーク描画
Vsd.DrawCarIndicator = function( cx, cy, angle, size, color ){
	angle *= Math.PI / 180;
	var cos = Math.cos( angle ) * size;
	var sin = Math.sin( angle ) * size;
	
	apex = [
		0, -1,
		0.707, 0.707,
		0, 0.25,
		-0.707, 0.707,
	];
	
	// 回転 + x,y 座標加算
	for( var i = 0; i < apex.length; i += 2 ){
		var x = cx + apex[ i ] * cos - apex[ i + 1 ] * sin;
		var y = cy + apex[ i ] * sin + apex[ i + 1 ] * cos;
		apex[ i     ] = x;
		apex[ i + 1 ] = y;
	}
	
	Vsd.DrawPolygon( apex, color, DRAW_FILL );
	Vsd.DrawCircle( cx, cy, size, color );
}

//*** Geocoding 処理 *********************************************************

Vsd.Geocoding = function( param ){
	// XMLHttpRequest 作成
	if( param.HttpRequest === undefined ){
		param.HttpRequest = CreateXMLHttpRequest();
		param.Address = undefined;
		param.Result = undefined;
		param.SendRequest = 0;
		param.PrevTime = Vsd.DateTime - param.UpdateTime;
	}
	
	// HTTP リクエスト完了
	if( param.SendRequest && param.HttpRequest.readyState === 4 ){
		param.SendRequest = 0;
		
		if( param.HttpRequest.status === 200 || param.HttpRequest.status === 0 ){
			param.Result = eval( "(" + param.HttpRequest.responseText + ")" );
			if( param.Result.results.length > 0 ){
				var i;
				
				// 道路名，郵便番号 をスキップ
				for( i = 0; i < param.Result.results.length - 1; ++i ){
					if(( "" + param.Result.results[ i ].types ).match( /political/ )) break;
				}
				
				param.Address = param.Result.results[ i ].formatted_address
					.replace( /^日本, (?:〒\d+.\d+ )?/, "" );
			}else{
				param.Address = "(取得できません)";
			}
		}
	}
	
	// リクエスト送信
	if(
		!param.SendRequest &&
		Math.abs( param.PrevTime - Vsd.DateTime ) >= param.UpdateTime
	){
		param.HttpRequest.open(
			"GET",
			"http://maps.googleapis.com/maps/api/geocode/json?sensor=false&latlng=" +
				Log.Latitude + "," + Log.Longitude,
			true
		);
		
		param.HttpRequest.send();
		param.PrevTime = Vsd.DateTime;
		param.SendRequest = 1;
	}
	return param.Address;
	
	// XMLHttpRequest 生成
	function CreateXMLHttpRequest(){
		try{ return new ActiveXObject( "Msxml2.XMLHTTP.6.0" ) }catch( e ){}
		try{ return new ActiveXObject( "Msxml2.XMLHTTP.3.0" ) }catch( e ){}
		try{ return new ActiveXObject( "Microsoft.XMLHTTP"  ) }catch( e ){}
		return false;
	}
}

//*** OpenStreetMap 描画 *****************************************************

Vsd.DrawRoadMap = function( param ){
	if( Log.Longitude === undefined ){
		NoMap( param.X, param.Y, param.X + param.Width - 1, param.Y + param.Height - 1 );
		return;
	}
	
	if( param.MapImg === undefined ){
		param.MapImg = [];
		
		if( param.PrefetchFrame === undefined ) param.PrefetchFrame = 30;
		if( param.PathColor === undefined ) param.PathColor = -1;
		if( param.Maptype === undefined ) param.Maptype = "openstreetmap";
		
		if( param.Maptype == "openstreetmap" ){
			param.TileSize = 256;
			param.TileZoom = param.Zoom;
		}else{
			// GoogleMaps でも表示できるけど地図の著作権的に隠しモード
			param.TileSize = 512;
			param.TileZoom = param.Zoom - 1;
			param.GMapURL = "http://maps.googleapis.com/maps/api/staticmap?sensor=false&language=ja" +
				( GoogleAPIKey[ 0 ] != '' ? "&key=" + GoogleAPIKey[ 0 ] : '' ) +
				"&maptype=" + param.Maptype +
				"&zoom=" + ( param.TileZoom + 1 ) +
				"&size=512x512&center=";
		}
	}
	
	// タイル先読み
	if( Vsd.FrameCnt != param.FrameCnt ){
		for( var i = 0; i < param.PrefetchFrame; ++i ){
			FetchMapTile( Vsd.FrameCnt + i, param );
		}
		param.FrameCnt = Vsd.FrameCnt;
	}
	
	FetchMapTile( Vsd.FrameCnt + param.PrefetchFrame, param );
	++param.FrameCnt;
	
	// タイル番号，タイル内 x,y 座標を求める
	var PosX = Lng2Tile( Log.Longitude, param.TileZoom ); var TileX = ~~PosX;
	var PosY = Lat2Tile( Log.Latitude,  param.TileZoom ); var TileY = ~~PosY;
	PosX = ~~(( PosX - TileX ) * param.TileSize );
	PosY = ~~(( PosY - TileY ) * param.TileSize );
	
	var TileStX = ~~((( param.Width >> 1 ) - PosX + param.TileSize - 1 ) / param.TileSize );
	var OffsStX = TileStX * param.TileSize + PosX - ( param.Width >> 1 );
	var TileStY = ~~((( param.Height >> 1 ) - PosY + param.TileSize - 1 ) / param.TileSize );
	var OffsY   = TileStY * param.TileSize + PosY - ( param.Height >> 1 );
	var OffsX;
	
	TileStX = TileX - TileStX;
	TileY   = TileY - TileStY;
	
	var w, h, e;
	
	// 表示
	for( var y = 0; y < param.Height; ){
		
		TileX = TileStX;
		OffsX = OffsStX;
		
		h = param.TileSize - OffsY;
		if( h > param.Height - y ) h = param.Height - y;
		
		for( var x = 0; x < param.Width; ){
			
			w = param.TileSize - OffsX;
			if( w > param.Width - x ) w = param.Width - x;
			
			// Image 存在確認
			var key = TileX + "," + TileY;
			if( Vsd.IsSaving && param.MapImg[ key ].WaitAsyncLoadComplete( 10000 ));
			
			if( param.MapImg[ key ].Status == IMG_STATUS_LOAD_COMPLETE ){
				Vsd.PutImage( param.X + x, param.Y + y, param.MapImg[ key ], 0, OffsX, OffsY, w, h );
			}else{
				NoMap( param.X + x, param.Y + y, param.X + x + w - 1, param.Y + y + h - 1 );
			}
			
			++TileX;
			OffsX = 0;
			x += w;
		}
		
		++TileY;
		OffsY = 0;
		y += h;
	}
	
	function FetchMapTile( frame, param ){
		// タイル番号，タイル内 x,y 座標を求める
		var PosX = Lng2Tile( Log.ValueOfIndex( "Longitude", frame ), param.TileZoom ); var TileX = ~~PosX;
		var PosY = Lat2Tile( Log.ValueOfIndex( "Latitude",  frame ), param.TileZoom ); var TileY = ~~PosY;
		PosX = ~~(( PosX - TileX ) * param.TileSize );
		PosY = ~~(( PosY - TileY ) * param.TileSize );
		
		//Print( "X=" + TileX + ":" + PosX + " Y=" + TileY + ":" + PosY + "\n" );
		
		// プリフェッチするタイル番号の範囲を求める
		var TileStX = ~~(( PosX - ( param.Width  >> 1 ) + 1 - param.TileSize ) / param.TileSize );
		var TileEdX = ~~(( PosX + ( param.Width  >> 1 ) + 1 ) / param.TileSize );
		var TileStY = ~~(( PosY - ( param.Height >> 1 ) + 1 - param.TileSize ) / param.TileSize );
		var TileEdY = ~~(( PosY + ( param.Height >> 1 ) + 1 ) / param.TileSize );
		
		// プリフェッチ
		var url;
		for( var x = TileX + TileStX; x <= TileX + TileEdX; ++x ){
			for( var y = TileY + TileStY; y <= TileY + TileEdY; ++y ){
				var key = x + "," + y;
				if( param.MapImg[ key ] === undefined ){
					if( param.Maptype == "openstreetmap" ){
						url = "http://" + String.fromCharCode( 0x61 + ~~( Math.random() * 3 )) + ".tile.openstreetmap.org/" + param.TileZoom + "/" + x + "/" + y + ".png";
					}else{
						url = param.GMapURL + Tile2Lat( y + 0.5, param.TileZoom ) + "," + Tile2Lng( x + 0.5, param.TileZoom );
					}
					param.MapImg[ key ] = new Image( url, IMG_INET_ASYNC );
					//Print( "new " + key + "=" + url + "\n" );
				}
			}
		}
	}
	
	// 走行軌跡
	if( param.PathColor != -1 ){
		Vsd.DrawMap(
			param.X, param.Y, param.X + param.Width - 1, param.Y + param.Height - 1,
			DRAW_LOADMAP, param.PathWidth, param.Zoom, param.PathColor
		);
	}
	
	// 自車インジケータ
	Vsd.DrawCarIndicator(
		param.X + param.Width  / 2,
		param.Y + param.Height / 2,
		Log.Direction, param.IndicatorSize, param.IndicatorColor
	);
	
	function Lng2Tile( lng, zoom ){ return ( lng + 180 ) / 360 * Math.pow( 2, zoom ); }
	function Lat2Tile( lat, zoom ){ return ( 1 - Math.log( Math.tan( lat * Math.PI / 180 ) + 1 / Math.cos( lat * Math.PI / 180 ))/ Math.PI ) / 2 * Math.pow( 2, zoom ); }
	
	function Tile2Lng( x, z ){
		return x / Math.pow( 2, z ) * 360 - 180;
	}
	function Tile2Lat( y, z ){
		var n = Math.PI - 2 * Math.PI * y / Math.pow( 2, z );
		return 180 / Math.PI * Math.atan( 0.5 * ( Math.exp( n ) - Math.exp( -n )));
	}
	
	function NoMap( x1, y1, x2, y2 ){
		Vsd.DrawRect( x1, y1, x2, y2, 0xC0C0C0, DRAW_FILL );
		Vsd.DrawRect( x1, y1, x2, y2, 0x808080 );
		Vsd.DrawLine( x1, y1, x2, y2, 0x808080 );
		Vsd.DrawLine( x2, y1, x1, y2, 0x808080 );
	}
}

//*** メータ用目盛り描画 *****************************************************

Vsd.DrawRoundMeterScale = function( param ){
	return Vsd.DrawRoundMeterScaleSub(
		param.X, param.Y, param.R,
		param.Line1Len, param.Line1Width, param.Line1Color, param.Line1Cnt,
		param.Line2Len, param.Line2Width, param.Line2Color, param.Line2Cnt,
		param.MinAngle, param.MaxAngle,
		param.MinVal, param.MaxVal,
		param.NumR, param.FontColor, param.Font
	);
}

Vsd.DrawLinearMeterScale = function( param ){
	return Vsd.DrawLinearMeterScaleSub(
		param.Flag,
		param.X, param.Y, param.Width,
		param.Line1Len, param.Line1Width, param.Line1Color, param.Line1Cnt,
		param.Line2Len, param.Line2Width, param.Line2Color, param.Line2Cnt,
		param.MinVal, param.MaxVal,
		param.NumPos, param.FontColor, param.Font
	);
}

// 後方互換性のための function
Vsd.DrawMeterScale = function(
	x, y, r,
	line1_len, line1_width, line1_color,
	line2_len, line2_width, line2_color, line2_cnt,
	min_angle, max_angle, r_num, max_val, line1_cnt,
	color, font
){
	return Vsd.DrawRoundMeterScaleSub(
		x, y, r,
		line1_len, line1_width, line1_color, line1_cnt,
		line2_len, line2_width, line2_color, line2_cnt,
		min_angle, max_angle, 0, max_val,
		r_num, color, font
	);
}
