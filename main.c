#include "map.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

static struct record_list record_list, result;
static struct record_array record_array;

// 1. link_id
// 2. road_name
static int field_to_sort_or_find;

// 1. ascend
// 2. descend
static int asc_or_desc;

static int input_file_open;

static char buffer[100];
static struct record *record_to_search = (struct record *) buffer;
const size_t road_name_max_size = sizeof buffer - (size_t) (&((struct record *) 0)->road_name);

static iconv_t iconv_handle;

static int record_binary_comparator(struct record *a, struct record *b) {
  if (field_to_sort_or_find == 1) {
    int a_link_id = ntohl(a->link_id), b_link_id = ntohl(b->link_id);
    if (asc_or_desc == 1) {
      return a_link_id - b_link_id;
    } else {
      return b_link_id - a_link_id;
    }
  } else {
    char a_str[100] = {0}, b_str[100] = {0};
    if (a->has_road_name) {
      memcpy(a_str, a->road_name, ntohs(a->road_name_size_plus_4) - 4);
    }
    if (b->has_road_name) {
      memcpy(b_str, b->road_name, ntohs(b->road_name_size_plus_4) - 4);
    }
    if (asc_or_desc == 1) {
      return strcmp(a_str, b_str);
    } else {
      return strcmp(b_str, a_str);
    }
  }
}

static int record_unary_comparator(struct record *record) {
  if (field_to_sort_or_find == 1) {
    return record->link_id == record_to_search->link_id;
  } else {
    return strstr(record->road_name, record_to_search->road_name) != NULL;
  }
}

static void display_sort_search_menu(int is_list, int operation) {
  int choice;
  while (1) {
    if (operation == 1 || operation == 2 || operation == 3) {
      puts("请选择要比较的字段");
      puts("1. 按 link_id 升序排序");
      puts("2. 按 link_id 降序排序");
      puts("3. 按 road_name 升序排序");
      puts("4. 按 road_name 降序排序");
      puts("0. 返回");
    } else {
      puts("请选择要查找的字段");
      puts("1. 按 link_id 查找");
      puts("2. 按 road_name 查找");
      puts("0. 返回");
    }
    printf("请选择要进行的操作：");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
      case 0:return;
      case 1:
        if (operation == 1 || operation == 2 || operation == 3) {
          field_to_sort_or_find = 1;
          asc_or_desc = 1;
        } else {
          field_to_sort_or_find = 1;
          printf("请输入 link_id：");
          scanf("%d", &record_to_search->link_id);
          record_to_search->link_id = htonl(record_to_search->link_id);
        }
        break;
      case 2:
        if (operation == 1 || operation == 2 || operation == 3) {
          field_to_sort_or_find = 1;
          asc_or_desc = 2;
        } else {
          field_to_sort_or_find = 2;
          printf("请输入 road_name：");
          char *road_name_utf8 = NULL, *road_name_gbk = record_to_search->road_name;
          size_t road_name_utf8_size = 0, road_name_gbk_left_bytes = road_name_max_size, road_name_gbk_size;
          getline(&road_name_utf8, &road_name_utf8_size, stdin);
          road_name_utf8_size = strlen(road_name_utf8) - 1;
          road_name_utf8[road_name_utf8_size] = 0;
          if (iconv_handle == 0) {
            iconv_handle = iconv_open("GBK", "UTF8");
          }
          iconv(iconv_handle, &road_name_utf8, &road_name_utf8_size, &road_name_gbk, &road_name_gbk_left_bytes);
          road_name_gbk_size = road_name_max_size - road_name_gbk_left_bytes;
          if (road_name_gbk_size != 0) {
            record_to_search->road_name_size_plus_4 = htons(road_name_gbk_size + 4);
            record_to_search->has_road_name = 1;
          }
          else {
            record_to_search->road_name_size_plus_4 = 0xFFFF;
            record_to_search->has_road_name = 0;
          }
        }
        break;
      case 3:
        if (operation == 1 || operation == 2 || operation == 3) {
          field_to_sort_or_find = 2;
          asc_or_desc = 1;
        } else {
          continue;
        }
        break;
      case 4:
        if (operation == 1 || operation == 2 || operation == 3) {
          field_to_sort_or_find = 2;
          asc_or_desc = 2;
        } else {
          continue;
        }
        break;
      default:continue;
    }
    switch (operation) {
      case 1: {
        if (is_list) {
          record_list_bubble_sort(&record_list, record_binary_comparator);
        } else {
          record_array_bubble_sort(&record_array, record_binary_comparator);
        }
        break;
      }
      case 2: {
        if (is_list) {
          record_list_insertion_sort(&record_list, record_binary_comparator);
        } else {
          record_array_insertion_sort(&record_array, record_binary_comparator);
        }
        break;
      }
      case 3: {
        if (is_list) {
          record_list_quick_sort(&record_list, record_binary_comparator);
        } else {
          record_array_quick_sort(&record_array, record_binary_comparator);
        }
        break;
      }
      case 4: {
        if (is_list) {
          record_list_traverse_search(&record_list, record_unary_comparator, &result);
        } else {
          record_array_traverse_search(&record_array, record_unary_comparator, &result);
        }
        for (struct record_list_node *current = result.head; current != NULL; current = current->next) {
          print_record(current->record);
        }
        break;
      }
      case 5: {
        struct record *record_to_find = (struct record *) buffer;
        record_array_binary_search(&record_array, record_binary_comparator, &result, record_to_find);
        for (struct record_list_node *current = result.head; current != NULL; current = current->next) {
          print_record(current->record);
        }
      }
      default:break;
    }
  }
}

