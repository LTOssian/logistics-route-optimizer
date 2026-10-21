#include "djikstra.h"
#include "graph.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int error_count = 0;
static int check_count = 0;
static int total_errors = 0;

static void expect_int(const char *label, int value, int expected) {
  check_count++;
  if (value != expected) {
    error_count++;
    printf("  [FAIL] %s: got %d, expected %d\n", label, value, expected);
  } else {
    printf("  [ok]   %s: %d\n", label, value);
  }
}

static void expect_double(const char *label, double value, double expected) {
  check_count++;
  double difference = value - expected;
  if (difference < 0)
    difference = -difference;
  if (difference > 1e-9) {
    error_count++;
    printf("  [FAIL] %s: got %f, expected %f\n", label, value, expected);
  } else {
    printf("  [ok]   %s: %f\n", label, value);
  }
}

static void expect_inf(const char *label, double value) {
  check_count++;
  if (!isinf(value)) {
    error_count++;
    printf("  [FAIL] %s: got %f, expected inf\n", label, value);
  } else {
    printf("  [ok]   %s: inf\n", label);
  }
}

static void expect_path(const char *label, const int *path, int path_length,
                        const int *expected_path, int expected_length) {
  check_count++;
  int is_equal = (path != NULL) && (path_length == expected_length);

  if (is_equal) {
    for (int index = 0; index < path_length; index++) {
      if (path[index] != expected_path[index])
        is_equal = 0;
    }
  }

  if (!is_equal) {
    error_count++;
    printf("  [FAIL] %s: got len=%d [", label, path_length);
    for (int index = 0; index < path_length; index++) {
      printf("%d%s", path != NULL ? path[index] : -999,
             (index + 1 < path_length) ? "-" : "");
    }
    printf("], expected len=%d [", expected_length);
    for (int index = 0; index < expected_length; index++) {
      printf("%d%s", expected_path[index],
             (index + 1 < expected_length) ? "-" : "");
    }
    printf("]\n");
  } else {
    printf("  [ok]   %s\n", label);
  }
}

static void print_suite_result(const char *name) {
  const char *state = (error_count > 0) ? "FAIL" : "PASS";
  printf("=> %s: TESTS %s (%d checks, %d errors)\n\n", name, state, check_count,
         error_count);
  total_errors += error_count;
  error_count = 0;
  check_count = 0;
}

/* --- suites --- */

static void test_dijkstra_basic(void) {
  printf("test_dijkstra_basic\n");

  Graph *graph = graph_new(4);
  graph_add_edge(graph, 0, 1, 1.0);
  graph_add_edge(graph, 1, 2, 1.0);
  graph_add_edge(graph, 0, 2, 3.0); // détour plus court : 0->1->2 = 2 < 3
  graph_add_edge(graph, 2, 3, 1.0);

  DijkstraResult *result = dijkstra(graph, 0);
  expect_int("resultat non NULL", result != NULL, 1);

  if (result != NULL) {
    expect_double("distances[0]", result->distances[0], 0.0);
    expect_double("distances[1]", result->distances[1], 1.0);
    expect_double("distances[2] (detour plus court)", result->distances[2], 2.0);
    expect_double("distances[3]", result->distances[3], 3.0);
    expect_int("previous[2] == 1", result->previous[2], 1);

    int path_length = 0;
    int *path = dijkstra_path(result, 3, &path_length);
    int expected_path[] = {0, 1, 2, 3};
    expect_path("chemin 0 -> 3", path, path_length, expected_path, 4);
    free(path);

    dijkstra_result_free(result);
  }

  graph_free(graph);
  print_suite_result("test_dijkstra_basic");
}

static void test_dijkstra_unreachable(void) {
  printf("test_dijkstra_unreachable\n");

  Graph *graph = graph_new(3);
  graph_add_edge(graph, 0, 1, 2.0);
  // nœud 2 isolé

  DijkstraResult *result = dijkstra(graph, 0);
  expect_int("resultat non NULL", result != NULL, 1);

  if (result != NULL) {
    expect_double("distances[0]", result->distances[0], 0.0);
    expect_double("distances[1]", result->distances[1], 2.0);
    expect_inf("distances[2] inatteignable", result->distances[2]);

    int path_length = -1;
    int *path = dijkstra_path(result, 2, &path_length);
    expect_int("chemin vers 2 == NULL", path == NULL, 1);
    expect_int("path_length == 0", path_length, 0);
    free(path);

    dijkstra_result_free(result);
  }

  graph_free(graph);
  print_suite_result("test_dijkstra_unreachable");
}

