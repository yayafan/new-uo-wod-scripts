

 //1001: Dismount (cliloc1=1075635,cliloc2=1075636)
 //1002: Disarm (cliloc1=1075637,cliloc2=1075638)
 //1005: Nightsight (cliloc1=1075643,cliloc2=1075644)
 //1006: Death Strike 
 //1007: Evil Omen 
 //1008: unknown? (GumpID 0x7556)
 //1009: Regeneration (cliloc1=1044106,cliloc2=1075106)
 //1010: Divine Fury 
 //1011: Enemy Of One 
 //1012: Stealth (cliloc1=1044107,cliloc2=1075655)
 //1013: Active Meditation (cliloc1=1044106,cliloc2=1075106)
 //1014: Blood Oath caster 
 //1015: Blood Oath curse 
 //1016: Corpse Skin 
 //1017: Mindrot 
 //1018: Pain Spike 
 //1019: Strangle 
 //1020: Gift of Renewal 
 //1021: Attune Weapon 
 //1022: Thunderstorm 
 //1023: Essence of Wind 
 //1024: Ethereal Voyage 
 //1025: Gift Of Life 
 //1026: Arcane Empowerment 
 //1027: Mortal Strike 
 //1028: Reactive Armor (cliloc1=1075812,cliloc2=1075813)
 //1029: Protection (cliloc1=1075814,cliloc2=1075815)
 //1030: Arch Protection (cliloc1=1075816,cliloc2=1075816)
 //1031: Magic Reflection (cliloc1=1075817,cliloc2=1075818)
 //1032: Incognito (cliloc1=1075819,cliloc2=1075820)
 //1033: Disguised 
 //1034: Animal Form
 //1035: Polymorph (cliloc1=1075824,cliloc2=1075820)
 //1036: Invisibility (cliloc1=1075825,cliloc2=1075826)
 //1037: Paralyze (cliloc1=1075827,cliloc2=1075828)
 //1038: Poison (cliloc1=0x0F8627, cliloc2=0x1069B1)
 //1039: Bleed (cliloc1=0x106a75,cliloc2=0x106a76)
 //1040: Clumsy (cliloc1=0x106a77,cliloc2=0x106a78)
 //1041: Feeble Mind (cliloc1=0x106a79,cliloc2=0x106a7a)
 //1042: Weaken (cliloc1=1075837,cliloc2=1075838)
 //1043: Curse (cliloc1=1075835,cliloc2=1075836)
 //1044: Mass Curse (cliloc1=0x106a7f,cliloc2=0x106a80)
 //1045: Agility (cliloc1=0x106a81,cliloc2=0x106a82)
 //1046: Cunning (cliloc1=0x106a83,cliloc2=0x106a84)
 //1047: Strength (cliloc1=0x106a85,cliloc2=0x106a86)
 //1048: Bless (cliloc1=0x106a87,cliloc2=0x106a88)

const BUFFICON_NIGHTSIGHT := "03E9";
const BUFFICON_STEALTH := "03F4";
const BUFFICON_MEDITATION := "03F5";
const BUFFICON_REACTIVE_ARMOR := "0404";
const BUFFICON_PROTECTION := "0405";
const BUFFICON_ARCHPROTECTION := "0406";
const BUFFICON_MAGIC_REFLECTION := "0407";
const BUFFICON_INCOGNITO := "0408";
const BUFFICON_POLYMORPH := "040B";
const BUFFICON_INVISIBILITY := "040C";
const BUFFICON_PARALYZE := "040D";
const BUFFICON_POISON := "040E";
const BUFFICON_BLEED := "040F";
const BUFFICON_CLUMSY := "0410";
const BUFFICON_FEEBLEMIND := "0411";
const BUFFICON_WEAKEN := "0412";
const BUFFICON_CURSE := "0413";
const BUFFICON_MASS_CURSE := "0414";
const BUFFICON_AGILITY := "0415";
const BUFFICON_CUNNING := "0416";
const BUFFICON_STRENGTH := "0417";
const BUFFICON_BLESS := "0418";
const BUFFICON_FLYING := "041E";

