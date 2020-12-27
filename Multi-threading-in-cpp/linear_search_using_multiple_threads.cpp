#include <bits/stdc++.h>
#include <thread>
using namespace std;

// CONSTANTS
#define MAX 16 
#define THREAD_MAX 4 
	
int a[MAX] = {1, 5, 7, 10, 12, 14, 15, 
			18, 20, 22, 25, 27, 30, 
			64, 110, 220};
			 
int key = 202; 
bool found = false; 
int current_thread = 0; 

void* ThreadSearch() 
{ 
	int num = current_thread++; 

	for (int i = num * (MAX / 4); i < ((num + 1) * (MAX / 4)); i++) 
	{ 
		if (a[i] == key)
		{ 
			found = true;
		}
	} 
} 

int main() 
{ 
	pthread_t thread[THREAD_MAX]; 

	for (int i = 0; i < THREAD_MAX; i++) 
	{ 
		pthread_create(&thread[i], NULL, ThreadSearch, (void*)NULL); 
	} 

	
    for (int i = 0; i < THREAD_MAX; i++) 
    { 
        pthread_join(thread[i], NULL); 
    } 
  
	if (found) 
		cout << "Key element found" << endl; 
	else
		cout << "Key not present" << endl; 
	return 0; 
} 
