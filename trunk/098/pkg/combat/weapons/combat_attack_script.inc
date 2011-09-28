use uo;
use os;
use util;
use cfgfile;

include "include/client";
include "include/objtype";
include "include/wodinc";
include "include/magic";
include "include/poison";
include "include/utility";
include "include/dotempmods";
include "include/attributes";
include "include/itemtypes";
include "../pkg/npcs/npcinfo";
include "../pkg/npcs/npc_util";
include "../pkg/character/virtue/virtue";




///////////////////
//  Figures out the defender's AR rating.  For players, this is done by randomly choosing a zone in which
//  they get hit.  For NPCs, its a combination of any armor they have equipped (if any) and their
//  entry in npcdesc.cfg
///////////////////

function GetDefendersAR (defender)
	//Figure out which zone got hit
	var hitzone := "";
	var therandomnumber := RandomInt (100);
	if (therandomnumber < 7)
		hitzone := "Hands";
	elseif (therandomnumber < 14)
		hitzone := "Neck";
	elseif (therandomnumber < 28)
		hitzone := "Legs/feet";
	elseif (therandomnumber < 42)
		hitzone := "Head";
	elseif (therandomnumber < 56)
		hitzone := "Arms";
	else
		hitzone := "Body";
	endif

	var zone_ar_rating := 0;
	var coverage_array;
	foreach item in ListEquippedItems (defender)
		coverage_array := GetArmorCoverageArray (item);
		if (len (coverage_array))
			if (hitzone in coverage_array)
				//Do some damage to the armor
				if (item.hp or item.hp == 0)
					if (RandomInt (1000) == 1)
						defender_armor.hp := defender_armor.hp - 1;
						if (defender_armor.hp < 1)
							if (defender_armor.desc)
								SendSysMessage (defender, "Your " + defender_armor.desc + " breaks!");
							endif
							DestroyItem (defender_armor);
						endif
					endif
				endif
				if (item.ar > zone_ar_rating)
					zone_ar_rating := item.ar;
					defender_armor := item;
				endif
			endif
		endif
	endforeach

	//Do some damage to the chosen armor, on top of the minor amount of damage already done to all
	//armor covering the zone
	if (defender_armor)
		if (defender_armor.hp or defender_armor.hp == 0)
			if (RandomInt (150) == 1)
				defender_armor.hp := defender_armor.hp - 1;
				if (defender_armor.hp < 1)
					if (defender_armor.desc)
						SendSysMessage (defender, "Your " + defender_armor.desc + " breaks!");
					endif
					DestroyItem (defender_armor);
				endif
			endif
		endif
	endif

	//Add in the bonus from AR modifiers from spells and the like
	if (defender.ar_mod)
		zone_ar_rating := zone_ar_rating + defender.ar_mod;
	endif

	//For NPCs, use the higher AR rating of what they have equipped and their entry in NPCdesc.cfg
	if (defender.npctemplate)
		var base_ar := defender_npcdesc_elem.AR;
		//Add in the bonus from AR modifiers from spells and the like
		if (defender.ar_mod)
			base_ar := base_ar + defender.ar_mod;
		endif
		if (base_ar > zone_ar_rating)
			return base_ar;
		endif
	endif

	return zone_ar_rating;
endfunction




///////////////////
//  Read an array of Coverage elements from the item's itemdesc.cfg entry
///////////////////

function GetArmorCoverageArray (item)
	case (item.objtype)
		UOBJ_BACKPACK:
		UOBJ_MOUNT:
			return 0;
	endcase

	var armor_itemdesc_elem := FindConfigElem (armorcfgfile, item.objtype);
	if (armor_itemdesc_elem)
		var coverage_array := GetConfigStringArray (armor_itemdesc_elem, "Coverage");
		if (len (coverage_array))
			return coverage_array;
		endif
	endif
	return 0;
endfunction




///////////////////
//  This function is slightly less accurate than the general-purpose AwardRawAttributePoints,
//  but is quite a bit faster (by about 40%).  Since the combat advancement systemhook
//  is called so often, this function is used.
///////////////////

function QuickAwardRawAttributePoints (character, attributeid, rawpoints)
	var current := GetAttributeBaseValue (character, attributeid);
	var to_increase := GetNeededRawPointsToIncrease (current);
	if (RandomInt (to_increase) < rawpoints)
		var skillcap := GetBaseSkillCap (character, GetSkillIDByAttributeID (attributeid));
		if (skillcap)
			if (current == skillcap)
				return;
			elseif (current+1 > skillcap)
				current := skillcap-1;
			endif
		endif
		SetAttributeBaseValue (character, attributeid, current+1);
	endif
endfunction

