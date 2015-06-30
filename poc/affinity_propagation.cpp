#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

int main(int argc, char *argv[])
{
	vector<double> tmpS;
	//read data 
	ifstream myfile(argv[1]);
	//N is the number of two-dimension data points
	//S is the similarity matrix
	//R is the responsibility matrix
	//A is the availabiltiy matrix
	//iter is the maximum number of iterations
	//lambda is the damping factor
	int N;
	int iter = 1500;
    int convit = 150; // number of iterations to check for change of exemplars. if no change for convit number of iterations, terminate
	double lambda = 0.9;
	int size;
	myfile >> size >> N;
	double S[N][N];
	memset(S, 0, N*N*sizeof(double));
	double R[N][N];
	memset(R, 0, N*N*sizeof(double));
	double A[N][N];
	memset(A, 0, N*N*sizeof(double));
	for(int i=0; i<size; i++) {
		int x,y; double num;
		myfile >> x >> y >> num;
		S[x][y] = num;
		S[y][x] = S[x][y];
		tmpS.push_back(S[x][y]); 
	}
	myfile.close();
	//compute preferences for all data points: median 
	sort(tmpS.begin(), tmpS.end());
	double median = 0;
	
	if(size%2==0) 
		median = (tmpS[size/2]+tmpS[size/2-1])/2;
	else 
		median = tmpS[size/2];
	for(int i=0; i<N; i++) S[i][i] = median;


    int last_idx[N];
    memset(last_idx, 0, N*sizeof(int));
    
	for(int m=0; m<iter; m++) {
	//update responsibility
		for(int i=0; i<N; i++) {
			for(int k=0; k<N; k++) {
				double max = -1e100;
				for(int kk=0; kk<k; kk++) {
					if(S[i][kk]+A[i][kk]>max) 
						max = S[i][kk]+A[i][kk];
				}
				for(int kk=k+1; kk<N; kk++) {
					if(S[i][kk]+A[i][kk]>max) 
						max = S[i][kk]+A[i][kk];
				}
				R[i][k] = (1-lambda)*(S[i][k] - max) + lambda*R[i][k];
			}
		}
	//update availability
		for(int i=0; i<N; i++) {
			for(int k=0; k<N; k++) {
				if(i==k) {
					double sum = 0.0;
					for(int ii=0; ii<i; ii++) {
						sum += max(0.0, R[ii][k]);
					}
					for(int ii=i+1; ii<N; ii++) {
						sum += max(0.0, R[ii][k]);
					}
					A[i][k] = (1-lambda)*sum + lambda*A[i][k];
				} else {
					double sum = 0.0;
					int maxik = max(i, k);
					int minik = min(i, k);
					for(int ii=0; ii<minik; ii++) {
						sum += max(0.0, R[ii][k]);
					}
					for(int ii=minik+1; ii<maxik; ii++) {
						sum += max(0.0, R[ii][k]);
					}
					for(int ii=maxik+1; ii<N; ii++) {
						sum += max(0.0, R[ii][k]);
					}
					A[i][k] = (1-lambda)*min(0.0, R[k][k]+sum) + lambda*A[i][k];
				}
			}
		}
        if (m % convit == 0 && m != 0) { // check for convergence
            cout << "checking for convergence.." << endl;
            //find the exemplar
            double E[N][N];
            memset(E, 0, N*N*sizeof(double));
            vector<int> center;
            for(int i=0; i<N; i++) {
                E[i][i] = R[i][i] + A[i][i];
                if(E[i][i]>0) {
                    bool isDuplicate = false;
                    for( std::vector<int>::const_iterator j = center.begin(); j != center.end(); ++j) {
                        int index2 = *j;
                        if (S[i][index2] == 0)
                        {
                            isDuplicate = true;
                        }
                    }
                    if (!isDuplicate)
                    {
                        center.push_back(i);
                    }
                }
            }
            
            //data point assignment, idx[i] is the exemplar for data point i
            int idx[N];
            memset(idx, 0, N*sizeof(int));
            for(int i=0; i<N; i++) {
                int idxForI = 0;
                double maxSim = -1e100;
                for(int j=0; j<center.size(); j++) {
                    int c = center[j];
                    if (i == c) {
                        idxForI = c;
                        break;
                    }
                    if (S[i][c]>maxSim) {
                        maxSim = S[i][c];
                        idxForI = c;
                    }
                }
                idx[i] = idxForI;
            }
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
	
	//output the assignment
	ofstream outfile(argv[2]);
	for(int i=0; i<N; i++) {
		outfile << last_idx[i] << endl;
	}
	outfile.close();
}


