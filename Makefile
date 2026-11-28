CC       = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -Isrc
LDLIBS   = -lm
BUILD    = build

LIB_SRC  = src/graph.c src/dijkstra.c src/heap.c src/astar.c
LIB_HDR  = src/graph.h src/dijkstra.h src/heap.h src/astar.h

LRO           = $(BUILD)/lro
TEST_GRAPH    = $(BUILD)/test_graph
TEST_DIJKSTRA = $(BUILD)/test_dijkstra
TEST_HEAP     = $(BUILD)/test_heap
TEST_RNG      = $(BUILD)/test_rng
TEST_ORACLE   = $(BUILD)/test_oracle
TEST_EXPECTED = $(BUILD)/test_expected
TEST_DISTANCE_MATRIX = $(BUILD)/test_distance_matrix
TEST_POINTS   = $(BUILD)/test_points
TEST_ASTAR    = $(BUILD)/test_astar
TEST_BINS     = $(TEST_GRAPH) $(TEST_DIJKSTRA) $(TEST_HEAP) $(TEST_RNG) $(TEST_ORACLE) $(TEST_EXPECTED) $(TEST_DISTANCE_MATRIX) $(TEST_POINTS) $(TEST_ASTAR)

all: $(LRO) $(TEST_BINS)

$(BUILD):
	mkdir -p $(BUILD)

$(LRO): src/main.c $(LIB_SRC) src/rng.c $(LIB_HDR) src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/main.c $(LIB_SRC) src/rng.c $(LDLIBS)

$(TEST_GRAPH): src/graph.c tests/test_graph.c src/graph.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/graph.c tests/test_graph.c

$(TEST_DIJKSTRA): $(LIB_SRC) tests/test_dijkstra.c $(LIB_HDR) | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ $(LIB_SRC) tests/test_dijkstra.c $(LDLIBS)

$(TEST_HEAP): src/heap.c src/rng.c tests/test_heap.c src/heap.h src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/heap.c src/rng.c tests/test_heap.c

$(TEST_RNG): src/rng.c tests/test_rng.c src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/rng.c tests/test_rng.c

$(TEST_ORACLE): $(LIB_SRC) src/rng.c tests/test_oracle.c $(LIB_HDR) src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ $(LIB_SRC) src/rng.c tests/test_oracle.c $(LDLIBS)

$(TEST_EXPECTED): $(LIB_SRC) tests/test_expected.c $(LIB_HDR) | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ $(LIB_SRC) tests/test_expected.c $(LDLIBS)

$(TEST_DISTANCE_MATRIX): $(LIB_SRC) src/distance_matrix.c tests/test_distance_matrix.c $(LIB_HDR) src/distance_matrix.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ $(LIB_SRC) src/distance_matrix.c tests/test_distance_matrix.c $(LDLIBS)

$(TEST_POINTS): src/graph.c src/points.c src/rng.c tests/test_points.c src/graph.h src/points.h src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ src/graph.c src/points.c src/rng.c tests/test_points.c $(LDLIBS)
	
$(TEST_ASTAR): $(LIB_SRC) src/rng.c tests/test_astar.c $(LIB_HDR) src/rng.h | $(BUILD)
	$(CC) $(CFLAGS) -Isrc -o $@ $(LIB_SRC) src/rng.c tests/test_astar.c $(LDLIBS)

test: $(LRO) $(TEST_BINS)
	./$(TEST_GRAPH)
	./$(TEST_DIJKSTRA)
	./$(TEST_HEAP)
	./$(TEST_RNG)
	./$(TEST_ORACLE)
	./$(TEST_EXPECTED)
	./$(TEST_DISTANCE_MATRIX)
	./$(TEST_POINTS)
	./$(TEST_ASTAR)
	./$(LRO) path data/toy/toy1.csv 0 4 | grep -q "distance = 7.00"
	./$(LRO) path data/toy/toy1.csv 0 4 --astar data/toy/toy1_nodes.csv | grep -q "distance = 7.00"
	@echo "=> test CLI : OK"

clean:
	rm -rf $(BUILD)

.PHONY: all test clean