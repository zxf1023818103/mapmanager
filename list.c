#include "map.h"
#include <stddef.h>
#include <stdlib.h>

int record_list_build(struct record_list *list) {
  rewind_input_file();

  int size = 0;
  struct record_list_node *head = NULL, *current = NULL;

  struct record *record;
  while ((record = next_record()) != NULL) {
    size++;
    struct record_list_node *new_node = calloc(1, sizeof(struct record_list_node));
    if (current == NULL) {
      head = current = new_node;
    } else {
      current->next = new_node;
      new_node->prev = current;
      current = current->next;
    }
    current->record = record;
  }

  list->head = head;
  list->tail = current;
  return size;
}

void record_list_release(struct record_list *list) {
  for (struct record_list_node *current = list->head; current != NULL;) {
    struct record_list_node *next = current->next;
    free(current);
    current = NULL;
    current = next;
  }
}

void record_list_bubble_sort(struct record_list *list, record_binary_comparator_t cmp) {
  for (struct record_list_node *i = list->tail; i != list->head; i = i->prev) {
    for (struct record_list_node *j = list->head; j != i; j = j->next) {
      if (cmp(j->record, j->next->record) > 0) {
        struct record *t = j->record;
        j->record = j->next->record;
        j->next->record = t;
      }
    }
  }
}

void record_list_insertion_sort(struct record_list *list, record_binary_comparator_t cmp) {
  for (struct record_list_node *i = list->head; i != NULL; i = i->next) {
    struct record_list_node *min = i;
    for (struct record_list_node *j = i; j != NULL; j = j->next) {
      if (cmp(min->record, j->record) > 0) {
        min = j;
      }
    }
    struct record *t = min->record;
    min->record = i->record;
    i->record = t;
  }
}

static void record_list_quick_sort_internal(struct record_list_node *head,
                                            struct record_list_node *tail,
                                            record_binary_comparator_t cmp) {
  if (head == tail) {
    return;
  }

  struct record_list_node *pivot = head, *i = head, *j = tail;
  while (i != j) {
    while (cmp(j->record, pivot->record) >= 0 && i != j) {
      j = j->prev;
    }
    while (cmp(i->record, pivot->record) <= 0 && i != j) {
      i = i->next;
    }
    struct record *t = i->record;
    i->record = j->record;
    j->record = t;
  }
  struct record *t = i->record;
  i->record = pivot->record;
  pivot->record = t;

  record_list_quick_sort_internal(head, i->prev, cmp);
  record_list_quick_sort_internal(i, tail, cmp);
}

void record_list_quick_sort(struct record_list *list, record_binary_comparator_t cmp) {
  record_list_quick_sort_internal(list->head, list->tail, cmp);
}

int record_list_traverse_search(struct record_list *list, record_unary_comparator_t cmp, struct record_list *result) {
  if (result->head != NULL) {
    record_list_release(result);
  }

  int result_size = 0;
  struct record_list_node *result_head = NULL, *result_current = NULL;

  for (struct record_list_node *current = list->head; current != NULL; current = current->next) {
    if (cmp(current->record)) {
      struct record_list_node *new_node = calloc(1, sizeof(struct record_list_node));
      if (result_size == 0) {
        result_head = result_current = new_node;
      } else {
        result_current->next = new_node;
        new_node->prev = result_current;
        result_current = result_current->next;
      }
      result_current->record = current->record;
      ++result_size;
    }
  }

  result->head = result_head;
  result->tail = result_current;
  return result_size;
}
