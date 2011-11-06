use uo;
use os;
use util;
use cfgfile;

include "include/client";

Const DAMAGETYPE_PHYSICAL	:= 0;
Const DAMAGETYPE_FIRE		:= 1;
Const DAMAGETYPE_COLD		:= 2;
Const DAMAGETYPE_ELECTRICAL	:= 3;
Const DAMAGETYPE_MAGIC		:= 4;
Const DAMAGETYPE_UNRESISTABLE	:= 5;



///////////////////
//  should be called whenever a spell is to do damage to a character
///////////////////

function DoDamageByType (caster, cast_on, damage_amount, damage_type := 0)
	var parms := {};
	parms[1] := caster;
	parms[2] := cast_on;
	parms[3] := damage_amount;
	parms[4] := damage_type;
	start_script (":combat:dodamage", parms);
endfunction




///////////////////
//  dismounts the given mount
///////////////////

function Dismount (character, mount := 0)
	var parms := {};
	parms[1] := character;
	parms[2] := mount;
	return run_script_to_completion (":drocket:dismount", parms);
endfunction




///////////////////
//  Determines if the given character counts as having GM status.  This is used to see if the given account
//  has permission to do things such as check the help page queue.  Characters with a cmdlevel automatically
//  count.  otherwise it checks to see if the accountname is listed in /config/gms.cfg
///////////////////

function IsAGM (character)
	if (character.cmdlevel)
		return CINT (character.cmdlevel);
	endif
	
	UnloadConfigFile ("::gms");
	var cfgfile := ReadConfigFile ("::gms");
	var elem := FindConfigElem (cfgfile, character.acctname);
	if (!elem)
		return 0;
	endif
	if (elem.cmdlevel)
		return elem.cmdlevel;
	endif

	return 0;
endfunction




///////////////////
//  Sends messages to all online GMs.  Useful for important errors
///////////////////

function SendGMErrorMessage (text, syslogit := 0, mincmdlevel := 1)
	if (syslogit)
		syslog (text);
	endif

	syslog (text);
	foreach player in EnumerateOnlineCharacters ()
		if (player.cmdlevel)
			if (player.cmdlevel >= mincmdlevel)
				SendSysMessage (player, text);
			endif
		endif
	endforeach
endfunction




///////////////////
//  stores the serial numbers of all the players who whack the given opponent, so that they
//  can be awared virtue
///////////////////

function AddToHitList (attacker, damaged_mobile)
	if (damaged_mobile.acctname)
		return;
	endif

	if (attacker.npctemplate)
		if (!GetObjProperty (attacker, "master") )
			return;
		endif
	endif

	var hitlist := GetObjProperty (damaged_mobile, "#hitlist");
	if (!hitlist)
		hitlist := {};
	endif

	if (attacker.serial in hitlist)
		return;
	endif

	hitlist.append (attacker.serial);
	SetObjProperty (damaged_mobile, "#hitlist", hitlist);
endfunction




