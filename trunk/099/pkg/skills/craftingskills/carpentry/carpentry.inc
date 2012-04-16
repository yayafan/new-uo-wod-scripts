///////////////////
//  Allows the user to try to repair a carpentry item
///////////////////

function RepairCarpentryItem (character, tool, item, repair_skill)
	if (!item.maxhp or !item.quality)
		SendSysmessage (character, "That item cannot be repaired.");
    if (tool)
    endif
		return;
	endif

	//if the item is in full repair, try to do special stuff
	if (item.hp >= item.maxhp + 1)
		if ( GetObjProperty (item,"ownerserial") )
			SendSysMessage (character, "That item is in full repair.");
			SendSysMessage (character, "That has an owner's mark!");
			return;
		endif
		DoCarpentryItemUpgrade (character, item, repair_skill);
		return;
	endif

	//make sure the item is good enough to be repairable
	if (item.hp <= 0)
		SendSysmessage (character, "The brittle material breaks when you handle it.");
		DestroyItem (item);
		return;
	endif

	//setup some variables
	var item_damaged_amount := item.maxhp - item.hp;

	//now determine how many material and how much skill it will take to repair this item
	var material_needed := CINT (item_damaged_amount/10);
	if (!material_needed)
		material_needed := 1;
	endif

	//allow the use to pick which logs to use
	SendSysmessage (character, "Target the logs to use:");
	var found_logs := Target (character);
	if (!found_logs)
		SendSysMessage (character, "Canceled.");
		return;
	endif
	if (found_logs.objtype != UOBJ_LOG)
		SendSysMessage (character, "That's not a log!");
		return;
	endif
	if (!ReserveItem (found_logs) )
		SendSysMessage (character, "You cannot use that right now.");
		return;
	endif
	if (!Accessible (character, found_logs) )
		SendSysMessage (character, "You can't reach that.");
		return;
	endif
	if (found_logs.amount < material_needed)
		SendSysMessage (character, "You need " + material_needed + " logs.  That's only " + found_logs.amount + "!");
		return;
	endif

	PlaySoundEffect(character, SFX_HAMMER);
	sleep (2);
	PlaySoundEffect(character, SFX_HAMMER);
	sleep (2);
	PlaySoundEffect(character, SFX_HAMMER);
	sleep (2);

	SubtractAmount (found_logs, material_needed);
	var total_damage := GetObjProperty (item, "total_damage");
	if (!total_damage)
		total_damage := 0;
	endif
	
	if (total_damage + item_damaged_amount > item.maxhp)
		DestroyItem (item);
		SendSysmessage (character, "The brittle material breaks when you handle it.");
		return;
	endif

	if (!CheckSkill (character, repair_skill, -1, 0) )
		total_damage := total_damage + RandomInt (10);
		SetObjProperty (item, "total_damage", total_damage);
		if (total_damage + item_damaged_amount > item.maxhp)
			DestroyItem (item);
			SendSysmessage (character, "The brittle material breaks when you handle it.");
			return;
		endif
		SendSysMessage (character, "You only manage to damage the item further...");
		return;
	endif

	var skill_bonus := CINT (GetAttribute (character, repair_skill)/4);
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
	item.hp := item.maxhp + 1;
	SendSysMessage (character, "You repair the item completely");
	return;
endfunction




///////////////////
//  Tries to upgrade the item that was selected
///////////////////

function DoCarpentryItemUpgrade (character, item, repair_skill)
	//make sure its an item that can be upgraded
	if (item.desc["xceptional"])
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	if (IsMagicalItem (item) )
		SendSysMessage (character, "That item is in full repair.");
		return;
	endif

	var smith_cfg_file := ReadConfigFile( ":carpentry:carpentry" );
	var elem := FindConfigElem (smith_cfg_file, item.graphic);
	if (!elem)
		SendSysMessage (character, "I don't know how to repair that.");
		return;
	endif

	var material_needed := cint (elem.material/2);
	var item_name := elem.name;
	var item_skill := elem.skill + 20;
	if (item.desc["quality"])
		item_skill := item_skill + 10;
	endif
	if (item_skill > 110)
		item_skill := 110;
	endif

	SendSysMessage (character, "That item is in full repair.");
	SendSysMessage (character, "Upgrading that item will take " + material_needed + " logs and " + item_skill + " skill.");
	if (item_skill >= GetAttribute (character, GetAttributeIDBySkillID (repair_skill)) + 20)
		SendSysMessage (character, "Your skill is too low to upgrade that item.");
		return;
	endif

	var found_lumber := FindObjtypeInContainer (character.backpack, UOBJ_LOG);
	if (!found_lumber or found_lumber.amount < material_needed)
		SendSysMessage (character, "You don't have enough logs to upgrade this item.");
		return;
	endif

	if (!ReserveItem (found_lumber) )
		SendSysMessage (character, "You can't do this right now.");
		return;
	endif

	var confirm_upgrade := YesNo (character, "Upgrade?");
	if (!confirm_upgrade)
		SendSysMessage (character, "Canceled.");
		return;
	endif

	PerformCarpentryItemUpgrade (character, item, found_lumber, material_needed, repair_skill, item_skill, item_name);
endfunction




///////////////////
//  this function does the actual item upgrade
///////////////////

function PerformCarpentryItemUpgrade (character, item, found_lumber, material_needed, repair_skill, item_skill, item_name)
	var sx := character.x;
	var sy := character.y;

	repeat
		for i := 1 to 4
			PlaySoundEffect(character, SFX_HAMMER);
			sleep (2);
		endfor
		PlaySoundEffect(character, SFX_HAMMER);

		if (CheckSkill (character, repair_skill, item_skill, 0) )
			SendSysMessage (character, "You succeed in upgrading the item.");
			SubtractAmount (found_lumber, material_needed);

			if (item.desc ["quality"])
				item.name := "an exceptional " + TruncateArticle(item_name) + " [crafted by " + character.name + "]";
				item.quality := 1.2;
				item.hp := item.maxhp + 1;
				item.buyprice := 0;
				return;
			else
				item.name := "a quality " + TruncateArticle(item_name) + " [crafted by " + character.name + "]";
				item.quality := 1.0;
				item.hp := item.maxhp + 1;
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
			SubtractAmount (found_lumber, RandomInt(material_needed) );
			if (RandomInt (10) == 0)
				SendSysMessage (character, "You ruin the item trying to upgrade it.");
				DestroyItem (item);
				return;
			else
				SendSysMessage (character, "You destroy some material.");
			endif
		endif

		if (!found_lumber or found_lumber.amount < material_needed)
			SendSysMessage (character, "You don't have enough material to continue.");
			return;
		endif

	until (character.x != sx or character.y != sy);

endfunction

