"use strict";

function SetupOldApi(){
	if( TargetVSDRevision === undefined ){
		Print(
			"このスキンが動作する VSD のリビジョン番号を\n" +
			"TargetVSDRevision に設定する必要があります．\n"
		);
		return;
	}
	
	// APIKey の設定を _user_config.js に分離
	if( TargetVSDRevision >= 779 ) return;
	if( !GoogleAPIKey[ 0 ]){
		__VSD_System__.prototype.__DrawGoogleMaps_r779 = __VSD_System__.prototype.DrawGoogleMaps;
		
		__VSD_System__.prototype.DrawGoogleMaps = function( param ){
			GoogleAPIKey[ 0 ] = param.APIKey;
			__VSD_System__.prototype.DrawGoogleMaps = __VSD_System__.prototype.__DrawGoogleMaps_r779;
			delete __VSD_System__.prototype.__DrawGoogleMaps_r779;
		}
	}
	
	// Vsd.Speed -> Log.Speed, Vsd.MaxSpeed -> Log.Max.Speed に変更
	if( TargetVSDRevision >= 769 ) return;
	{
		var Getter = '';
		for( var prop in Log ){
			if(
				typeof( Log[ prop ]) != 'object' &&
				typeof( Log[ prop ]) != 'function'
			){
				__VSD_System__.prototype[ "Max" + prop ] = Log.Max[ prop ];
				__VSD_System__.prototype[ "Min" + prop ] = Log.Min[ prop ];
				Getter += Sprintf( "__VSD_System__.prototype.__defineGetter__( '%s', function(){ return Log.%s; });\n", prop, prop );
			}
		}
		eval( Getter );
	}
	
	// DrawOpenStreetMap → Vsd.DrawRoadMap に変更
	if( TargetVSDRevision >= 743 ) return;
	__VSD_System__.prototype.DrawOpenStreetMap = __VSD_System__.prototype.DrawRoadMap;
	
	// DrawMeterScale 廃止
	if( TargetVSDRevision >= 721 ) return;
	__VSD_System__.prototype.DrawMeterScale = function(
		x, y, r,
		line1_len, line1_width, line1_color,
		line2_len, line2_width, line2_color, line2_cnt,
		min_angle, max_angle, r_num, max_val, line1_cnt,
		color, font
	){
		return this.DrawRoundMeterScaleSub(
			x, y, r,
			line1_len, line1_width, line1_color, line1_cnt,
			line2_len, line2_width, line2_color, line2_cnt,
			min_angle, max_angle, 0, max_val,
			r_num, color, font
		);
	}
}

SetupOldApi();
SetupOldApi = undefined;
