#include "heapsort.h"

#include "event/Timer.h"
#include <iostream>
using namespace std;

#define BUFSIZE 30000

int compareMyType(const void* a, const void* b)
{
	if ( *(unsigned*)a <  *(unsigned*)b ) return -1;
	if ( *(unsigned*)a == *(unsigned*)b ) return 0;
	if ( *(unsigned*)a >  *(unsigned*)b ) return 1;
}

int main(int argc, char** argv)
{
	unsigned* buffer = new unsigned[BUFSIZE];
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = i;

	Timer t;
	heapsort(buffer, BUFSIZE);
	long long elapsed = t.micros();
	cout << elapsed << "us" << endl;

	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = i;

	Timer t2;
	qsort(buffer, BUFSIZE, sizeof(unsigned), &compareMyType);
	long long el2 = t2.micros();
	cout << el2 << "us" << endl;

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
	return 0;
}
