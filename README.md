##Requirement
flex 2.5+  
bison 2.4+  
llvm 3.2  
os linux i386

##Building
    make

##Usage
    ./sprc [options] file

    Options:
      -S            Compile to ir file
      -s            Compile to asm file
      -c            Compile to object file, but do not link
      -o <file>     Place the output into <file>

##Example

####fibonacci.sp
	long main(){
		printL(fib(10));println(); //55
		printL(fib(20));println(); //6765
		printL(fib2(10));println(); //55
		printL(fib2(20));println(); //6765
		printL(fib2(40));println(); //102334155
	}
	
	// recursion
	long fib(long n){
		if(n < 0)
			return 0;
		if(n == 0)
			return 0;
		else if(n == 1)
			return 1;
		else
			return fib(n-1)+fib(n-2);
	}
	
	// loop
	[long res] fib2(long n){
		long a0 = 0;
		long a1 = 1;
		res = 0;
		if(n <= 0){
			res = 0;
			return;
		}
		if(n == 1){
			res = 1;
			return;
		}
		for(long i = 2; i <= n; i++){
			res = a0 + a1;
			a0 = a1;
			a1 = res;
		}
	}