const BUFFSTATE_ENABLED := "0001";
const BUFFSTATE_DISABLED := "0000";

function BuffDebuffIcon(who, icon, state, time := 0)
	var packetheader := "DF";
	var packetbody := substr(hex(who.serial), 3, 8);
	while (len(packetbody) < 8)
		packetbody := "0" + packetbody;
	endwhile
	packetbody += icon;
	packetbody += state;
	if (state == BUFFSTATE_ENABLED)
		packetheader += "002E";
		packetbody += "00000000";
		packetbody += icon;
		packetbody += state;
		packetbody += "00000000";
		packetbody += substr(hex(time + 0x10000), 4, 4);
		packetbody += "000000";
		packetbody += BuffDebuffCliloc1(icon);
		packetbody += BuffDebuffCliloc2(icon);
		packetbody += "00000000000100000000";
	else
		packetheader += "000B";
	endif
	SendPacket(who, packetheader + packetbody);
endfunction

function BuffDebuffCliloc1 (icon)
	case (icon)
		BUFFICON_NIGHTSIGHT: return "001069B5";
		BUFFICON_STEALTH: return "000FEE8B";
		BUFFICON_MEDITATION: return "000FEE8A";
		BUFFICON_REACTIVE_ARMOR: return "00106A64";
		BUFFICON_PROTECTION: return "00106A66";
		BUFFICON_ARCHPROTECTION: return "00106A68";
		BUFFICON_MAGIC_REFLECTION: return "00106A69";
		BUFFICON_INCOGNITO: return "00106A6B";
		BUFFICON_POLYMORPH: return "00106A70";
		BUFFICON_INVISIBILITY: return "00106A71";
		BUFFICON_PARALYZE: return "00106A73";
		BUFFICON_POISON: return "000F8627";
		BUFFICON_BLEED: return "00106A75";
		BUFFICON_CLUMSY: return "00106A77";
		BUFFICON_FEEBLEMIND: return "00106A79";
		BUFFICON_WEAKEN: return "00106A7D";
		BUFFICON_CURSE: return "00106A7B";
		BUFFICON_MASS_CURSE: return "00106A7F";
		BUFFICON_AGILITY: return "00106A81";
		BUFFICON_CUNNING: return "00106A83";
		BUFFICON_STRENGTH: return "00106A85";
		BUFFICON_BLESS: return "00106A87";
		BUFFICON_FLYING: return "0010F881";
		default: return 0;
	endcase
	return 0;
endfunction

function BuffDebuffCliloc2 (icon)
	case (icon)
		BUFFICON_NIGHTSIGHT: return "001069B6";
		BUFFICON_STEALTH: return "001069C7";
		BUFFICON_MEDITATION: return "001067A2";
		BUFFICON_REACTIVE_ARMOR: return "00106A65";
		BUFFICON_PROTECTION: return "00106A67";
		BUFFICON_ARCHPROTECTION: return "00106A68";
		BUFFICON_MAGIC_REFLECTION: return "00106A6A";
		BUFFICON_INCOGNITO: return "00106A6C";
		BUFFICON_POLYMORPH: return "00106A6C";
		BUFFICON_INVISIBILITY: return "00106A72";
		BUFFICON_PARALYZE: return "00106A74";
		BUFFICON_POISON: return "001069B1";
		BUFFICON_BLEED: return "00106A76";
		BUFFICON_CLUMSY: return "00106A78";
		BUFFICON_FEEBLEMIND: return "00106A7a";
		BUFFICON_WEAKEN: return "00106A7E";
		BUFFICON_CURSE: return "00106A7C";
		BUFFICON_MASS_CURSE: return "00106A80";
		BUFFICON_AGILITY: return "00106A82";
		BUFFICON_CUNNING: return "00106A84";
		BUFFICON_STRENGTH: return "00106A86";
		BUFFICON_BLESS: return "00106A88";
		BUFFICON_FLYING: return "0010F9F7";
		default: return 0;
	endcase
	return 0;
endfunction

