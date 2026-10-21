CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -std=c11 -g
LDLIBS   = -lm
BUILD    = build

LIB_SRC  = src/graph.c src/djikstra.c
LIB_HDR  = src/graph.h src/djikstra.h

TEST_GRAPH    = $(BUILD)/test_graph
TEST_DJIKSTRA = $(BUILD)/test_djisktra
TEST_BINS     = $(TEST_GRAPH) $(TEST_DJIKSTRA)

all: $(TEST_BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(TEST_GRAPH): src/graph.c tests/test_graph.c src/graph.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/graph.c tests/test_graph.c

$(TEST_DJIKSTRA): $(LIB_SRC) tests/test_djisktra.c $(LIB_HDR) | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ $(LIB_SRC) tests/test_djisktra.c $(LDLIBS)

test: $(TEST_BINS)
	./$(TEST_GRAPH)
	./$(TEST_DJIKSTRA)

clean:
	rm -rf $(BUILD)

.PHONY: all test clean
