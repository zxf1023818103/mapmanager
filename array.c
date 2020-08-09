#include "map.h"
#include <stddef.h>
#include <stdlib.h>

void record_array_build(struct record_array *array) {
  rewind_input_file();

  struct record **records = NULL;
  int size = 0;

  struct record *record;
  while ((record = next_record()) != NULL) {
    size++;
    if (size == 0) {
      records = malloc(sizeof(struct record *));
    } else {
      records = realloc(records, sizeof(struct record *) * size);
    }
    records[size - 1] = record;
  }

  array->records = records;
  array->size = size;
}

void record_array_release(struct record_array *array) {
  free(array->records);
  array->records = NULL;
  array->size = 0;
}

void record_array_bubble_sort(struct record_array *array, record_binary_comparator_t cmp) {
  for (int i = 0; i < array->size; i++) {
    for (int j = i; j < array->size; j++) {
      if (cmp(array->records[i], array->records[j]) > 0) {
        struct record *t = array->records[i];
        array->records[i] = array->records[j];
        array->records[j] = t;
      }
    }
  }
}

void record_array_insertion_sort(struct record_array *array, record_binary_comparator_t cmp) {
  for (int i = 0; i < array->size; i++) {
    struct record **min = array->records + i;
    for (int j = i; j < array->size; j++) {
      if (cmp(*min, array->records[j]) < 0) {
        min = array->records + j;
      }
    }
    struct record *t = *min;
    *min = array->records[i];
    array->records[i] = t;
  }
}

void record_array_quick_sort_internal(struct record **begin, struct record **end, record_binary_comparator_t cmp) {
  if (begin >= end - 1) {
    return;
  }

  struct record **i = begin, **j = end - 1, **pivot = begin;
  while (i != j) {
    while (cmp(*j, *pivot) >= 0) {
      j--;
    }
    while (cmp(*i, *pivot) >= 0) {
      i++;
    }
    struct record *t = *i;
    *i = *j;
    *j = t;
  }
  struct record *t = *pivot;
  *pivot = *i;
  *i = t;

  record_array_quick_sort_internal(begin, i, cmp);
  record_array_quick_sort_internal(i, end, cmp);
}

void record_array_quick_sort(struct record_array *array, record_binary_comparator_t cmp) {
  record_array_quick_sort_internal(array->records, array->records + array->size, cmp);
}

int record_array_traverse_search(struct record_array *array,
                                 record_unary_comparator_t cmp,
                                 struct record_list *result) {
  int result_size = 0;
  struct record_list_node *result_head = NULL, *result_current = NULL;

  for (int i = 0; i < array->size; i++) {
    if (cmp(array->records[i])) {
      struct record_list_node *new_node = calloc(1, sizeof(struct record_list_node));
      if (result_size == 0) {
        result_head = result_current = new_node;
      } else {
        result_current->next = new_node;
        new_node->prev = result_current;
        result_current = result_current->next;
      }
      result_current->record = array->records[i];
      ++result_size;
    }
  }

  result->head = result_head;
  result->tail = result_current;
  return result_size;
}

int record_array_binary_search(struct record_array *array,
                               record_binary_comparator_t cmp,
                               struct record_list *result,
                               struct record *record) {
  if (result->head != NULL) {
    record_list_release(result);
  }

  struct record **begin = array->records, **end = array->records + array->size;
  while (begin < end - 1) {
    struct record **mid = (end - begin) / 2 + begin;
    int cmp_result = cmp(*mid, record);
    if (cmp_result > 0) {
      end = mid;
    } else if (cmp_result < 0) {
      begin = mid;
    } else {
      struct record **upper_bound, **lower_bound;
      upper_bound = lower_bound = mid;
      while (cmp(lower_bound[-1], record) == 0) {
        lower_bound--;
      }
      while (cmp(*upper_bound, record) == 0) {
        upper_bound++;
      }

      struct record_list_node *result_head = NULL, *result_current = NULL;
      int result_size = 0;

      for (struct record **current = lower_bound; current != upper_bound; current++) {
        struct record_list_node *new_node = calloc(1, sizeof(struct record_list_node));
        if (result_size == 0) {
          result_head = result_current = new_node;
        } else {
          result_current->next = new_node;
          new_node->prev = result_current;
          result_current = result_current->next;
        }
        result_current->record = *current;
        ++result_size;
      }

      result->head = result_head;
      result->tail = result_current;

      return result_size;
    }
  }
  return 0;
}
