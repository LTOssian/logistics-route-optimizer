#include "heap.h"

#include <stdlib.h>

/**
 * @file heap.c
 * @brief Tas min binaire (implémentation naïve).
 *
 * Le tas est utilisé pour améliorer la complexité de Dijkstra à O((V + E) log V).
 * Il est implémenté par un tableau dynamique, avec une croissance par doublement.
 */

 /** 
 * @brief Échange deux entrées du tas.
 * @param a première entrée.
 * @param b deuxième entrée.
 */
static void swap_items(HeapItem *a, HeapItem *b) {
  HeapItem tmp = *a;
  *a = *b;
  *b = tmp;
}

/**
 * @brief Remonte une entrée dans le tas.
 * @param heap le tas.
 * @param index l'index de l'entrée à remonter.
 */
static void sift_up(MinHeap *heap, int index) {
  while (index > 0) {
    int parent = (index - 1) / 2;
    if (heap->items[parent].key <= heap->items[index].key)
      break;
    swap_items(&heap->items[parent], &heap->items[index]);
    index = parent;
  }
}

/**
 * @brief Descend une entrée dans le tas.
 * @param heap le tas.
 * @param index l'index de l'entrée à descendre.
 */
static void sift_down(MinHeap *heap, int index) {
  for (;;) {
    int left = 2 * index + 1;
    int right = left + 1;
    int smallest = index;

    if (left < heap->size && heap->items[left].key < heap->items[smallest].key)
      smallest = left;
    if (right < heap->size &&
        heap->items[right].key < heap->items[smallest].key)
      smallest = right;
    if (smallest == index)
      break;

    swap_items(&heap->items[smallest], &heap->items[index]);
    index = smallest;
  }
}

/**
 * @brief Alloue un tas vide.
 * @param capacity capacité initiale (capacity >= 1).
 * @return le tas, ou NULL si `capacity` est invalide ou si l'allocation échoue.
 * @note À libérer avec heap_free().
 */

MinHeap *heap_new(int capacity) {
  if (capacity <= 0)
    return NULL;

  MinHeap *heap = malloc(sizeof(MinHeap));
  if (heap == NULL)
    return NULL;

  heap->items = malloc(sizeof(HeapItem) * (size_t)capacity);
  if (heap->items == NULL) {
    free(heap);
    return NULL;
  }

  heap->size = 0;
  heap->capacity = capacity;
  return heap;
}

/**
 * @brief Insère l'entrée (key, node).
 * @param heap le tas.
 * @param key la clé de l'entrée.
 * @param node le nœud de l'entrée.
 * @return 1 en cas de succès, 0 si `heap` est NULL ou si la croissance échoue
 *         (le tas reste alors valide et inchangé).
 */
int heap_push(MinHeap *heap, double key, int node) {
  if (heap == NULL)
    return 0;

  if (heap->size == heap->capacity) {
    int new_capacity = heap->capacity * 2;
    HeapItem *grown =
        realloc(heap->items, sizeof(HeapItem) * (size_t)new_capacity);
    if (grown == NULL)
      return 0; /* l'ancien tableau reste valide */
    heap->items = grown;
    heap->capacity = new_capacity;
  }

  heap->items[heap->size].key = key;
  heap->items[heap->size].node = node;
  heap->size++;
  sift_up(heap, heap->size - 1);
  return 1;
}

/**
 * @brief Extrait l'entrée de plus petite clé.
 * @param heap le tas.
 * @param out reçoit l'entrée extraite.
 * @return 1 en cas de succès, 0 si le tas est vide ou si un paramètre est NULL.
 */
int heap_pop(MinHeap *heap, HeapItem *out) {
  if (heap == NULL || out == NULL || heap->size == 0)
    return 0;

  *out = heap->items[0];
  heap->size--;
  if (heap->size > 0) {
    heap->items[0] = heap->items[heap->size];
    sift_down(heap, 0);
  }
  return 1;
}

/**
 * @brief Teste si le tas est vide.
 * @param heap le tas.
 * @return 1 si vide ou NULL, 0 sinon.
 */
int heap_is_empty(const MinHeap *heap) {
  return heap == NULL || heap->size == 0;
}

/**
 * @brief Libère le tas.
 * @param heap tas à libérer (NULL accepté).
 */
void heap_free(MinHeap *heap) {
  if (heap == NULL)
    return;
  free(heap->items);
  free(heap);
}