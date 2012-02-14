#include "common/TestFace.h"
#include "graphics/NoU.h"

#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	NoU you;
	cout << you.yourFace().method() << endl;
	return 0;
}
