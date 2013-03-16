//#define SOOPER_DEBUG
#include "algorithm/heapsort.h"
#include "algorithm/mergesort.h"

#include "event/Timer.h"
#include <cstring>
#include <iostream>
using namespace std;

#define BUFSIZE 480000

int compareMyType(const void* a, const void* b)
{
	if ( *(unsigned*)a <  *(unsigned*)b ) return -1;
	if ( *(unsigned*)a == *(unsigned*)b ) return 0;
	if ( *(unsigned*)a >  *(unsigned*)b ) return 1;
}

int main(int argc, char** argv)
{
	std::cout << "*** buffer size : " << BUFSIZE << " ***" << std::endl;

	unsigned* buffer = new unsigned[BUFSIZE];

	// HEAP
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = i;

	Timer t;
	heapsort(buffer, BUFSIZE);
	cout << "heap : " << t.micros() << "us (" << t.micros()*100.0/BUFSIZE << ")" << endl;
	cout << "       " << t.millis() << "ms" << endl;

	/*for (unsigned i = 0; i < BUFSIZE; ++i)
	{
		if (i % 20 == 0)
			cout << endl;
		else
			cout << ",";
		cout << buffer[i];
	}
	cout << endl;*/


	// QUICK (built in)
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = i;

	Timer t2;
	qsort(buffer, BUFSIZE, sizeof(unsigned), &compareMyType);
	cout << "qsort: " << t2.micros() << "us (" << t2.micros()*100.0/BUFSIZE << ")" << endl;
	cout << "       " << t2.millis() << "ms" << endl;

	// MERGE
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = i;

	Timer t3;
	mergesort(buffer, BUFSIZE);
	cout << "merge: " << t3.micros() << "us (" << t3.micros()*100.0/BUFSIZE << ")" << endl;
	cout << "       " << t3.millis() << "ms" << endl;

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
