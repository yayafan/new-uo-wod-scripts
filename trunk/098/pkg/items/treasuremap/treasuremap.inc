include "include/locations";
include "../pkg/combat/loot/loot";

Const UOBJECT_TREASURE_CHEST := 0x0E40;

///////////////////
//  called by mining when a map is targetted
///////////////////

function TryToDigUpTreasureMap (character, map)

	if (!ReserveItem (map))
		return;
	endif

	var chestx := GetObjProperty (map, "chestx");
	var chesty := GetObjProperty (map, "chesty");

	if(!chestx or !chesty)	//z might be 0!
		SendSysmessage (character, "That is not a treasure map.");
		return;
	endif

	//if this isn't the person who decoded the map, they need to do it again
	if (character.serial != GetObjProperty (map, "decoder") )
		DecodeMapAgain (character, map);
		return;
	endif

	var range := GetAttribute (character, ATTRIBUTEID_MINING)/10 + 1;
	if (!range)
		range := 2;
	elseif (range > 10)
		range := 10;
	endif

	var distn := CoorDist (character.x, character.y, chestx, chesty);
	var direction := GetDirection (character, chestx, chesty);

	if (distn > range)
		SendSysmessage (character, "This map leads " + direction);
		return;
	endif

	var createdx := character.x;
	var createdy := character.y;

	var chest := CreateTreasureChest (map, createdx, createdy, character.z - 5 );
	if (!chest) 
		SendSysmessage (character, "This map is a fake!");
		DestroyItem (map);
		return;
	endif

	if (!ReserveItem (chest))
		return;
	endif
	var z := character.z;

	chest.locked := 1;
	var i;
	for (i := 5; i >= 0; i := i - 1)
		PerformAction (character, 0x0B);
		PlaySoundEffect (character, 0x126);
		sleep (1);
		chest.movable := 1;
		MoveObjectToLocation( chest, createdx, createdy, z - i, realm := _DEFAULT_REALM, flags := MOVEOBJECT_FORCELOCATION );
		chest.movable := 0;
	endfor

	DestroyItem (map);
	ReleaseItem (chest);
	detach ();
	sleep (1800);
	DestroyItem (chest);

endfunction




///////////////////
//  decodes an already decoded map, if the person who's digging it up isn't the person who decoded it
//  (Anti-mule code)
///////////////////

function DecodeMapAgain (character, map)

	var maplevel := GetObjProperty (map, "level");
	var diffi := maplevel * 10;

	if (diffi > GetAttribute (character, ATTRIBUTEID_CARTOGRAPHY)+20)
		SendSysmessage (character, "You cannot comprehend the map.");
		return;
	endif

	if (!CheckSkill (character, SKILLID_CARTOGRAPHY, diffi, 0) )
		SendSysmessage( character , "You fail to decode the map..." );
		return;
	endif

	SetObjProperty (map, "decoder", character.serial);
	SendSysmessage (character, "You successfully decode the map.");

endfunction




///////////////////
//  creates a treasure chest, or, for seer created maps, finds the pre-buried one
///////////////////

function CreateTreasureChest (map, x, y, z)

	var chest;
	if (GetObjProperty (map, "chestserial"))
		chest := SystemFindObjectBySerial (GetObjProperty (map, "chestserial"));
	endif

	//if there wasn't a previously defined map, or else if it couldn't be found
	if (!chest)
		chest := CreateItemAtLocation (x, y, z, UOBJECT_TREASURE_CHEST, 1);
		var lvl := GetObjProperty (map, "level");
		SetObjProperty (chest, "level", lvl);
		var lootgroup := "treasuremapchest" + CSTR (lvl);
		MakeLootInChest (chest, lootgroup);
	endif

	chest.usescript := ":treasuremap:treasurechest";
	return chest;

endfunction
