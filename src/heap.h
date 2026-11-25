#ifndef HEAP_H
#define HEAP_H

/**
 * @file heap.h
 * @brief Tas min binaire de couples (clé, nœud), pour Dijkstra et A*.
 *
 * Le tas grandit automatiquement : avec la lazy deletion, il peut contenir
 * plus de `node_count` entrées (une par amélioration de distance).
 */

/**
 * @struct HeapItem
 * @brief Une entrée du tas.
 */
typedef struct HeapItem {
  double key; /**< priorité (distance, ou f = g + h). */
  int node;   /**< nœud associé. */
} HeapItem;

/**
 * @struct MinHeap
 * @brief Tas min binaire stocké dans un tableau.
 *
 * Pour l'indice i : père = (i - 1) / 2, fils = 2i + 1 et 2i + 2.
 */
typedef struct MinHeap {
  HeapItem *items; /**< tableau des entrées. */
  int size;        /**< nombre d'entrées présentes. */
  int capacity;    /**< taille allouée de `items`. */
} MinHeap;

/**
 * @brief Alloue un tas vide.
 * @param capacity capacité initiale (capacity >= 1).
 * @return le tas, ou NULL si `capacity` est invalide ou si l'allocation échoue.
 * @note À libérer avec heap_free().
 */
MinHeap *heap_new(int capacity);

/**
 * @brief Insère l'entrée (key, node).
 * @return 1 en cas de succès, 0 si `heap` est NULL ou si la croissance échoue
 *         (le tas reste alors valide et inchangé).
 */
int heap_push(MinHeap *heap, double key, int node);

/**
 * @brief Extrait l'entrée de plus petite clé.
 * @param out reçoit l'entrée extraite.
 * @return 1 en cas de succès, 0 si le tas est vide ou si un paramètre est NULL.
 */
int heap_pop(MinHeap *heap, HeapItem *out);

/**
 * @brief Teste si le tas est vide.
 * @return 1 si vide ou NULL, 0 sinon.
 */
int heap_is_empty(const MinHeap *heap);

/**
 * @brief Libère le tas.
 * @param heap tas à libérer (NULL accepté).
 */
void heap_free(MinHeap *heap);

#endif