static void display_operation_menu(int is_list) {
  int choice;
  while (1) {
    puts("1. 冒泡排序");
    puts("2. 插入排序");
    puts("3. 快速排序");
    puts("4. 顺序查找");
    if (!is_list) {
      puts("5. 二分查找");
    }
    puts("0. 返回");
    printf("请选择要进行的操作：");
    scanf("%d", &choice);
    getchar();
    if (choice == 0) {
      return;
    } else if (choice == 1) {
      display_sort_search_menu(is_list, 1);
    } else if (choice == 2) {
      display_sort_search_menu(is_list, 2);
    } else if (choice == 3) {
      display_sort_search_menu(is_list, 3);
    } else if (choice == 4) {
      display_sort_search_menu(is_list, 4);
    } else if (choice == 5 && !is_list) {
      display_sort_search_menu(is_list, 5);
    }
  }
}

static void display_store_type_menu() {
  int choice;
  while (1) {
    puts("请选择数据在内存中的存储方式");
    puts("1. 使用链表存储");
    puts("2. 使用数组存储");
    puts("0. 返回");
    printf("请选择要进行的操作：");
    scanf("%d", &choice);
    getchar();
    if (choice == 0) {
      return;
    } else if (choice == 1) {
      if (record_list.head != NULL) {
        record_list_release(&record_list);
      }
      if (record_array.records != NULL) {
        record_array_release(&record_array);
      }
      int size = record_list_build(&record_list);
      printf("共 %d 条记录\n", size);
      display_operation_menu(1);
    } else if (choice == 2) {
      if (record_list.head != NULL) {
        record_list_release(&record_list);
      }
      if (record_array.records != NULL) {
        record_array_release(&record_array);
      }
      record_array_build(&record_array);
      printf("共 %d 条记录\n", record_array.size);
      display_operation_menu(0);
    }
  }
}

static void display_main_menu() {
  int choice;
  while (1) {
    puts("地图管理系统");
    puts("1. 载入文件");
    if (input_file_open) {
      puts("2. 输出文件内容");
      puts("3. 管理");
      if (record_array.records != NULL || record_list.head != NULL) {
        puts("4. 保存文件");
      }
    }
    puts("0. 退出");
    printf("请选择要进行的操作：");
    scanf("%d", &choice);
    getchar();
    if (choice == 0) {
      return;
    } else if (choice == 1) {
      printf("文件路径：");
      char *filename = NULL;
      size_t filename_size = 0;
      getline(&filename, &filename_size, stdin);
      filename[strlen(filename) - 1] = 0;
      open_input_file(filename);
      free(filename);
      input_file_open = 1;
    } else if (choice == 2 && input_file_open) {
      if (record_array.records != NULL) {
        for (int i = 0; i < record_array.size; i++) {
          print_record(record_array.records[i]);
        }
      } else if (record_list.head != NULL) {
        for (struct record_list_node *current = record_list.head; current != NULL; current = current->next) {
          print_record(current->record);
        }
      } else {
        struct record *record;
        while ((record = next_record()) != NULL) {
          print_record(record);
        }
      }
    } else if (choice == 3 && input_file_open) {
      display_store_type_menu();
    } else if (choice == 4 && input_file_open && (record_array.records != NULL || record_list.head != NULL)) {
      printf("请输入要保存的文件名：");
      char *filename = NULL;
      size_t filename_size = 0;
      getline(&filename, &filename_size, stdin);
      filename[strlen(filename) - 1] = 0;
      open_output_file(filename);
      free(filename);
      if (record_array.records != NULL) {
        for (int i = 0; i < record_array.size; i++) {
          append_record(record_array.records[i]);
        }
      } else if (record_list.head != NULL) {
        for (struct record_list_node *current = record_list.head; current != NULL; current = current->next) {
          append_record(current->record);
        }
      }
      close_output_file();
    }
  }
}

int main() {
  display_main_menu();
}
