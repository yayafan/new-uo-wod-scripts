use uo;
use os;
use util;

include "include/client";
include "include/objtype";
include "include/utility";
include "include/eventid";
include "include/accounts";
include "include/clock";
include "include/gumps";


///////////////////
//  figures out how many tickets the player has
///////////////////

function CountPlayersTickets (player_acctname, guild_name)
	var tickets := GetAccountProperty (player_acctname, guild_name + "_tickets");
	if (tickets)
		return tickets;
	endif
	return 0;
endfunction




///////////////////
//  adds or subtracts tickets from a player's account (pass a negative number to subtract)
///////////////////

function AddTicketsToPlayer (player_acctname, new_amount, guild_name)
	set_critical (1);

	var tickets := GetAccountProperty (player_acctname, guild_name + "_tickets");
	if (!tickets)
		tickets := 0;
	endif

	tickets := tickets + CINT (new_amount);
	SetAccountProperty (player_acctname, guild_name + "_tickets", tickets);

	set_critical (0);
endfunction




///////////////////
//  adds tickets to a monthly counter.  Only awarded tickets for completed tasks go here
///////////////////

function AddToMonthlyTicketCounter (player_acctname, new_amount, guild_name)
	set_critical (1);
	
	var year := GetYear ();
	var month := lower (GetMonth ());

	var monthly_tickets := GetAccountProperty (player_acctname, guild_name + "_tickets_" + year + "_" + month);
	if (!monthly_tickets)
		monthly_tickets := 0;
	endif
	monthly_tickets := monthly_tickets + CINT (new_amount);
	SetAccountProperty (player_acctname, guild_name + "_tickets_" + year + "_" + month, monthly_tickets);

	set_critical (0);
endfunction



///////////////////
//  determines if the player given is a member of the guild
///////////////////

function IsAGuildMember (player, guild_name)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 0;
	endif

	if (players_guild[1] == guild_name)
		return 1;
	endif

	return 0;
endfunction




///////////////////
//  determines whether the player can join a guild
///////////////////

function CanJoinGuild (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 1;
	endif

	//when a player quits a guild, they're guild is set to "none", and they can't join another guild for a week
	if (players_guild[1] == "none")
		if (!players_guild[2])
			return 1;
		endif
		if (players_guild[2] + 86400 < ReadGameClock () )
			return 1;
		endif
	endif

	return 0;
endfunction




///////////////////
//  set properties on the player for them to join the guild
///////////////////

function JoinGuild (player, guild_name, guild_title)
	var players_guild := {};
	players_guild[1] := guild_name;
	players_guild[2] := ReadGameClock ();
	players_guild[3] := ReadGameClock ();
	players_guild[4] := 0;
	SetObjProperty (player, "players_guild", players_guild);
	AddGuildRankingPoints (player, 1, guild_title);
endfunction




///////////////////
//  set properties on the player for them to quit their guild
///////////////////

function QuitGuild (player, guild_name)
	var players_guild := {};
	players_guild[1] := "none";
	players_guild[2] := ReadGameClock ();
	players_guild[3] := ReadGameClock ();
	SetObjProperty (player, "players_guild", players_guild);
	EraseObjProperty (player, "guild_quest");
	player.title_race := "";

	var tickets := CountPlayersTickets (player.acctname, guild_name);
	AddTicketsToPlayer (player.acctname, (0-tickets), guild_name);
endfunction




///////////////////
//  figures out if and how much money is owed to the guild
///////////////////

function DetermineMoneyOwed (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		return 0;
	endif

	var last_payment := players_guild[3];
	if (!last_payment)
		last_payment := ReadGameClock();
	endif

	var pasttime := ReadGameClock() - last_payment;
	if (pasttime < 0)
		return 0;
	endif

	var feeperiods := CINT (pasttime/604800);
	if (!feeperiods)
		return 0;
	endif

	return (feeperiods * 100);
endfunction




///////////////////
//  Pays off the guild fees
///////////////////

function PayOffGuildFees (player)
	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		Syslog ("Error trying to pay off guild dues!");
		return 0;
	endif

	players_guild[3] := ReadGameClock();
	SetObjProperty (player, "players_guild", players_guild);
endfunction




///////////////////
//  Adds points to a person's guild ranking and sets their title
///////////////////

function AddGuildRankingPoints (player, new_points, guild_title)
	new_points := CINT (new_points);

	var players_guild := GetObjProperty (player, "players_guild");
	if (!players_guild)
		player.title_race := "";
		return;
	endif

	var old_points := players_guild[4];
	if (!old_points)
		old_points := 0;
	endif

	old_points := old_points + new_points;
	if (old_points < 0)
		old_points := 0;
	endif
	players_guild[4] := old_points;
	SetObjProperty (player, "players_guild", players_guild);

	//Don't want GMs with titles
	if (player.cmdlevel)
		return;
	endif
	
	var guildtitle;
	case (players_guild[1])
		"adventurers_guild":
			guildtitle := "Jr. Pathfinder, AG";
			if (old_points > 50000)
				guildtitle := "Sr. Explorer, AG";
			elseif (old_points > 45000)
				guildtitle := "Explorer, AG";
			elseif (old_points > 40000)
				guildtitle := "Jr. Explorer, AG";
			elseif (old_points > 35000)
				guildtitle := "Sr. Pioneer, AG";
			elseif (old_points > 30000)
				guildtitle := "Pioneer, AG";
			elseif (old_points > 25000)
				guildtitle := "Jr. Pioneer, AG";
			elseif (old_points > 20000)
				guildtitle := "Sr. Guide, AG";
			elseif (old_points > 15000)
				guildtitle := "Guide, AG";
			elseif (old_points > 11000)
				guildtitle := "Jr. Guide, AG";
			elseif (old_points > 8000)
				guildtitle := "Sr. Trailblazer, AG";
			elseif (old_points > 4500)
				guildtitle := "Trailblazer, AG";
			elseif (old_points > 2000)
				guildtitle := "Jr. Trailblazer, AG";
			elseif (old_points > 1000)
				guildtitle := "Sr. Pathfinder, AG";
			elseif (old_points > 500)
				guildtitle := "Pathfinder, AG";
			endif
		default:
			player.title_race := "";
			return;
	endcase
	player.title_race := guildtitle;
endfunction















