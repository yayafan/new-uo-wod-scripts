include "include/attributes";

///////////////////
//  Allows the user to try to repair a tailoring item
///////////////////

function RepairTailoringItem (character, tool, item, repair_skill)
	if (!item.maxhp or !item.quality)
		SendSysmessage (character, "That item cannot be repaired.");
		return;
	endif

	//if the item is in full repair, try to do special stuff
	if (item.hp == item.maxhp)
		if ( GetObjProperty (item,"ownerserial") )
			SendSysMessage (character, "That item is in full repair.");
			SendSysMessage (character, "That has an owner's mark!");
			return;
		endif
		if (IsLeatherArmor (item))
			DoSpecialTailoringStuff (character, item, repair_skill);
		elseif (IsAHat (item) and repair_skill == SKILLID_TAILORING)
			FortifyHat (character, item);
		else
			SendSysmessage (character, "That item is in full repair");
		endif
		return;
	endif

	//make sure the item is good enough to be repairable
	if (item.hp <= 0)
		SendSysmessage (character, "The delicate material tears when you handle it.");
		DestroyItem (item);
		return;
	endif

	//setup some variables
	var item_damaged_amount := item.maxhp - item.hp;
	var material_needed := CINT (item_damaged_amount/10);
	if (!material_needed)
		material_needed := 1;
	endif

	//try to find whatever material we need to fix the item
	var found_material;
	if (IsLeatherArmor (item) or IsAMask (item))
		SendSysMessage (character, "You need " + material_needed + " hides to repair this item.");
		found_material := FindObjtypeInContainer (character.backpack, UOBJ_HIDE);
		if (!found_material or found_material.amount < material_needed)
			SendSysMessage (character, "You don't have the material needed to repair the item!");
			return;
		endif
	else
		SendSysMessage (character, "You need " + material_needed + " cloth to repair this item.");
		found_material := FindObjtypeInContainer (character.backpack, UOBJ_CLOTH);
		if (!found_material or found_material.amount < material_needed)
			SendSysMessage (character, "You don't have the material needed to repair the item!");
			return;
		endif
	endif

	PlaySoundEffect(character, SFX_TAILORING);
	sleep (2);
	SubtractAmount (found_material, material_needed);
	var total_damage := GetObjProperty (item, "total_damage");
	if (!total_damage)
		total_damage := 0;
	endif
	if (total_damage + item_damaged_amount > item.maxhp)
		DestroyItem (item);
		SendSysmessage (character, "The delicate material tears when you handle it.");
		return;
	endif

	if (!CheckSkill (character, repair_skill, -1, 0) )
		total_damage := total_damage + RandomInt (10);
		SetObjProperty (item, "total_damage", total_damage);
		if (total_damage + item_damaged_amount > item.maxhp)
			DestroyItem (item);
			SendSysmessage (character, "The delicate material tears when you handle it.");
			return;
		endif
		SendSysMessage (character, "You only manage to damage the item further...");
		return;
	endif

	var skill_bonus := CINT (GetAttribute (character, GetAttributeIDBySkilLID (repair_skill))/4);
	if (item.name["fine"])
		skill_bonus := skill_bonus + 5;
	elseif (item.name["durable"])
		skill_bonus := skill_bonus + 10;
	elseif (item.name["rugged"])
		skill_bonus := skill_bonus + 15;
	elseif (item.name["tempered"])
		skill_bonus := skill_bonus + 20;
	elseif (item.name["indestructable"])
		skill_bonus := skill_bonus + 25;
	elseif (item.name["exceptional"])
		skill_bonus := skill_bonus + 25;
	endif

	item_damaged_amount := CINT (item_damaged_amount * (100-skill_bonus)/100);
	if (item_damaged_amount < 5)
		item_damaged_amount := 5;
	endif

	total_damage := total_damage + item_damaged_amount;
	SetObjProperty (item, "total_damage", total_damage);
	item.hp := item.maxhp;
	SendSysMessage (character, "You repair the item completely");
	return;
endfunction




///////////////////
//  checks what special options are available for upgrading a tailoring item
///////////////////

