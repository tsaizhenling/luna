TEST_DIR = test

test.CXX = g++
test.CXXFLAGS = -g -lgtest -lgtest_main -lpthread -Wno-c++11-extensions
test.INCS = -I./ -I$(SOURCE_DIR) -I/usr/include

test.SOURCE_DIR = $(TEST_DIR)/src
test.OBJ_DIR = $(TEST_DIR)/obj
test.CPP_FILES = $(wildcard $(test.SOURCE_DIR)/*.cpp)
test.OBJS = $(patsubst $(test.SOURCE_DIR)%,$(test.OBJ_DIR)%,$(patsubst %.cpp,%.o,$(test.CPP_FILES)))

test.all: test.pre-build $(test.OBJS)
	$(test.CXX) $(test.CXXFLAGS) $(test.INCS) -L $(LIB_DIR) -l $(LIB_NAME) -o testAll $(test.OBJS)
	./testAll --gtest_output="xml:./testAll.xml"

test.pre-build:
	mkdir -p $(test.OBJ_DIR)

$(test.OBJ_DIR)/%.o: $(test.SOURCE_DIR)/%.cpp
	$(test.CXX) $(test.CXXFLAGS) -c $< -o $@ $(test.INCS)

test.clean:
	-rm -rf testAll $(test.OBJ_DIR)/*.o testAll.xml