///////////////////
//
//  Assorted time-related functions
//  The date stuff is based on DateUtils Include file by Jouni Airaksinen, Mintus@Codefield.com
//
///////////////////

use os;
use uo;
use cfgfile;

Const SECONDS_PER_DAY := 86400;


///////////////////
//  returns the in-game time
///////////////////

function GetTimeofDay()
	var hours := CSTR (GetHour());
	if (len(hours) == 1)
		hours := "0" + hours;
	endif

	var minutes := CSTR (GetMinute());
	if (len(minutes) == 1)
		minutes := "0" + minutes;
	endif
	return (hours+":"+minutes);
endfunction


function GetTime ()
	var dayscfg := ReadConfigFile ("::days");
	if (!dayscfg)
		Syslog ("Unable to read days.cfg!");
		return 0;
	endif

	var elem := FindConfigElem (dayscfg, "WholeDay");
	var length := GetConfigInt (elem, "Length");
	var day_length := (60 * length); 

	var x := readgameclock();
	return (CINT (x%day_length));
endfunction

function GetTransition()
	var cfg := ReadConfigFile ( "::days" );
	var elem := FindConfigElem ( cfg , "Transition" );
	var length := GetConfigInt ( elem , "Length" );
	return length;
endfunction

function GetDayLength()
	var cfg := ReadConfigFile( "::days" );
	var elem := FindConfigElem( cfg , "WholeDay" );
	var length := GetConfigInt( elem , "Length" );
	return length;
endfunction

function GetHour()
	var time_sec := GetTime() * 24;
	var length := GetDayLength() * 60;
	var hours := CINT (time_sec / length);
	return hours;
endfunction

function GetMinute()
	var time_sec := GetTime();
	var length := (GetDayLength() * 60) * 1.0 ;

	var temp := length / time_sec;
	var raw_time := 24.0 / temp;

	var hours := cint(raw_time);
	var remainder := raw_time - hours;
	var minutes := CINT ( remainder * 60 );
	return minutes;
endfunction




///////////////////
//  Figures out what the current year is
///////////////////

function GetYear ()
	//Seconds since 1/1/1970
	var systime := polcore().systime;
	//Lets skip ahead to y2k
	systime := systime - 946684800;
	var current_year_counter := 0;
	while (systime > 31536000)
		if (current_year_counter%4 == 0)
			systime := systime - 31622400;
		else
			systime := systime - 31536000;
		endif
		current_year_counter := current_year_counter + 1;
	endwhile;
	return (2000 + current_year_counter);	
endfunction





///////////////////
//  Figures out what the current month is (by name)
///////////////////

