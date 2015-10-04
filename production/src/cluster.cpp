#include <map>
#include <set>
#include <vector>
#include "utils.h"
#include "cluster.h"

using namespace std;

void Cluster::print(ostream& out) {
	out << "keywords: ";
	printSetOfStrings(out,this->getKeywords());

	out << '*';
	set<string> words = keyProduct;
	for (auto word : words)
	{
		out << word << " ";
	}
	out << endl;
	for (auto words : allOtherProducts) {
		for (auto word : words)
		{
			out << word << " ";
		}
		out << endl;
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