#include <set>
#include <vector>
#include <iostream>

using namespace std;

class Cluster
{
	public:
		set<string> keyProduct;
		vector<set<string> > allOtherProducts;
		void print(ostream& out = cout);
		set<string> getKeywords();
};