function GetMonth ()
	var MonthDays := array { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	var MonthNames := array {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
	};

	//Seconds since 1/1/1970
	var systime := polcore().systime;
	//Lets skip ahead to y2k
	systime := systime - 946684800;

	var current_year_counter := 0;
	var current_month_counter := 1;
	var done := 0;
	repeat
		var seconds_in_this_month := 0;
		if (current_month_counter == 2)
			if (current_year_counter%4 == 0)
				seconds_in_this_month := SECONDS_PER_DAY * 29;
			else
				seconds_in_this_month := SECONDS_PER_DAY * 28;
			endif
		else
			seconds_in_this_month := SECONDS_PER_DAY * MonthDays[current_month_counter];
		endif

		if (systime > seconds_in_this_month)
			systime := systime - seconds_in_this_month;
			current_month_counter := current_month_counter + 1;
			if (current_month_counter > 12)
				current_month_counter := 1;
				current_year_counter := current_year_counter + 1;
			endif
		else
			done := 1;
		endif
	until (done);
	return MonthNames [current_month_counter];	
endfunction




///////////////////
//  Gets the current year_month, used for recording playtime information
///////////////////

function GetCurrentPlaytimeMonth ()
	var year := GetYear ();
	var month := lower (GetMonth ());
	
	return (year + "_" + month + "_play_time");
endfunction




///////////////////
//  Gets the previous year_month, used for recording playtime information
///////////////////

function GetPreviousPlaytimeMonth (previous_month_counter := 1)
	var year := GetYear ();
	var month := lower (GetMonth ());
	
	repeat
		case (month)
			"december":
				month := "november";
			"november":
				month := "october";
			"october":
				month := "september";
			"september":
				month := "august";
			"august":
				month := "july";
			"july":
				month := "june";
			"june":
				month := "may";
			"may":
				month := "april";
			"april":
				month := "march";
			"march":
				month := "february";
			"february":
				month := "january";
			"january":
				month := "december";
				year := year - 1;
			default:
				Syslog ("Error figuring out month " + month);
				return "";
		endcase
		previous_month_counter := previous_month_counter - 1;
	until (!previous_month_counter);

	return (year + "_" + month + "_play_time");
endfunction




///////////////////
//  Turns the number of seconds into a string describing it in time terms
///////////////////

function GetTimeDesc (seconds, showseconds := 0)
	seconds := CINT (seconds);
	var minutes := CINT (seconds/60);
	var hours := CINT (minutes/60);
	var days := CINT (hours/24);

	if (seconds < 60)
		if (showseconds)
			return (seconds + " seconds");
		else
			return "< 1 minute";
		endif
	endif
	
	hours := hours - days*24;
	minutes := minutes - (days*24 + hours)*60;
	
	var returnstring := "";
	if (days)
		if (days > 1)
			returnstring := returnstring + days + " days, ";
		else
			returnstring := returnstring + "1 day, ";
		endif
	endif
	
	if (hours or days)
		if (hours > 1)
			returnstring := returnstring + hours + " hours, ";
		else
			returnstring := returnstring + "1 hour, ";
		endif
	endif
	
	if (minutes or hours or days)
		if (minutes > 1)
			returnstring := returnstring + minutes + " minutes";
		else
			returnstring := returnstring +  "1 minute";
		endif
	endif
	
	if (showseconds)
		seconds := CINT (seconds%60);
		if (seconds > 1)
			returnstring := returnstring + ", " + seconds + " seconds, ";
		elseif (seconds == 1)
			returnstring := returnstring + ", " + "1 second, ";
		else
			returnstring := returnstring + ", " + "0 seconds, ";
		endif
	endif
	
	return returnstring;
endfunction




///////////////////
//  Turns the number of seconds into a string describing it in time terms - only shorter
///////////////////

function GetShortTimeDesc (seconds, showseconds := 0)
	seconds := CINT (seconds);
	var minutes := CINT (seconds/60);
	var hours := CINT (minutes/60);
	var days := CINT (hours/24);

	if (seconds < 60)
		if (showseconds)
			return (seconds + " sec.");
		else
			return "< 1 min.";
		endif
	endif
	
	hours := hours - days*24;
	minutes := minutes - (days*24 + hours)*60;
	
	var returnstring := "";
	if (days)
		returnstring := returnstring + days + "d ";
	endif
	
	if (hours or days)
		returnstring := returnstring + hours + "h ";
	endif
	
	if (minutes or hours or days)
		returnstring := returnstring + minutes + "m";
	endif
	
	if (showseconds)
		returnstring := returnstring + " " + seconds + "s";
	endif
	
	return returnstring;
endfunction




///////////////////
//  Turns the number of seconds into a string describing it in time terms, but only up to hours (no days)
///////////////////

function GetTimeDescInHours (seconds, showseconds := 0)
	seconds := CINT (seconds);
	var minutes := CINT (seconds/60);
	var hours := CINT (minutes/60);

	if (seconds < 60)
		if (showseconds)
			return (seconds + " seconds");
		else
			return "< 1 minute";
		endif
	endif
	
	minutes := minutes - hours*60;
	
	var returnstring := "";
	if (hours)
		if (hours > 1)
			returnstring := returnstring + hours + " hours, ";
		else
			returnstring := returnstring + "1 hour, ";
		endif
	endif
	
	if (minutes or hours)
		if (minutes > 1)
			returnstring := returnstring + minutes + " minutes";
		else
			returnstring := returnstring +  "1 minute";
		endif
	endif
	
	if (showseconds)
		seconds := CINT (seconds%60);
		if (seconds > 1)
			returnstring := returnstring + ", " + seconds + " seconds, ";
		elseif (seconds == 1)
			returnstring := returnstring + ", " + "1 second, ";
		else
			returnstring := returnstring + ", " + "0 seconds, ";
		endif
	endif
	
	return returnstring;
endfunction



