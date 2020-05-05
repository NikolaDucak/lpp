func fib( a ){
	if( a < 2 ) return a;
	return fib(a-1) + fib(a-2);
}
println( fib(15) );

