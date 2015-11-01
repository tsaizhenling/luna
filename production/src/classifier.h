#include <cmath>
#include <map>
#include <vector>
#include <string>
#include <set>
#include "matrix.h"

using namespace std;

template class Matrix<double>;

class Classifier {
public:
	Classifier(vector<set<string> > allProducts,int iter = 1500,int convit=150,double lambda = 0.9);
	set<set<string> >deriveKeywords(ostream& out);
protected:
	//iter is the maximum number of iterations
	//lambda is the damping factor
	int _iter;
    int _convit; // number of iterations to check for change of exemplars. if no change for convit number of iterations, terminate
	double _lambda;
	vector<set<string> > products;
	int* getIndexes(Matrix<double> S,double median, int N, ostream& out);
	static double jaccardIndex(set<string>set1,set<string>set2);
	static double transformedJaccardIndex( double JaccardIndex );	
	int* getIndexesForResponsibilitiesAndAvailabilities (Matrix<double> R, Matrix<double> A, Matrix<double> S, int N);
};