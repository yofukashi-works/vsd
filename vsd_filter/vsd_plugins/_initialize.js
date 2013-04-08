//*** 設定ここから ***********************************************************

// ギア比の設定  各ギアの エンジン回転[rpm]／スピード[km/h] を設定
GEAR_RATIO = [
	101.9192993,	// 1速
	61.08177186,	// 2速
	45.72406922,	// 3速
	35.95098573,	// 4速
	29.66828919,	// 5速  6速以降がある場合は同じように追加
];

REV_LIMIT		= 6500;	// レブリミット (上限)

//*** 設定ここまで ***********************************************************

Vsd = new __VSD_System__( __CVsdFilter );

DRAW_FILL			= 1;
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
GRAPH_SPEED			= 1 << 0;
GRAPH_TACHO			= 1 << 1;
GRAPH_GX			= 1 << 2;
GRAPH_GY			= 1 << 3;
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

GlobalInstance = this;

function DisposeAll(){
	if( typeof GlobalInstance[ "Dispose" ] == 'function' ){
		Dispose();
	}
	
	for( var obj in GlobalInstance ){
		if(
			obj != "GlobalInstance" &&
			typeof( GlobalInstance[ obj ] ) == 'object' &&
			typeof( GlobalInstance[ obj ].Dispose ) == 'function'
		){
			GlobalInstance[ obj ].Dispose();
			GlobalInstance[ obj ] = null;
		}
	}
}

//*** グラフ描画 *************************************************************

Vsd.MakeGraphParam = function( params ){
	for( var i = 0; i < params.length; ){
		if( typeof( Vsd[ params[ i ]] ) == "undefined" ){
			params.splice( i, 3 );	// データがない要素を削除
		}else{
			i += 3;
		}
	}
}

Vsd.DrawGraph = function( x1, y1, x2, y2, font, flags, params ){
	// グラフを表示しない条件
	if( !Vsd.Config_sync_mode && !Vsd.Config_graph ) return;
	
	// 同期情報表示時は，旧グラフ表示
	if( Vsd.Config_sync_mode || typeof( params ) == 'undefined' ){
		return Vsd.DrawGraphMulti( x1, y1, x2, y2, font, flags );
	}
	
	// 以下，新グラフ表示
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

//*** 多角形描画 *************************************************************

Vsd.DrawPolygon = function( points, color ){
	for( var i = 0; i < points.length; i += 2 ){
		Vsd.DrawLine(
			points[ i + 0 ],
			points[ i + 1 ],
			points[ ( i + 2 ) % points.length ],
			points[ ( i + 3 ) % points.length ],
			0, 1, DRAW_FILL
		);
	}
	Vsd.FillPolygon( color );
}
