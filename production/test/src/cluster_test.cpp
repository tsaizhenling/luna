#include <limits.h>
#include "gtest/gtest.h"
#include "cluster.h"

using namespace std;

class cluster_test : public::testing::Test {
protected:
	virtual void SetUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(cluster_test,print) {
	Cluster testCluster;
	string mystrings[] = {"one","two","three"};
	set<string> testSet(mystrings,mystrings+3);
	testCluster.keyProduct = testSet;
	string testStrings1[] = {"one"};
	set<string> testSet1(testStrings1,testStrings1+1);
	string testStrings2[] = {"one","four"};
	set<string> testSet2(testStrings2,testStrings2+2);
	string testStrings3[] = {"one","three"};
	set<string> testSet3(testStrings3,testStrings3+2);
	string testStrings4[] = {"blah","three"};
	set<string> testSet4(testStrings4,testStrings4+2);
	testCluster.allOtherProducts.push_back(testSet1);
	testCluster.allOtherProducts.push_back(testSet2);
	testCluster.allOtherProducts.push_back(testSet3);
	testCluster.allOtherProducts.push_back(testSet4);
	stringstream ss;
	testCluster.print(ss);

	ASSERT_EQ("keywords: one three \n*one three two \none \nfour one \none three \nblah three \n", ss.str());
}

TEST_F(cluster_test,getKeywords) {
	Cluster testCluster;
	string mystrings[] = {"one","two","three"};
	set<string> testSet(mystrings,mystrings+3);
	testCluster.keyProduct = testSet;
	ASSERT_EQ(testSet,testCluster.getKeywords());

	string testStrings1[] = {"one"};
	set<string> testSet1(testStrings1,testStrings1+1);

	string testStrings2[] = {"one","four"};
	set<string> testSet2(testStrings2,testStrings2+2);
	testCluster.allOtherProducts.push_back(testSet1);
	testCluster.allOtherProducts.push_back(testSet2);
	ASSERT_EQ(testSet1,testCluster.getKeywords());

	string testStrings3[] = {"one","three"};
	set<string> testSet3(testStrings3,testStrings3+2);
	string testStrings4[] = {"blah","three"};
	set<string> testSet4(testStrings4,testStrings4+2);
	testCluster.allOtherProducts.push_back(testSet3);
	testCluster.allOtherProducts.push_back(testSet4);
	ASSERT_EQ(testSet3,testCluster.getKeywords());
}