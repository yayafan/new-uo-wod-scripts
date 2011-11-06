///////////////////
//
//  Functions used in POL www pages
//
///////////////////

function WriteHTMLPageOpen (page_title)
	WriteHTML ("<html><head>");
	WriteHTML ("<title>" + page_title + "</title>");
	WriteHTML ("</head>");
	WriteHTML ("<body bgcolor=\"#dcdeed\">");
endfunction

function WriteHTMLPageClose ()
	WriteHTML ("</body></html>");
endfunction




///////////////////
//  Try to match a webpage hit with an online player, based on IP address
///////////////////

function FindOnlineAccountByIP (ipaddress)
	foreach character in EnumerateOnlineCharacters()
		if (character.ip == ipaddress)
			return character;
		endif
	endforeach
	return 0;
endfunction