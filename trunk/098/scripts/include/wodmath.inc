// Added by Jace

function Exp(base, power)

// Returns the exponent of <base> raised to <power>:
// Exp(2, 5) = 2^5 = 32

	var i;
	var x := 1;
	var check := Cint(power);
	if (power == check)
		for(i:=1;i<=power;i:=i+1)
			x := x * base;
		endfor
		return x;
	else
		var m, n;
		m := power - check;
		m := m * 1000;
		n := YRoot(base, 1000);
		x := Exp(n, m);
		return x;
	endif

endfunction

function YRoot(x, y)

// Returns the <y> root of a number <x>:
	
	var i := 0.000;
	var n := 0.000;
	var aroot := 2.000;
	var a := CInt(Sqr(x));

	if (a < 4)
		a := 4
;
	endif

	var success := 0;
	
	
	While (success == 0)		

		n := Exp(aroot, y);
		i := x - n;
		i := i / (a * aroot);
		i := Trunc(i, 3);
		aroot := aroot + i;
		if (i == 0)
			success := 1;
			if ( Exp(aroot, y) > x )
				aroot := aroot - 0.001;
			endif
			if ( Exp((aroot + 0.001), y) == x)
				aroot := aroot + 0.001;
			endif
		endif
	endwhile

	return aroot;

endfunction


function Sqr(x)

// Returns the square root of x.

	var i := 0.000;
	var n := 0.000;
	var aroot := 2.000;
	var success := 0;
	
	While (success == 0)		

		n := Exp(aroot, 2);
		i := x - n;
		i := i / (2 * aroot);
		i := Trunc(i, 3);
		aroot := aroot + i;
		if (i == 0)
			success := 1;
			if ( Exp(aroot, 2) > x )
				aroot := aroot - 0.001;
			endif
		endif
	endwhile

	return aroot;

endfunction


function Trunc(number, places)

// Truncates a dbl to a fixed amount of decimal places.

	var base := Exp(10, places);

	number := number * base;
	number := Cint(number);
	number := CDbl(number);
	number := number / base;

	return number;

endfunction

function Mod(Numerator, Denominator)

// New Mod, to appease Louds, Dundee, and other disgruntled elements :P
// Returns the modulus (remainder) of the fraction:
//
//		 Numerator
//		-----------
//		Denominator


	var dividend := Numerator - ( CInt(Numerator/Denominator) * Denominator );
	return dividend;
	
endfunction


function FlagCheck(flag, bit);

// This function will check the flag in the <bit> position of the
// <flag> register.  It will return a 1 if the flag is set, and a 0
// if the flag is reset.  Both <flag> and <bit> are integers.

	var FlagArray := BitOut(flag);
	bit := 9 - bit;

	if (FlagArray[bit] == 1)
		return 1;
	else
		return 0;
	endif

endfunction



function BitOut(x)

// This function takes an unsigned, short integer (0-255) and returns
// an array containing the individual bit values.   IMPORTANT: the 
// first element of the array is NOT the first bit, it is the 8th
// bit, and so on down.   See the desc for BitIn for the details.

	x := Cint(x);
	var loop := 1;
	var bitvalue := 0;
	var ArrayX := {0,0,0,0,0,0,0,0};
	var n;

	while (loop <= 8)

		n := 8 - loop;
		bitvalue := Exp(2, n);
		if ( (x - bitvalue) >= 0)
			ArrayX[loop] := 1;
			x := x - bitvalue;
		endif
		loop := loop + 1;

	endwhile

	return ArrayX;

endfunction

function FlagSet(flag, bit);

// This function will set the <bit> position in the <flag> register.
// Both <flag> and <bit> are integers.

	bit := 9 - bit;
	
	var FlagArray := BitOut(flag);
	FlagArray[bit] := 1;

	bit := BitIn(FlagArray);

	return bit;

endfunction

function FlagReset(flag, bit);

// This function will reset the <bit> position in the <flag> register.
// Both <flag> and <bit> are integers.

	bit := 9 - bit;
	
	var FlagArray := BitOut(flag);
	FlagArray[bit] := 0;

	bit := BitIn(FlagArray);

	return bit;

endfunction


function BitIn(ArrayX)

// Takes an array of 8 bits and converts them into an integer.
// This is used for the flag support.  IMPORTANT: bit positon is
// *NOT* the same thing as the array position!   The first bit in 
// the flag is the 8th bit in the array, and vice versa.  This is
// because it processes the bit values in the following order:
// 128, 64, 32, 16, 8, 4, 2, 1.

	var i := 8;
	var n;
	var loop := 1;
	var bitvalue := 0;

	While (i >= 0)

		n := 8 - i;
		if (ArrayX[i] == 1)
			bitvalue := bitvalue + Exp(2, n);
		endif
		i := i - 1;

	endwhile

	return bitvalue;

endfunction






