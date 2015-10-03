SOURCE_DIR = src
OBJ_DIR = obj
EXE_DIR = bin
EXE_NAME = ap2
LIB_DIR = lib
LIB_NAME = luna
HEADER_DIR = $(LIB_DIR)/headers
MAIN_FILE = example/main.cpp

CPP_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJS = $(patsubst $(SOURCE_DIR)%,$(OBJ_DIR)%,$(patsubst %.cpp,%.o,$(CPP_FILES)))

CC = g++
DEBUG = -g
CFLAGS = -Wno-c++11-extensions


all : pre-build $(OBJS) $(LIB_DIR)/lib$(LIB_NAME).a
	$(CC) $(CFLAGS) $(MAIN_FILE) -I $(HEADER_DIR) -L $(LIB_DIR) -l $(LIB_NAME) -o $(EXE_DIR)/$(EXE_NAME)

pre-build:
	mkdir -p $(EXE_DIR) $(OBJ_DIR) $(LIB_DIR) $(TEMP_DIR)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR)/lib$(LIB_NAME).a: $(OBJS)
	ar rcs $@ $^
	mkdir -p $(HEADER_DIR)
	cp $(SOURCE_DIR)/* $(HEADER_DIR)
	rm $(HEADER_DIR)/*.cpp

clean:
	\rm -r $(OBJ_DIR)/*.o $(EXE_DIR)/$(EXE_NAME) $(LIB_DIR)/*