// var access; get val;
var a = 5;
var b = 3.14;
var c;
var d = nil;
var f = true;
var g = "true";

println(a,b,c,d,f,g);

// var access; set val;
a = "newa";
b = false;
c;
d = nil;
f = 100;
g = 3.15;

println(a,b,c,d,f,g);

// var access; get & set;
a = g;
b = f;
c = d;
d = b;
f = a;
g = g;

println(a,b,c,d,f,g);

