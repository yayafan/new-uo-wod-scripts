use uo;
use os;
use util;

include "include/client";
include "include/objtype";
include "include/itemtypes";
include "include/utility";

Const MAX_AUCTIONS_PER_CATEGORY := 3;

Const BUTTON_LAST_ITEM		:= 1000;
Const BUTTON_NEXT_ITEM		:= 1001;
Const BUTTON_EVALUATE_ITEM	:= 1002;

var storage := find_or_create_storage ( "Auction Storage" );
if (!storage)
	exit;
endif



///////////////////
//  figures out how much gold the auctioneer is holding for the player
///////////////////

function DetermineGoldHeld (acctname)
	var gold_array := GetGlobalProperty ("auctioneer_gold");
	if (!gold_array)
		gold_array := dictionary;
	endif

	if (!gold_array.exists (acctname) )
		return 0;
	else
		return CINT (gold_array[acctname]);
	endif
endfunction




///////////////////
//  adds or subtracts gold from a player's account (pass a negative number to subtract).  This
//  function doesn't check the numbers, so its possible to go below 0.
///////////////////

function AddAmountToGoldHeld (acctname, new_amount)
	set_critical (1);
	new_amount := CINT (new_amount);
	var gold_array := GetGlobalProperty ("auctioneer_gold");
	if (!gold_array)
		gold_array := dictionary;
	endif

	var gold_amount;
	if (!gold_array.exists (acctname) )
		gold_amount := 0;
	else
		gold_amount := CINT (gold_array[acctname]);
	endif

	gold_array [acctname] := gold_amount + new_amount;
	SetGlobalProperty ("auctioneer_gold", gold_array);
	set_critical (0);
endfunction




///////////////////
//  determines the amount of gold the player has available to use (When a player bids on an item, that gold
//  is marked as unavailable)
///////////////////

function DetermineGoldAvailable (acctname)
	var gold_held := DetermineGoldHeld (acctname);
	var gold_reserved := 0;
	foreach category in ListCategoryNames ()
		var category_box := find_or_create_item (storage, category, UOBJ_BANKBOX);
		foreach item in ListRootItemsInContainer (category_box)
			var current_high_bidder := GetObjProperty (item, "current_high_bidder");
			if (current_high_bidder and current_high_bidder == acctname)
				gold_reserved := gold_reserved + CINT (GetObjProperty (item, "current_high_bid"));
			endif
		endforeach
	endforeach

	return (gold_held - gold_reserved);
endfunction




///////////////////
//  builds the gump for the category selection menu
///////////////////

function SelectCategory (player, message := "What kind of items do you want to look at?")
	var categories := CreateMenu (message);
	foreach category_description in ListCategoryDescriptions ()
		AddMenuItem (categories, 0, category_description);
	endforeach
	var cat_select := SelectMenuItem2 (player, categories);
	if (!cat_select)
		return 0;
	endif
	
	var category_names := ListCategoryNames ();
	
	return category_names[cat_select.index];
endfunction




///////////////////
//  returns a listing of the auction category names (behind the scenes names)
///////////////////

function ListCategoryNames ()
	return array {
		"auc_weapons_general",
		"auc_armor_general",
		"auc_clothing_general",
		"auc_decorative_general",
		"auc_other_general"
	};
endfunction




///////////////////
//  returns a listing of the auction category descriptions (names the player sees)
///////////////////

function ListCategoryDescriptions ()
	return array {
		"Weapons",
		"Armor",
		"Clothing",
		"Decorative",
		"Misc."
	};
endfunction




///////////////////
//  Checks all the auctions to see if which ones have been completed
///////////////////

function CheckCompletedAuctions ()
	start_script (":auctioneer:completeauctions", 0);
endfunction




///////////////////
//  does thw grunt work of building the gump
///////////////////

