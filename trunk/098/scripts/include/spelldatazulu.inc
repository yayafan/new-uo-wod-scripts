function PolymorphMobile( cast_on, new_form, new_color, duration, mod := 0 )

	var parms := {};

	parms[1] := cast_on;
	parms[2] := CInt( new_form );
	parms[3] := CInt( new_color );
	parms[4] := CInt( duration );
	if( mod )
		parms[5] := mod;
	endif

	start_script( ":summoning:polymorphing", parms );

endfunction
 
