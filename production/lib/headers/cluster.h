#include <set>
#include <vector>

using namespace std;

class Cluster
{
	public:
		set<string> keyProduct;
		vector<set<string> > allOtherProducts;
		void print();
		set<string> getKeywords();
};