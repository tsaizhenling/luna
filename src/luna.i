%module luna
%{
#include "classifier.h"
%}

%include "std_vector.i"
%include "std_set.i"
%include "std_string.i"

namespace std {
	%template(StringSet) set<string>;
	%template(StringSetSet) set<set<string> >;
	%template(StringSetVector) vector<set<string> >;
}

class Classifier {
public:
	Classifier(std::vector<set<string> > allProducts,int iter = 1500,int convit=150,double lambda = 0.9);
	std::set<set<string> >deriveKeywords();
};
