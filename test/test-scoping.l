var a = 1;

{
	var a1 = -1;
	println(a);
	var a = 2;
	println(a);

	{
		println(a);
		var a = 3;
		println(a);

		var a2= -2;
		println(a1);
		
		{
			println(a);
			var a = 4;
			println(a);

			println(a1);
			println(a2);
		}
	}
}
