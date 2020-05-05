//while

// entering
var a = true;
while( a ){
	println("enter");
	a = false;
}

// non entering
while( a ){
	println("shouldn't be printed");
}

// boolifying num 
// num != 0 is true
a = 2;
while( a ){
	println(a);
	a = 0;
}

// boolifying string
var b = "not empty";
while( b ){
	println(b);
	b = "";
}

