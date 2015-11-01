#include <limits.h>
#include "gtest/gtest.h"
#include "classifier.h"
#include "utils.h"

using namespace std;

class classifier_test : public::testing::Test {
protected:
	virtual void SetUp() {

	}
	virtual void TearDown() {

	}
};

class ClassifierTest : public Classifier {
	public:
		ClassifierTest(vector<set<string> > allProducts) :  Classifier(allProducts) {
		}
    	static double jaccardIndex(set<string>set1,set<string>set2) {
     		return Classifier::jaccardIndex(set1,set2);
    	}

    	static double transformedJaccardIndex(double jaccardIndex) {
     		return Classifier::transformedJaccardIndex(jaccardIndex);
    	}

    	int* getIndexesForResponsibilitiesAndAvailabilities (Matrix<double> R, Matrix<double> A, Matrix<double> S, int N) {
    		return Classifier::getIndexesForResponsibilitiesAndAvailabilities(R,A,S,N);
    	}

    	int* getIndexes(Matrix<double> S,double median, int N) {
    		return Classifier::getIndexes(S,median,N,cout);
    	}
};

TEST_F(classifier_test,jaccardIndex) {
	string testStrings1[] = {"one","four"};
	set<string> testSet1(testStrings1,testStrings1+1);
	string testStrings2[] = {"one","three"};
	set<string> testSet2(testStrings2,testStrings2+2);
	ASSERT_EQ(0.5,ClassifierTest::jaccardIndex(testSet1,testSet2));

	string testStrings3[] = {"blah","meow"};
	set<string> testSet3(testStrings3,testStrings3+2);
	ASSERT_EQ(0,ClassifierTest::jaccardIndex(testSet1,testSet3));
}

TEST_F(classifier_test,transformedJaccardIndex) {
	ASSERT_EQ(-99999999,ClassifierTest::transformedJaccardIndex(0));
	ASSERT_NEAR(2273921.9939110577,ClassifierTest::transformedJaccardIndex(0.1),0.0001);
	ASSERT_NEAR(5.7302582817157637,ClassifierTest::transformedJaccardIndex(0.89),0.0001);
}

TEST_F(classifier_test,getIndexesForResponsibilitiesAndAvailabilities) {
	Matrix<double> S(4,4);
	Matrix<double> R(4,4);
	Matrix<double> A(4,4);
	S(0,0) = 20;
	S(0,1) = 4555.99;
	S(0,2) = 1025;
	S(0,3) = -1e+08;
	S(1,0) = 4555.99;
	S(1,1) = 20;
	S(1,2) = 4555.99;
	S(1,3) = -1e+08;
	S(2,0) = 1025;
	S(2,1) = 4555.99;
	S(2,2) = 20;
	S(2,3) = -1e+08;
	S(3,0) = -1e+08;
	S(3,1) = -1e+08;
	S(3,2) = -1e+08;
	S(3,3) = 20;

	R(0,0) = -4535.99;
	R(0,1) = 4535.99;
	R(0,2) = -3530.99;
	R(0,3) = -1.00005e+08;
	R(1,0) = -4535.99;
	R(1,1) = -6.43945e-08;
	R(1,2) = -4535.99;
	R(1,3) = -1.00009e+08;
	R(2,0) = -3530.99;
	R(2,1) = 4535.99;
	R(2,2) = -4535.99;
	R(2,3) = -1.00005e+08;
	R(3,0) = -1e+08;
	R(3,1) = -1e+08;
	R(3,2) = -1e+08;
	R(3,3) = 1e+08;

	A(0,0) = 7.09121e-11;
	A(0,1) = -4.39112e-12;
	A(0,2) = -4535.99;
	A(0,3) = 0;
	A(1,0) = -4535.99;
	A(1,1) = 9071.98;
	A(1,2) = -4535.99;
	A(1,3) = 0;
	A(2,0) = -4535.99;
	A(2,1) = -4.39112e-12;
	A(2,2) = 7.09121e-11;
	A(2,3) = 0;
	A(3,0) = -4535.99;
	A(3,1) = 0;
	A(3,2) = -4535.99;
	A(3,3) = 0;

	vector<set<string> > testProducts;
	ClassifierTest testClassifier(testProducts);
	int *last_idx = testClassifier.getIndexesForResponsibilitiesAndAvailabilities(R,A,S,4);
	ASSERT_EQ(1,last_idx[0]);
	ASSERT_EQ(1,last_idx[1]);
	ASSERT_EQ(1,last_idx[2]);
	ASSERT_EQ(3,last_idx[3]);
}

TEST_F(classifier_test,getIndexes) {
	Matrix<double> S(4,4);
	S(0,0) = 2;
	S(0,1) = 4555.99;
	S(0,2) = 1025;
	S(0,3) = -1e+08;
	S(1,0) = 4555.99;
	S(1,1) = 2;
	S(1,2) = 4555.99;
	S(1,3) = -1e+08;
	S(2,0) = 1025;
	S(2,1) = 4555.99;
	S(2,2) = 2;
	S(2,3) = -1e+08;
	S(3,0) = -1e+08;
	S(3,1) = -1e+08;
	S(3,2) = -1e+08;
	S(3,3) = 2;
	vector<set<string> > testProducts;
	ClassifierTest testClassifier(testProducts);
	int *last_idx = testClassifier.getIndexes(S,20,4);
	ASSERT_EQ(1,last_idx[0]);
	ASSERT_EQ(1,last_idx[1]);
	ASSERT_EQ(1,last_idx[2]);
	ASSERT_EQ(3,last_idx[3]);
}

TEST_F(classifier_test,deriveKeywords) {
	string testStrings1[] = {"blue","azzyati","jubah"};
	set<string> testSet1(testStrings1,testStrings1+3);
	string testStrings2[] = {"light","red","azzyati","jubah"};
	set<string> testSet2(testStrings2,testStrings2+4);
	string testStrings3[] = {"grey","azzyati","jubah"};
	set<string> testSet3(testStrings3,testStrings3+3);
	string testStrings4[] = {"apple","green","pallazo","wrinkle","free","pant"};
	set<string> testSet4(testStrings4,testStrings4+6);

	vector<set<string> > testProducts;
	testProducts.push_back(testSet1);
	testProducts.push_back(testSet2);
	testProducts.push_back(testSet3);
	testProducts.push_back(testSet4);
	Classifier testClassifier(testProducts);

	string resultsStrings1[] = {"apple","free","green","pallazo","pant","wrinkle"};
	set<string> resultsSet1(resultsStrings1,resultsStrings1+6);
	string resultsStrings2[] = {"azzyati","jubah"};
	set<string> resultsSet2(resultsStrings2,resultsStrings2+2);
	set<set<string> > results;
	results.insert(resultsSet1);
	results.insert(resultsSet2);
	ASSERT_EQ(results,testClassifier.deriveKeywords(cout));
}
