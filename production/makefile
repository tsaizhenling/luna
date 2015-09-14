SOURCE_DIR = src
OBJ_DIR = obj
EXE_DIR = bin
EXE_NAME = ap2
LIB_DIR = lib

CPP_FILES = $(wildcard $(SOURCE_DIR)/*.cpp)
OBJS = $(patsubst $(SOURCE_DIR)%,$(OBJ_DIR)%,$(patsubst %.cpp,%.o,$(CPP_FILES)))

CC = g++
DEBUG = -g
CFLAGS = -Wno-c++11-extensions


all : pre-build $(OBJS)
	$(CC) $(OBJS) -o $(EXE_DIR)/$(EXE_NAME)

pre-build:
	mkdir -p $(EXE_DIR) $(OBJ_DIR) $(LIB_DIR)

$(OBJ_DIR)/%.o: $(SOURCE_DIR)/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_DIR)/libluna.a: $(OBJS)
	ar rcs $@ $^

clean:
	\rm $(OBJ_DIR)/*.o $(EXE_DIR)/$(EXE_NAME)