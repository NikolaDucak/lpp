// for
for(var a = 5; a > 10; a = a+1){
	println("shouldn't be printent");
}

for(var a = 0; a < 5; a = a+1){
	println(a);
}

for(a =10; a < 40; a = a+10)
	println(a);

var a = 0;
for(; a < 3.5; a = a+0.5)
	println(a);


a = 1;
for(; a<9;){
	println(a);
	a = a+1;
}

for(; false; ){
	println("shouldn't be printent");
}
