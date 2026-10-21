CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -std=c11 -g
BUILD    = build

TEST_BIN = $(BUILD)/test_graph
LIB_SRC  = src/graph.c
TEST_SRC = tests/test_graph.c
HEADERS  = src/graph.h

all: $(TEST_BIN)

$(BUILD):
	mkdir -p $(BUILD)

$(TEST_BIN): $(LIB_SRC) $(TEST_SRC) $(HEADERS) | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $(TEST_BIN) $(LIB_SRC) $(TEST_SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
