long main(){
	printL(fibonacci(10));println(); //55
	printL(fibonacci(20));println(); //6765

	printC('\u4e60');
	printC('\u8fd1');
	printC('\u5e73');
	println();
	return 0;
}

long fibonacci(long n){
	if(n < 0)
		return 0;
	if(n == 0)
		return 0;
	else if(n == 1)
		return 1;
	else
		return fibonacci(n-1)+fibonacci(n-2);
}
