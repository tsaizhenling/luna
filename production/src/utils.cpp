#include <iostream>
#include <set>
#include <sstream>
#include "utils.h"

using namespace std;

void printSetOfStrings(ostream& dataOut, set<string> setOfStrings) {
	for (auto str : setOfStrings)
	{
		dataOut << str << " ";
	}
	dataOut << endl;
}