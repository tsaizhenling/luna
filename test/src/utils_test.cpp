#include <limits.h>
#include "gtest/gtest.h"
#include "utils.h"
#include <sstream>

class utils_test : public::testing::Test {
protected:
	virtual void SetUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(utils_test,printSetOfStrings) {
	stringstream ss;
	string mystrings[] = {"one","two","three"};
	set<string> testSet(mystrings,mystrings+3);
	printSetOfStrings(ss,testSet);
	set<string> words;
	string s;
	while (getline(ss, s, ' ')) {
		if (s != "\n")
		{
			words.insert(s);
		}	
	}
	ASSERT_EQ(testSet,words);
}
