SOURCE_DIR = src
OBJ_DIR = obj
EXE_DIR = bin
LIB_DIR = lib
LIB_NAME = luna
BUNDLE_NAME = luna.bundle
SWIG_CXX = luna_wrap.cxx
SWIG_O = luna_wrap.o
SWIG_I = luna.i
HEADER_DIR = $(LIB_DIR)/headers
TEST_DIR = test
RUBY_DIR = $$HOME/.rbenv/versions/2.2.0/include/ruby-2.2.0

CPP_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJS = $(patsubst $(SOURCE_DIR)%,$(OBJ_DIR)%,$(patsubst %.cpp,%.o,$(CPP_FILES)))

CC = g++
DEBUG = -g
CFLAGS = -Wno-c++11-extensions

all : pre-build $(OBJS) $(LIB_DIR)/lib$(LIB_NAME).a $(EXE_DIR)/(LIB_NAME).bundle

pre-build:
	mkdir -p $(EXE_DIR) $(OBJ_DIR) $(LIB_DIR)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR)/lib$(LIB_NAME).a: $(OBJS)
	ar rcs $@ $^
	mkdir -p $(HEADER_DIR)
	cp $(SOURCE_DIR)/* $(HEADER_DIR)
	rm $(HEADER_DIR)/*.cpp

$(EXE_DIR)/(LIB_NAME).bundle: 
	swig -c++ -ruby -o $(SOURCE_DIR)/$(SWIG_CXX) $(SOURCE_DIR)/$(SWIG_I)
	g++ -c $(SOURCE_DIR)/$(SWIG_CXX) -I$(RUBY_DIR) -o $(OBJ_DIR)/$(SWIG_O)
	g++ -bundle -flat_namespace -undefined suppress $(OBJS) $(OBJ_DIR)/$(SWIG_O) -o $(EXE_DIR)/$(BUNDLE_NAME)

clean-test: test.clean

clean:
	-rm -rf $(OBJ_DIR)/*.o $(EXE_DIR)/$(EXE_NAME) $(LIB_DIR)/* $(SOURCE_DIR)/$(SWIG_CXX)

test: pre-build $(OBJS) $(LIB_DIR)/lib$(LIB_NAME).a test.all

include $(TEST_DIR)/makefile