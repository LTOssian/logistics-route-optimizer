CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -std=c11 -g
LDLIBS   = -lm
BUILD    = build

LIB_SRC  = src/graph.c src/dijkstra.c src/heap.c
LIB_HDR  = src/graph.h src/dijkstra.h src/heap.h

TEST_GRAPH    = $(BUILD)/test_graph
TEST_DIJKSTRA = $(BUILD)/test_dijkstra
TEST_HEAP     = $(BUILD)/test_heap
TEST_RNG      = $(BUILD)/test_rng
TEST_BINS     = $(TEST_GRAPH) $(TEST_DIJKSTRA) $(TEST_HEAP) $(TEST_RNG)

all: $(TEST_BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(TEST_GRAPH): src/graph.c tests/test_graph.c src/graph.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/graph.c tests/test_graph.c

$(TEST_DIJKSTRA): $(LIB_SRC) tests/test_dijkstra.c $(LIB_HDR) | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ $(LIB_SRC) tests/test_dijkstra.c $(LDLIBS)

$(TEST_HEAP): src/heap.c src/rng.c tests/test_heap.c src/heap.h src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/heap.c src/rng.c tests/test_heap.c

$(TEST_RNG): src/rng.c tests/test_rng.c src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/rng.c tests/test_rng.c

test: $(TEST_BINS)
	./$(TEST_GRAPH)
	./$(TEST_DIJKSTRA)
	./$(TEST_HEAP)
	./$(TEST_RNG)

clean:
	rm -rf $(BUILD)

.PHONY: all test clean