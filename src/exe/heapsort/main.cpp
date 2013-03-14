#include "algorithm/heapsort.h"
#include "algorithm/mergesort.h"

#include "event/Timer.h"
#include <cstring>
#include <iostream>
using namespace std;

#define BUFSIZE 300000

int compareMyType(const void* a, const void* b)
{
	if ( *(unsigned*)a <  *(unsigned*)b ) return -1;
	if ( *(unsigned*)a == *(unsigned*)b ) return 0;
	if ( *(unsigned*)a >  *(unsigned*)b ) return 1;
}

int main(int argc, char** argv)
{
	unsigned* buffer = new unsigned[BUFSIZE];

	// MERGE
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = BUFSIZE-i;
	unsigned* workBuffer = new unsigned[BUFSIZE];
	::memset(workBuffer, 0, BUFSIZE);

	Timer t3;
	mergesort(buffer, workBuffer, BUFSIZE);
	long long el3 = t3.micros();
	cout << "merge: " << el3 << "us" << endl;


	// HEAP
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = i;

	Timer t;
	heapsort(buffer, BUFSIZE);
	long long elapsed = t.micros();
	cout << "heap : " << elapsed << "us" << endl;


	// QUICK (built in)
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = i;

	Timer t2;
	qsort(buffer, BUFSIZE, sizeof(unsigned), &compareMyType);
	long long el2 = t2.micros();
	cout << "qsort: " << el2 << "us" << endl;

	/*for (unsigned i = 0; i < BUFSIZE; ++i)
	{
		if (i % 20 == 0)
			cout << endl;
		else
			cout << ",";
		cout << buffer[i];
	}
	cout << endl;*/

	delete[] buffer;
	delete[] workBuffer;
	return 0;
}
