use uo;
use os;

function CreateMoongates()
  var moongates := {
	{4467,	1283,	5, "Moonglow"},		// moonglow
    {1336,	1997,	5, "Britain"},		// britain
    {1499,	3771,	5, "Jhelom"},		// jehlom
	{771,	753,	5, "Yew"},		    // yew
    {2701,	693,	5, "Minoc"},		// minoc
    {1828,	2948, -20, "Trinsic"},	    // trinsic
    {643,	2067,	5, "Skara"},		// skara
    {3564,	2140,  34, "Magincia"} 	    // magincia
  };
  var moongate;
  var index := 1;
  syslog( "Creating moongates." );

  var totaldestroyed := 0;

  foreach gate in moongates
	var results := ListItemsNearLocationOfType( gate[1], gate[2], LIST_IGNORE_Z, 25, "systemmoongate", realm := _DEFAULT_REALM );
	foreach thing in results
        	totaldestroyed := totaldestroyed + 1;
		DestroyItem(thing);
	endforeach

	results := ListItemsNearLocationOfType( gate[1], gate[2], LIST_IGNORE_Z, 25, "systemteleporter", realm := _DEFAULT_REALM );
	foreach thing in results
		totaldestroyed := totaldestroyed + 1;
		DestroyItem(thing);
	endforeach
  endforeach

  var createdgates := 0;
  foreach gate in moongates
	moongate := CreateItemAtLocation( gate[1], gate[2], gate[3], "systemmoongate", amount := 1, realm := _DEFAULT_REALM );
	SetObjProperty(moongate, "City", gate[4]);
	moongate.facing := 29;
    	moongate.decayAt := 0;
    	moongate.movable := 0;
	createdgates := createdgates + 1;
  endforeach
  syslog("Destroyed: (" + totaldestroyed + ") Moongates Recreated (" + createdgates + ")" );
endfunction
