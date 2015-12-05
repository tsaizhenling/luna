#include <limits.h>
#include "gtest/gtest.h"
#include "matrix.h"

class matrix_test : public::testing::Test {
protected:
	virtual void SetUp() {

	}
	virtual void TearDown() {

	}
};

TEST_F(matrix_test,accessor) {
	Matrix<double> S(3,3);
	S(0,0) = 4.5;
	S(1,2) = 1.3;
	ASSERT_EQ(4.5,S(0,0));
	ASSERT_EQ(1.3,S(1,2));
	ASSERT_EQ(0,S(0,1));
	ASSERT_EQ(0,S(0,2));
	ASSERT_EQ(0,S(1,0));
	ASSERT_EQ(0,S(1,1));
	ASSERT_EQ(0,S(2,0));
	ASSERT_EQ(0,S(2,1));
	ASSERT_EQ(0,S(2,2));
}