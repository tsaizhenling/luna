TEST_DIR = test

test.CXX = g++
test.CXXFLAGS = -g -lgtest -lgtest_main -lpthread -Wno-c++11-extensions
test.INCS = -I./ -I$(SOURCE_DIR) -I/usr/include

test.SOURCE_DIR = $(TEST_DIR)/src
test.OBJ_DIR = $(TEST_DIR)/obj
test.CPP_FILES = $(wildcard $(test.SOURCE_DIR)/*.cpp)
test.OBJS = $(wildcard $(OBJ_DIR)/*.o) $(patsubst $(test.SOURCE_DIR)%,$(test.OBJ_DIR)%,$(patsubst %.cpp,%.o,$(test.CPP_FILES)))

test.all: $(test.OBJS)
	$(test.CXX) $(test.CXXFLAGS) $(test.INCS) -o testAll $(test.OBJS)
	./testAll --gtest_output="xml:./testAll.xml"

$(test.OBJ_DIR)/%.o: $(test.SOURCE_DIR)/%.cpp
	$(test.CXX) $(test.CXXFLAGS) -c $< -o $@ $(test.INCS)

test.clean:
	rm -r testAll $(test.OBJ_DIR)/*.o testAll.xml