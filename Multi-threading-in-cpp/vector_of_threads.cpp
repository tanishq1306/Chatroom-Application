#include <bits/stdc++.h>
#include <thread>

using namespace std;

void AccumulateRange(int &sum, int start, int end) 
{
	sum = 0;
	for (int i = start; i < end; i++) 
	{
		sum += i;
	}
}

template <class T>
void PrintVector(vector<T> input) 
{
	cout << "{ ";
	unsigned int count = 0;
	for (auto &n: input) 
	{
		count++;
		cout << n;
		if (count < input.size()) 
		{
			cout << ", ";
		}
	}
	cout << " }" << std::endl;
}

int main() 
{
	const int number_of_threads = 2;
	int number_of_elements = 100;
	int step = number_of_elements / number_of_threads;

	vector<thread> threads;
	vector<int> partial_sums(number_of_threads, 0);

	for (int i = 0; i < number_of_threads; i++) 
	{
		threads.push_back(thread(AccumulateRange, ref(partial_sums[i]), 
								(int)(i * step), (int)((i + 1) * step)));
	}

	for (thread &t : threads) 
	{
		if (t.joinable()) 
		{
			t.join();
		}
	}

	int total = accumulate(partial_sums.begin(), partial_sums.end(), int(0));

	//PrintVector(partial_sums);
	cout << "total: " << total << endl;

	return 0;
}
