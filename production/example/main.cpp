#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "classifier.h"
#include "utils.h"

using namespace std;

int main(int argc, char *argv[])
{

	ifstream testfile(argv[1]);
	string line;
	vector<set<string> > products;
	while (std::getline(testfile, line)) {
		stringstream ss(line);
		string s;
		set<string> words;
		while (getline(ss, s, ' ')) {
 			words.insert(s);
		}
		products.push_back(words);
	}
	testfile.close();

	Classifier classifier(products);

	//output the assignment
	set<set<string> > keywords = classifier.deriveKeywords(cout);

	
	ofstream outfile(argv[2]);
	for (auto query : keywords)
	{
		printSetOfStrings(outfile,query);
	}
	outfile.close();
}

