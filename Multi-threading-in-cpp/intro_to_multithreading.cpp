#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
using namespace std;
typedef long long int  ull;

void findEven(ull start, ull end, ull* EvenSum) 
{
	for (ull i = start; i <= end; ++i)
	{
		if (!(i & 1))
		{
			*(EvenSum) += i;
		}
	}
}

void findOdd(ull start, ull end, ull* OddSum) 
{
	for (ull i = start; i <= end; ++i)
	{
		if (i & 1)
		{
			(*OddSum) += i;
		}
	}
}

int main() 
{
	
	ull start = 0, end = 100;

	ull OddSum = 0;
	ull EvenSum = 0;

	// WITH THREAD
    
    thread t1(findEven, start, end, &(EvenSum));
    thread t2(findOdd, start, end, &(OddSum));

	t1.join();
	t2.join();
	
	
	// WITHOUT THREAD
	//findEven(start,end, &EvenSum);
	//findOdd(start, end, &OddSum);
    
	cout << "OddSum : " << OddSum << endl;
	cout << "EvenSum : " << EvenSum << endl;
	cerr << "\nTime elapsed: " << 1000 * clock() / CLOCKS_PER_SEC << "ms\n";

	return 0;
}