static void test_dijkstra_ties(void) {
  printf("test_dijkstra_ties\n");

  Graph *graph = graph_new(4);
  graph_add_edge(graph, 0, 1, 1.0);
  graph_add_edge(graph, 0, 2, 1.0);
  graph_add_edge(graph, 1, 3, 1.0);
  graph_add_edge(graph, 2, 3, 1.0);

  DijkstraResult *result = dijkstra(graph, 0);
  expect_int("resultat non NULL", result != NULL, 1);

  if (result != NULL) {
    expect_double("distances[3] (deux chemins egaux)", result->distances[3],
                  2.0);

    int path_length = 0;
    int *path = dijkstra_path(result, 3, &path_length);
    expect_int("longueur du chemin", path_length, 3);
    expect_int("commence par 0", path != NULL ? path[0] : -1, 0);
    expect_int("finit par 3", path != NULL ? path[path_length - 1] : -1, 3);
    free(path);

    dijkstra_result_free(result);
  }

  graph_free(graph);
  print_suite_result("test_dijkstra_ties");
}

static void test_dijkstra_toy1(void) {
  printf("test_dijkstra_toy1\n");

  Graph *graph = graph_load_csv("data/toy/toy1.csv");
  expect_int("toy1 charge", graph != NULL, 1);
  if (graph == NULL) {
    print_suite_result("test_dijkstra_toy1");
    return;
  }

  DijkstraResult *result = dijkstra(graph, 0);
  expect_int("dijkstra non NULL", result != NULL, 1);

  if (result != NULL) {
    double expected_distances[] = {0.0, 8.0, 9.0, 5.0, 7.0};
    for (int index = 0; index < 5; index++) {
      char label[32];
      snprintf(label, sizeof label, "distances[%d]", index);
      expect_double(label, result->distances[index], expected_distances[index]);
    }

    int path_length = 0;

    int *path_to_1 = dijkstra_path(result, 1, &path_length);
    int expected_1[] = {0, 3, 1};
    expect_path("chemin 0 -> 1", path_to_1, path_length, expected_1, 3);
    free(path_to_1);

    int *path_to_2 = dijkstra_path(result, 2, &path_length);
    int expected_2[] = {0, 3, 1, 2};
    expect_path("chemin 0 -> 2", path_to_2, path_length, expected_2, 4);
    free(path_to_2);

    int *path_to_3 = dijkstra_path(result, 3, &path_length);
    int expected_3[] = {0, 3};
    expect_path("chemin 0 -> 3", path_to_3, path_length, expected_3, 2);
    free(path_to_3);

    int *path_to_4 = dijkstra_path(result, 4, &path_length);
    int expected_4[] = {0, 3, 4};
    expect_path("chemin 0 -> 4", path_to_4, path_length, expected_4, 3);
    free(path_to_4);

    dijkstra_result_free(result);
  }

  graph_free(graph);
  print_suite_result("test_dijkstra_toy1");
}

static void test_dijkstra_invalid(void) {
  printf("test_dijkstra_invalid\n");

  expect_int("graphe NULL -> NULL", dijkstra(NULL, 0) == NULL, 1);

  Graph *graph = graph_new(2);
  if (graph != NULL) {
    expect_int("source hors bornes -> NULL", dijkstra(graph, 2) == NULL, 1);
    expect_int("source negative -> NULL", dijkstra(graph, -1) == NULL, 1);
    graph_free(graph);
  }

  expect_int("dijkstra_path(NULL) -> NULL",
             dijkstra_path(NULL, 0, NULL) == NULL, 1);
  dijkstra_result_free(NULL);

  print_suite_result("test_dijkstra_invalid");
}

int main(void) {
  test_dijkstra_basic();
  test_dijkstra_unreachable();
  test_dijkstra_ties();
  test_dijkstra_toy1();
  test_dijkstra_invalid();

  printf("Toutes les suites sont terminees (%d erreurs).\n", total_errors);
  return (total_errors == 0) ? 0 : 1;
}
