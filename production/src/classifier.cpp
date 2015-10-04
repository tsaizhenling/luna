#include <cmath>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "cluster.h"
#include "utils.h"
#include "classifier.h"

using namespace std;

Classifier::Classifier(vector<set<string> > allProducts,int iter,int convit,double lambda) 
: 
products(allProducts),
_iter(iter),
_lambda(lambda),
_convit(convit) {
}

set<set<string> >Classifier::deriveKeywords() {
	int N = products.size();
	Matrix<double> S(N,N);
	vector<double> tmpS;

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (j >= i)
			{
				double similarity = transformedJaccardIndex(jaccardIndex(products[i],products[j]));
				S(i,j) = similarity;
				S(j,i) = S(i,j);
				tmpS.push_back(S(i,j)); 
			}
		}
	}
	
	sort(tmpS.begin(), tmpS.end());
	double median = 0;
	
	if(tmpS.size()%2==0) 
		median = (tmpS[tmpS.size()/2]+tmpS[tmpS.size()/2-1])/2;
	else 
		median = tmpS[tmpS.size()/2];
	
	int *last_idx = getIndexes(S,median,N);

	// build the clusters
	std::map <int, Cluster > clusters;
	for (int i = 0; i < N; ++i) {
		int index = last_idx[i];
		Cluster cluster = clusters[index];
		if (index == i)
		{
			cluster.keyProduct = products[i];
		} else {
			cluster.allOtherProducts.push_back(products[i]);
		}
		clusters[index] = cluster;
	}

	int i = 1;
	//output the assignment
	set<set<string> > keywords;
	for(auto kv : clusters) {
		Cluster cluster = kv.second;
		cout << "========cluster:" << i << "=========" << endl;
		cluster.print();
		keywords.insert(cluster.getKeywords());
		++i;
	}
	return keywords;
}

int* Classifier::getIndexesForResponsibilitiesAndAvailabilities (Matrix<double> R, Matrix<double> A, Matrix<double> S, int N) {
	//find the exemplar
    set<int> *center = new set<int>;
    for(int i=0; i<N; i++) {
        double E = R(i,i) + A(i,i);
        if(E>0) {
            center->insert(i);
        }
    }
	//data point assignment, idx[i] is the exemplar for data point i
    int *idx = new int[N];
    memset(idx, 0, N*sizeof(int));
    for(int i=0; i<N; i++) {
        int idxForI = 0;
        double maxSim = -1e100;
        for(auto c : *center) {
            if (i == c) {
                idxForI = c;
                break;
            }
        	if (S(i,c)>maxSim) {
            	maxSim = S(i,c);
            	idxForI = c;
        	}
    	}
    	idx[i] = idxForI;
    }
    return idx;
}

int* Classifier::getIndexes(Matrix<double> S,double median, int N) {
	//N is the number of two-dimension data points
	//S is the similarity matrix
	//R is the responsibility matrix
	//A is the availabiltiy matrix
	
	Matrix<double> R(N,N);
	Matrix<double> A(N,N);

	//compute preferences for all data points: median 
	for(int i=0; i<N; i++) S(i,i) = median;
    int *last_idx = new int[N];
    memset(last_idx, 0, N*sizeof(int));
    
	for(int m=0; m<_iter; m++) {
	//update responsibility
		for(int i=0; i<N; i++) {
			for(int k=0; k<N; k++) {
				double max = -1e100;
				for(int kk=0; kk<k; kk++) {
					if(S(i,kk)+A(i,kk)>max) 
						max = S(i,kk)+A(i,kk);
				}
				for(int kk=k+1; kk<N; kk++) {
					if(S(i,kk)+A(i,kk)>max) 
						max = S(i,kk)+A(i,kk);
				}
				R(i,k) = (1-_lambda)*(S(i,k) - max) + _lambda*R(i,k);
			}
		}
	//update availability
		for(int i=0; i<N; i++) {
			for(int k=0; k<N; k++) {
				if(i==k) {
					double sum = 0.0;
					for(int ii=0; ii<i; ii++) {
						sum += max(0.0, R(ii,k));
					}
					for(int ii=i+1; ii<N; ii++) {
						sum += max(0.0, R(ii,k));
					}
					A(i,k) = (1-_lambda)*sum + _lambda*A(i,k);
				} else {
					double sum = 0.0;
					int maxik = max(i, k);
					int minik = min(i, k);
					for(int ii=0; ii<minik; ii++) {
						sum += max(0.0, R(ii,k));
					}
					for(int ii=minik+1; ii<maxik; ii++) {
						sum += max(0.0, R(ii,k));
					}
					for(int ii=maxik+1; ii<N; ii++) {
						sum += max(0.0, R(ii,k));
					}
					A(i,k) = (1-_lambda)*min(0.0, R(k,k)+sum) + _lambda*A(i,k);
				}
			}
		}
        if (m % _convit == 0 && m != 0) { // check for convergence
            cout << "checking for convergence.." << endl;
            
            int *idx = getIndexesForResponsibilitiesAndAvailabilities(R,A,S,N);

            bool equal = true;
            for (int i = 0; i < N; i++) {
                if (idx[i] != last_idx[i]) {
                    equal = false;
                }
            }
            if (equal) {
                cout << "terminate early! at iter="<< m << endl;
                break;
            } else {
                memcpy(last_idx, idx, N*sizeof(int));
            }
        }
	}
	return last_idx;
}

double Classifier::jaccardIndex(set<string>set1,set<string>set2) {
	set<string> unionSet;
	set_union(set1.begin(),set1.end(),set2.begin(),set2.end(),inserter(unionSet,unionSet.begin()));
	set<string> intersectSet;
	set_intersection(set1.begin(),set1.end(),set2.begin(),set2.end(),inserter(intersectSet,intersectSet.begin()));
	return (double)intersectSet.size() / (double)unionSet.size();
}

double Classifier::transformedJaccardIndex( double jaccardIndex ) {
	if (jaccardIndex == 0)
	{
		return -99999999;
	} else {
		return pow(-(-log2(jaccardIndex)+1),10)+1;
	}
}

