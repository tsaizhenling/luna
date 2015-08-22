#include <map>
#include <set>
#include <vector>
#include <iostream>
#include "utils.h"
#include "cluster.h"

using namespace std;

void Cluster::print() {
	cout << "keywords: ";
	printSetOfStrings(cout,this->getKeywords());

	cout << '*';
	set<string> words = keyProduct;
	for (auto word : words)
	{
		cout << word << " ";
	}
	cout << endl;
	for (auto words : allOtherProducts) {
		for (auto word : words)
		{
			cout << word << " ";
		}
		cout << endl;
	}
}

set<string> Cluster::getKeywords() {
	set<string> keywords;
	map<string, int> occurrences;
	set<string> words = keyProduct;
	for (auto word : words) {
		occurrences[word]++;
	}
	for (auto words : allOtherProducts) {
		for (auto word : words)
		{
			if (occurrences[word]>0)
			{
				occurrences[word]++;
			}
		}
	}
	for (auto kv : occurrences)
	{
		if (kv.second > (allOtherProducts.size()+1)/2)
		{
			keywords.insert(kv.first);
		}
	}

	return keywords;
}