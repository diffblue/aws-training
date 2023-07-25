/*
../cbmc/cmake-build-release/bin/cbmc ./numerical_benchmark.c --unwind 250 --slice-formula --trace --incremental-smt2-solver "z3 --smt2 -in" --cover location
real	0m45.170s
user	0m33.256s
sys	0m0.795s

time ../cbmc/cmake-build-release/bin/cbmc ./numerical_benchmark.c --unwind 250 --slice-formula --trace --cover location
real	12m9.538s
user	12m8.103s
sys	0m0.903s
*/

#include <assert.h>
#include <stdbool.h>

bool isPrime(unsigned int x)
{
	for(unsigned int i = 2; i < x; ++i)
	{
		if(x % i == 0)
			return false;
	}
	return true;
}

int main()
{
	unsigned int a, b;
	__CPROVER_assume(a > 1 && a < 250);
	__CPROVER_assume(isPrime(a));
	__CPROVER_assume(b > 1 && b < 250);
	__CPROVER_assume(isPrime(b));
	unsigned int result = a * a * a + b * b * b;
	assert(result != 152);  // 3^3 + 5^3
	assert(result != 793802); // 79^3 + 67^3
	assert(result != 22043268); // 233^3 + 211^3
}
