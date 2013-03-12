#include "heapsort.h"

#include <iostream>
using namespace std;

#define BUFSIZE 1500

int main(int argc, char** argv)
{
	int buffer[BUFSIZE];
	for (unsigned i = 0; i < BUFSIZE; ++i)
		buffer[i] = BUFSIZE-i;

	heapsort(buffer, BUFSIZE);
	for (unsigned i = 0; i < BUFSIZE; ++i)
	{
		if (i % 20 == 0)
			cout << endl;
		else
			cout << ",";
		cout << buffer[i];
	}
	cout << endl;
	return 0;
}
