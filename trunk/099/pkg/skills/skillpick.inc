use os;
use uo;
use cfgfile;

include "include/client";
include "include/string";
include "include/attributes";

const LASTPAGE := 5;
const skillvalcolor := " 2400 ";
const skillnamecolor := " 0 ";

var selectskill_gump_layout := array {
	"nodispose",
	"page 0",
	"gumppic 0 50 500"
};
var selectskill_gump_data := array {
	""
};




///////////////////
//  Allows the character to select a new skill
///////////////////

function PickASkill (me)
	selectskill_gump_layout := array {
		"nodispose",
		"page 0",
		"gumppic 0 50 500"
	};
	selectskill_gump_data := array {
		""
	};

	var skills := array {};
	var skill := {};
	skill.+ value;
	skill.+ index;

	//organize them by skill value:
	for i := 0 to 57
		skill.value := SortValue (me, i);
		skill.index := i;
		skills[i+1] := skill;
	endfor

	//sort from high to low (thanks Myrathi!  nice function!)
	skills := SortMultiArrayByIndex (skills, 1);
	skills.reverse();

	foreach skill in skills
		selectskill_gump_data.append (GetSkillValDisplay (me, skill.index));
		selectskill_gump_data.append (GetSkillName (skill.index));
	endforeach

	//finish loading skillwinlayout
	var button;
	for pagenum := 1 to LASTPAGE
		LoadPageArray (pagenum);
		for i := 1 to 6
			button := ( (pagenum - 1) * 12 ) + i;
			LoadLeftPageArray (button, i, skills[button].index);
		endfor
		for i := 7 to 12
			button := ( (pagenum - 1) * 12 ) + i;
			LoadRightPageArray (button, i-6, skills[button].index);
		endfor
	endfor
	LoadPageArray (LASTPAGE);
	LoadLeftPageArray (49, 1, skills[49].index );//load the 49th skill

	var sreturn := SendDialogGump( me, selectskill_gump_layout, selectskill_gump_data );
	var snum := sreturn[0];
	if ( (snum) or (snum == 0) )
		return skills[snum].index;
	else
		return;
	endif
endfunction



function LoadPageArray (pagenum)
	if ( pagenum == 1 )
		selectskill_gump_layout.append ("page 1");
		selectskill_gump_layout.append ("button 356 50 502 502 0 2");//next page
	elseif ( pagenum == LASTPAGE )
		selectskill_gump_layout.append ("page " + cstr(LASTPAGE));
		selectskill_gump_layout.append ("button 0 50 501 501 0 "+ cstr(LASTPAGE-1));//previous page
	else
		selectskill_gump_layout.append ("page " + cstr(pagenum));
		selectskill_gump_layout.append ("button 0 50 501 501 0 " + cstr(pagenum - 1));//next
		selectskill_gump_layout.append ("button 356 50 502 502 0 "+ cstr(pagenum + 1));//previous
	endif
endfunction


function LoadLeftPageArray (button, i, skillid)
	var skillvaltext := cstr((button*2)-1);
	var skillnametext := cstr(button*2);
	var skillname := GetSkillName(skillid);
	if ( skillname == "None" )
		skillvaltext := " 0";
		skillnametext := " 0";
	endif

	selectskill_gump_layout.append ("text 50 " + cstr(80 + (20*i)) + skillvalcolor + skillvaltext);
	selectskill_gump_layout.append ("text 100 " + cstr(80 + (20*i)) + skillnamecolor + skillnametext);

	if ( skillname == "None" )
		return;
	endif

	var buttoncolor := " 2104 2103 ";
	if (IsTradeSkill(skillid))
		buttoncolor := " " + cstr(cint(0x939)) + " 2103 ";
	endif
	
	selectskill_gump_layout.append ("button 35 " + cstr(85 + (20*i)) + buttoncolor + "1 0 " + cstr( button ));

endfunction

function LoadRightPageArray (button, i, skillid)

	var skillvaltext := cstr((button*2)-1);
	var skillnametext := cstr(button*2);
	var skillname := GetSkillName(skillid);
	if ( skillname == "None" )
		skillvaltext := " 0";
		skillnametext := " 0";
	endif

	selectskill_gump_layout.append ("text 240 " + cstr(80 + (20*i)) + skillvalcolor + skillvaltext);
	selectskill_gump_layout.append ("text 290 " + cstr(80 + (20*i)) + skillnamecolor + skillnametext);

	if ( skillname == "None" )
		return;
	endif

	var buttoncolor := " 2104 2103 ";
	if (IsTradeSkill(skillid))
		buttoncolor := " " + cstr(cint(0x939)) + " 2103 ";
	endif

	selectskill_gump_layout.append ("button 225 " + cstr(85 + (20*i)) + buttoncolor + "1 0 " + cstr( button ));

endfunction




///////////////////
//  Makes sure that primary and secondary skills are listed first
///////////////////

function SortValue (character, skillid)
	if (GetSkillName (skillid) == "None")
		return 0;		//sort disabled skills to the end
	endif

	//The +1 makes sure enabled skills go in front of disabled ones
	return GetAttribute (character, GetAttributeIDBySkillID (skillid)) * 10 + 1;
endfunction