function BuildAuctionDisplayGump (byref item, byref player)
	gump_layout := array {
		"nodispose",
		"page 0",
		"resizepic 20 20 2600 500 380",
		"resizepic 60 60 5120 60 60",
		"tilepic 70 70 " + item.graphic,

		"text 140 70 1327 0",			//"Item:"
		"text 200 70 0 1",				//First line of item name
		"text 220 85 0 2",				//Second line of item name
		
		"text 140 110 1327 3",			//"Seller:"
		"text 200 110 0 4",				//Seller name

		"text 100 135 1327 5",			//"Current Bid:"
		"text 200 135 0 6",				//the current bid

		"text 100 160 1327 7",			//"Ends in:"
		"text 200 160 0 8",				//Time left

		"croppedtext 100 190 350 20 0 9",				//First line of description
		"croppedtext 100 210 350 20 0 10",			//Second line of description
		"croppedtext 100 230 350 20 0 11",			//Third line of description
		"croppedtext 100 250 350 20 0 12",			//Fourth line of description

		"text 150 330 1327 13",			//Gold held string
		"text 300 330 0 14",			//Gold held amount
		"text 150 350 1327 15",			//Gold available string
		"text 300 350 0 16",			//Gold available amount

		"text 120 280 0 17",			//"Bid on item"
		"text 320 280 0 18",			//"Evalute item"
		"button 60 200 5603 5607 1 0 " + BUTTON_LAST_ITEM,
		"button 480 200 5601 5605 1 0 " + BUTTON_NEXT_ITEM,
		"button 300 285 2104 2103 1 0 " + BUTTON_EVALUATE_ITEM
	};
	
	gump_data := array {
		"Item:",						//0
		item.desc,
		"",
		"Seller:",
		GetObjProperty (item, "sellername"),

		"Current bid:",				//5
		GetObjProperty (item, "minbid"),
		"Ends in:",
		"",
		"",
		"",							//10
		"",
		"",
		
		"Gold Held:",
		players_gold_held,
		"Gold Available:",				//15
		players_gold_available,
		"Bid on Item",
		"Evaluate Item"
	};
	
	//Split the name into 2 parts if its too long for one line
	var itemname := item.desc;
	if (len (itemname) > 40)
		var splitted := SplitWords (itemname);
		var itemname1 := "";
		var itemname2 := "";
		var done := 0;
		foreach word in splitted
			if (!done)
				if (len (itemname1) + len (word) < 40)
					if (len (itemname1))
						itemname1 := itemname1 + " " + word;
					else
						itemname1 := word;
					endif
				else
					itemname2 := word;
					done := 1;
				endif
			else
				itemname2 := itemname2 + " " + word;
			endif
		endforeach
		gump_data[2] := itemname1;
		gump_data[3] := itemname2;
	endif

	//Calculate the time left
	var time := CINT (GetObjProperty (item, "endtime")) - ReadGameClock();
	var days := CINT (time/86400);
	var hours := CINT ((time - days*86400)/3600);
	var minutes := CINT ( (time - days*86400 - hours*3600)/60);

	gump_data[9] := (days + " days, " + hours + " hours, " + minutes + " minutes");
	
	//Put in the item's description, if it has one
	var description := GetObjProperty (item, "auctiondescription");
	if (!description)
		description := {"", "", "", ""};
	endif
	for i := 1 to 4
		gump_data[9+i] := description[i];	
	endfor
	
	//Add the buy button
	if (player.acctname != GetObjProperty (item, "selleracctname"))
		gump_layout.append ("button 100 285 2104 2103 1 0 " + (current_item_num));
	elseif (!GetObjProperty (item, "current_high_bidder"))
		gump_layout.append ("button 100 285 2104 2103 1 0 " + (current_item_num));
		gump_data[18] := "Remove item from auction";
	else
		gump_data[18] := "This item has already been bid on.";
	endif
endfunction




///////////////////
//  Erases all the properties set on the item when its put up for auction
///////////////////

function EraseAllProperties (item)
	EraseObjProperty (item, "endtime");
	EraseObjProperty (item, "minbid");
	EraseObjProperty (item, "sellerserial");
	EraseObjProperty (item, "selleracctname");
	EraseObjProperty (item, "sellername");
	EraseObjProperty (item, "current_high_bidder");
	EraseObjProperty (item, "current_high_bid");
	EraseObjProperty (item, "auctiondescription");
endfunction




///////////////////
//  Maintains a global list of transaction messages for each account
///////////////////

function AddTransactionMessage (acctname, message)
	set_critical (1);
	var allmessages := GetGlobalProperty ("auctioneer_transaction_messages");
	if (!allmessages)
		allmessages := dictionary;
	endif
	
	var acctmessages := {};
	if (allmessages.exists (acctname))
		acctmessages := allmessages[acctname];
	endif
	
	acctmessages.append (message);
	allmessages [acctname] := acctmessages;
	SetGlobalProperty ("auctioneer_transaction_messages", allmessages);
	set_critical (0);
endfunction




///////////////////
//  Erase the message log for an account
///////////////////

function EraseTransactionMessages (acctname)
	set_critical (1);
	var allmessages := GetGlobalProperty ("auctioneer_transaction_messages");
	if (!allmessages)
		set_critical (0);
		return;
	endif

	if (!allmessages.exists (acctname))
		set_critical (0);
		return;
	endif

	allmessages.erase (acctname);
	SetGlobalProperty ("auctioneer_transaction_messages", allmessages);
	set_critical (0);
endfunction




///////////////////
//  Retrieves the messages for an account
///////////////////

function GetTransactionMessages (acctname)
	var allmessages := GetGlobalProperty ("auctioneer_transaction_messages");
	if (!allmessages)
		return {};
	endif

	if (allmessages.exists (acctname))
		return allmessages [acctname];
	endif

	return {};
endfunction




///////////////////
//  Figures out the proper commision on a sliding scale
///////////////////

function CalculateCommision (price)
	//10% commission on the first 1000 gold
	if (price <= 1000)
		return CINT (price * 0.1);
	endif
	var commission := 100;
	price := price - 1000;

	//5% commission up to 10,000 gold (next 9,000)
	if (price <= 9000)
		commission := commission + CINT (price * 0.05);
		return commission;
	endif
	commission := commission + 450;
	price := price - 9000;

	//2.5% commission up to 100,000 gold (next 90,000)
	if (price <= 90000)
		commission := commission + CINT (price * 0.025);
		return commission;
	endif
	commission := commission + 2250;
	price := price - 90000;
	
	//after that, 1% commission
	commission := commission + CINT (price * 0.01);
	return commission;
endfunction