function DoSpecialTailoringStuff (character, item, repair_skill)

	//nothing we can do with player made items
	if (item.desc["exceptional"] or item.desc["Exceptional"])
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	//if its a magic item and they're using tailoring, they can change the color of the item, or
	//if its a tunic, they can change its gender
	if (IsMagicalItem (item) and repair_skill == SKILLID_TAILORING)
		if ( item.graphic == UOBJ_LEATHER_FEMALE or item.graphic == UOBJ_LEATHER_TUNIC or
				item.graphic == UOBJ_STUDDED_FEMALE or item.graphic == UOBJ_STUDDED_TUNIC
				or item.graphic == UOBJ_LEATHER_TUNIC2 or item.graphic == UOBJ_STUDDED_BUSTIER)
			var menu := CreateMenu ("Options:");
			AddMenuItem (menu, 0, "Reinforce");
			AddMenuItem (menu, 0, "Change gender");

			var selection := SelectMenuItem2 (character, menu);
			if (!selection)
				SendSysMessage (character, "Canceled.");
				return;
			elseif (selection.index == 1)
				DoLeatherColorChange (character, item, repair_skill);
				return;
			elseif (selection.index == 2)
				ChangeGenderOfLeatherArmor (character, item, repair_skill);
				return;
			else
				SendSysMessage (character, "You're not supposed to get this message.  Whoops.");
				return;
			endif
			return;
		else
			DoLeatherColorChange (character, item, repair_skill);
			return;
		endif

	//otherwise, if its a magic item, there's nothing they can do with it
	elseif (IsMagicalItem (item) )
		SendSysMessage (character, "That item is in full repair.");
		return;

	//otherwise try to upgrade it
	else
		DoTailoringItemUpgrade (character, item, repair_skill);
		return;
	endif

endfunction




///////////////////
//  Tries to upgrade the item that was selected
///////////////////

