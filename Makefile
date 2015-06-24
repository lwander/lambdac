IDIR=inc
CXX=gcc
CXXFLAGS=-I$(IDIR)/ -c -Wall -Wpedantic -Werror -std=c11 -m32 -g

OBJ_DIR=obj
SRC_DIR=src
SUB_DIRS=libs test
ALL_DIRS=$(SUB_DIRS:%=$(OBJ_DIR)/%)

EXECUTABLE=lcc

TEST_EXECUTABLE=test_lcc

# Files needed only by LLC executable
LCC_SRCS=main.c ast.c

# Files required by unit tests & LCC executable
# SHRD_SRCS=libs/hashtable.c

# Files required only by unit tests
# TEST_SRCS=test/lcc_test.c

SHRD_OBJS=$(SHRD_SRCS:%.c=$(OBJ_DIR)/%.o)

LCC_OBJS=$(LCC_SRCS:%.c=$(OBJ_DIR)/%.o)

TEST_OBJS=$(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean dirs test

all: dirs $(EXECUTABLE)

test: dirs $(TEST_EXECUTABLE)
	
$(TEST_EXECUTABLE): $(SHRD_OBJS) $(TEST_OBJS)
	$(CXX) $^ -o $(TEST_EXECUTABLE)

$(EXECUTABLE): $(SHRD_OBJS) $(LCC_OBJS)
	$(CXX) $^ -o $(EXECUTABLE) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CXX) $(CXXFLAGS) $< -o $@

dirs: 
	-mkdir -p $(OBJ_DIR) $(ALL_DIRS)

clean:
	-rm -rf $(OBJ_DIR)
	-rm $(EXECUTABLE)
	-rm $(TEST_EXECUTABLE)

#-include $(OBJS:%.o=%.d)
