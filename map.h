#ifndef MAPMANAGER__MAP_H_
#define MAPMANAGER__MAP_H_

#include <stdint.h>

#pragma pack(2)
struct record {
  uint16_t record_size;
  uint32_t link_id;
  uint16_t road_name_size_plus_4;
  struct {
    __unused uint32_t reserved: 24;
    uint8_t class_number: 4;
    uint8_t branch: 3;
    uint8_t has_road_name: 1;
  };
  __unused uint32_t unknown;
  char road_name[1];
};
#pragma pack()

struct record *next_record();
void open_input_file(const char *filename);
void rewind_input_file();
void open_output_file(const char *filename);
void close_output_file();
void append_record(struct record *record);
void print_record(struct record *record);

struct record_list_node {
  struct record_list_node *prev;
  struct record *record;
  struct record_list_node *next;
};

struct record_list {
  struct record_list_node *head, *tail;
};

typedef int(*record_binary_comparator_t)(struct record *a, struct record *b);
typedef int(*record_unary_comparator_t)(struct record *record);

int record_list_build(struct record_list *list);
void record_list_release(struct record_list *list);
void record_list_bubble_sort(struct record_list *list, record_binary_comparator_t cmp);
void record_list_insertion_sort(struct record_list *list, record_binary_comparator_t cmp);
void record_list_quick_sort(struct record_list *list, record_binary_comparator_t cmp);

int record_list_traverse_search(struct record_list *list, record_unary_comparator_t cmp, struct record_list *result);

struct record_array {
  struct record **records;
  int size;
};

void record_array_build(struct record_array *array);
void record_array_release(struct record_array *array);
void record_array_bubble_sort(struct record_array *array, record_binary_comparator_t cmp);
void record_array_insertion_sort(struct record_array *array, record_binary_comparator_t cmp);
void record_array_quick_sort(struct record_array *array, record_binary_comparator_t cmp);

int record_array_traverse_search(struct record_array *array, record_unary_comparator_t cmp, struct record_list *result);
int record_array_binary_search(struct record_array *array,
                               record_binary_comparator_t cmp,
                               struct record_list *result,
                               struct record *record);

#endif //MAPMANAGER__MAP_H_