function DoTailoringItemUpgrade (character, item, repair_skill)
	//make sure its an item that can be upgraded
	if (item.desc["xceptional"])
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var leather_cfg_file := ReadConfigFile( ":tailoring:leather_items" );
	var elem := FindConfigElem (leather_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var time_delay := elem.time;
	var item_name := elem.name;
	var item_skill := elem.skill + 15;
	if (item.desc["quality"])
		item_skill := item_skill + 10;
	endif
	if (item_skill > 110)
		item_skill := 110;
	endif

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Upgrading that item will take " + material_needed + " hides and " + item_skill + " skill.");
	if (item_skill > GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
		SendSysMessage (character, "Your skill is too low to upgrade that item.");
		return;
	endif

	SendSysmessage (character, "Pick the hides to use:");
	var found_hides := Target (character);
	if (!found_hides)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!ReserveItem (found_hides) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!IsTypeOfHide (found_hides) )
		SendSysMessage (character, "That's not a hide!");
		return;
	endif
	if (!Accessible (character, found_hides) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_hides.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " hides.  That's only " + found_hides.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Upgrade?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	PerformTailoringItemUpgrade (character, item, found_hides, material_needed, repair_skill, item_skill, time_delay, item_name);
endfunction




///////////////////
//  this function does the actual upgrading of the item
///////////////////

function PerformTailoringItemUpgrade (character, item, found_hides, material_needed, repair_skill, item_skill, time_delay, item_name)

	var sx := character.x;
	var sy := character.y;

	repeat
		for i := 1 to time_delay
			PlaySoundEffect(character, SFX_TAILORING);
			sleep (5);
		endfor

		if (CheckSkill (character, repair_skill, item_skill, 0) )
			SubtractAmount (found_hides, material_needed);
			SendSysMessage (character, "You succeed in upgrading the item.");

			if (item.desc ["quality"])
				item.name := "an exceptional " + TruncateArticle(item_name) + " [crafted by " + character.name + "]";
				item.quality := 1.2;
				item.hp := item.maxhp;
				item.buyprice := 0;
				return;
			else
				item.name := "a quality " + TruncateArticle(item_name) + " [crafted by " + character.name + "]";
				item.quality := 1.0;
				item.hp := item.maxhp;
				item.buyprice := 0;

				item_skill := item_skill + 10;
				if (item_skill > 110)
					item_skill := 110;
				endif

				if (item_skill > GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
					SendSysMessage (character, "Your skill is too low to upgrade the item any further.");
					return;
				endif
			endif
		else
			SubtractAmount (found_hides, RandomInt(material_needed) );
			if (RandomInt (10) == 0)
				SendSysMessage (character, "You ruin the item trying to upgrade it.");
				DestroyItem (item);
				return;
			else
				SendSysMessage (character, "You destroy some material.");
			endif
		endif

		if (!found_hides or found_hides.amount < material_needed)
			SendSysMessage (character, "You don't have enough material to continue.");
			return;
		endif

	until (character.x != sx or character.y != sy);
endfunction




///////////////////
//  allows the user to coat the given piece of equipment with another color
///////////////////

function DoLeatherColorChange (character, item, repair_skill)
	if (repair_skill != SKILLID_TAILORING)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif
	if ( GetObjProperty (item,"ownerserial") )
		SendSysMessage (character, "That has an owner's mark!");
		return;
	endif

	var leather_cfg_file := ReadConfigFile( ":tailoring:leather_items" );
	var elem := FindConfigElem (leather_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var time_delay := elem.time;
	if (time_delay > 1)
		time_delay := time_delay - 1;
	endif;

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Reinforcing that item will take " + material_needed + " hides and 90 skill.");
	if (GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) < 90)
		SendSysMessage (character, "Your skill is too low to reinforce that item.");
		return;
	endif

	SendSysmessage (character, "Target some hides to reinforce the item with.");
	var found_hides := Target (character);
	if (!found_hides)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!ReserveItem (found_hides) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!IsTypeOfHide (found_hides) )
		SendSysMessage (character, "That's not a hide!");
		return;
	endif
	if (!Accessible (character, found_hides) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_hides.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " hides.  That's only " + found_hides.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Reinforce?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	for i := 1 to time_delay
		PlaySoundEffect(character, SFX_TAILORING);
		sleep (5);
	endfor
	PlaySoundEffect(character, SFX_TAILORING);
	sleep (3);

	if (RandomInt (1000) == 0)
		SendSysMessage (character, "You fail, destroying the item.");
		DestroyItem (item);
		return;
	elseif ( RandomInt (100) < 10 )
		SendSysMessage (character, "You fail, destroying some hides.");
		SubtractAmount (found_hides, RandomInt (material_needed) );
		return;
	else
		SendSysMessage (character, "You succeed in reinforcing the item.");
		item.color := found_hides.color;
		SubtractAmount (found_hides, material_needed);
		return;
	endif

endfunction




///////////////////
//  changes the gender of plate armor from male to female and back again
///////////////////

function ChangeGenderOfLeatherArmor (character, item, repair_skill)
	if (repair_skill != SKILLID_TAILORING)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif
	if ( GetObjProperty (item,"ownerserial") )
		SendSysMessage (character, "That has an owner's mark!");
		return;
	endif

	var leather_cfg_file := ReadConfigFile( ":tailoring:leather_items" );
	var elem := FindConfigElem (leather_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var time_delay := elem.time;
	if (time_delay > 1)
		time_delay := time_delay - 1;
	endif;

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Changing the gender of that item will take " + material_needed + " hides and 90 skill.");
	if (GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) < 90)
		SendSysMessage (character, "Your skill is too low to reinforce that item.");
		return;
	endif

	SendSysmessage (character, "Target some hides to change the gender of the item with.");
	var found_hides := Target (character);
	if (!found_hides)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (!ReserveItem (found_hides) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!IsTypeOfHide (found_hides) )
		SendSysMessage (character, "That's not a hide!");
		return;
	endif
	if (!Accessible (character, found_hides) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_hides.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " hides.  That's only " + found_hides.amount + "!");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Change gender?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	for i := 1 to time_delay
		PlaySoundEffect(character, SFX_TAILORING);
		sleep (5);
	endfor
	PlaySoundEffect(character, SFX_TAILORING);
	sleep (3);

	if (RandomInt (1000) == 0)
		SendSysMessage (character, "You fail, destroying the item.");
		DestroyItem (item);
		return;
	elseif ( RandomInt (100) < 10 )
		SendSysMessage (character, "You fail, destroying some hides.");
		SubtractAmount (found_hides, RandomInt (material_needed) );
		return;
	else
		SendSysMessage (character, "You succeed in modifying the item.");
		case (item.graphic)
			UOBJ_LEATHER_TUNIC:
				item.graphic := UOBJ_LEATHER_FEMALE;
			UOBJ_LEATHER_TUNIC2:
				item.graphic := UOBJ_LEATHER_FEMALE;
			UOBJ_LEATHER_FEMALE:
				item.graphic := UOBJ_LEATHER_TUNIC;
			UOBJ_STUDDED_TUNIC:
				item.graphic := UOBJ_STUDDED_FEMALE;
			UOBJ_STUDDED_FEMALE:
			UOBJ_STUDDED_BUSTIER:
				item.graphic := UOBJ_STUDDED_TUNIC;
			default:
				SendSysMessage (character, "Um, never mind.  I was thinking of a different type of item, I guess.");
				return;
		endcase
		SubtractAmount (found_hides, material_needed);
		return;
	endif

endfunction




///////////////////
//  fortify a hat with a helm
///////////////////

function FortifyHat (character, hat)

	SendSysMessage (character, "Fortify this hat with what?");
	var helm := Target (character);
	if (!helm)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	if (!IsAHelm (helm))
		SendSysMessage (character, "That's not a helmet.");
		return;
	endif

	if (!ReserveItem (helm))
		SendSysMessage (character, "That helm is not available right now.");
		return;
	endif

	if ( !InInventory (character.backpack, hat) or !InInventory (character.backpack, helm) )
		SendSysMessage (character, "Both the hat and the helm need to be in your backpack.");
		return;
	endif

	if (!CheckSkill (character, SKILLID_TAILORING, -1, 0))
		SendSysMessage (character, "You fail and ruin the hat.");
		DestroyItem (hat);
		return;
	endif

	PlaySoundEffect(character, SFX_TAILORING);
	sleep (5);
	PlaySoundEffect(character, SFX_TAILORING);
	sleep (3);

	helm.graphic := hat.graphic;
	helm.color := hat.color;
	var tempstring := TruncateArticle (helm.desc);
	helm.name := "a fortified " + TruncateArticle (hat.desc) + " (" + tempstring + ")";
	DestroyItem (hat);
endfunction




///////////////////
//  determines if the given item is a type of hide
///////////////////

function IsTypeOfHide (item)

	case (item.objtype)
		UOBJ_HIDE:
		UOBJ_BROWN_HIDE:
		UOBJ_BLUE_HIDE:
		UOBJ_RED_HIDE:
		UOBJ_GREEN_HIDE:
		UOBJ_SILVER_HIDE:
			return 1;
	endcase

	return 0;

endfunction




///////////////////
//  Determines if the given item objtype is a piece of leather armor
///////////////////

function IsClothItem (item)
	case (item.graphic)
		0x1F03:
		0x204E:
		0x153B:
		0x153D:
		0x1541:
		0x1EFD:
		0x1EFF:
		0x1F01:
		0x1F7B:
		0x1F9F:
		0x1FA1:
		0x1FFD:
		0x1515:
		0x1517:
		0x153A:
		0x1516:
		0x152E:
		0x1537:
		0x1539:
		0x153F:
		0x1543:
		0x1713:
		0x1714:
		0x1715:
		0x1718:
		0x1719:
		0x171A:
		0x171B:
		0x171C:
			return 1;
	endcase;

	return 0;

endfunction




///////////////////
//  Determines if the item objtype passed is a helm
///////////////////

function IsAHelm (item)

	case (item.objtype)
		0x1408:
		0x1409:
		0x140A:
		0x140B:
		0x140C:
		0x140D:
		0x140E:
		0x140F:
		0x1412:
		0x1419:
		0x1DB9:
		0x708b:
		0x13BB:
		0x13C0:
		0x708c:
		0x1451:
			return 1;
	endcase

	return 0;

endfunction

