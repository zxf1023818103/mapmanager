#include <fcntl.h>
#include <iconv.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "map.h"

static off_t file_size;
static uint8_t *input_begin, *input_current, *output_begin, *output_current;
static int input_fd, output_fd;
static iconv_t iconv_handle;

struct record *next_record() {
  if (input_begin == NULL) {
    return NULL;
  }
  if (input_current == NULL) {
    input_current = input_begin;
    return (struct record *) input_current;
  }
  if (input_current - input_begin >= file_size) {
    return NULL;
  } else {
    struct record* record = (struct record *) input_current;
    uint16_t offset = ntohs(((struct record *) input_current)->record_size);
    input_current += offset;
    return record;
  }
}

void open_input_file(const char *filename) {
  input_fd = open(filename, O_RDONLY);
  if (input_fd == -1) {
    perror(filename);
    _exit(-1);
  }
  file_size = lseek(input_fd, 0, SEEK_END);
  lseek(input_fd, 0, SEEK_SET);
  input_begin = mmap(NULL, (size_t) file_size, PROT_READ, MAP_SHARED, input_fd, 0);
  input_current = NULL;
  if (input_begin == MAP_FAILED) {
    perror("mmap");
    _exit(-1);
  }
}

void rewind_input_file() {
  input_current = input_begin;
}

void close_output_file() {
  munmap(output_begin, file_size);
  output_begin = output_current = NULL;
  close(output_fd);
  output_fd = 0;
}

void open_output_file(const char *filename) {
  output_fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
  if (output_fd == -1) {
    perror(filename);
    _exit(-1);
  }
  if (ftruncate(output_fd, file_size)) {
    perror("ftruncate");
    _exit(-1);
  }
  output_begin = output_current = mmap(NULL, (size_t) file_size, PROT_READ | PROT_WRITE, MAP_SHARED, output_fd, 0);
  if (output_begin == MAP_FAILED) {
    perror("mmap");
    _exit(-1);
  }
}

void append_record(struct record *record) {
  memcpy(output_current, record, ntohs(record->record_size));
  output_current += ntohs(record->record_size);
}

void print_record(struct record *record) {
  printf("record_size: %" PRIu16
         ", link_id: %" PRIu32
         ", road_name_size_plus_4: %" PRIu16
         ", class_number: %" PRIu8
         ", branch: %" PRIu8
         ", has_road_name: %" PRIu8
         ", road_name:",
         ntohs(record->record_size),
         ntohl(record->link_id),
         ntohs(record->road_name_size_plus_4),
         record->class_number,
         record->branch,
         record->has_road_name);
  if (record->has_road_name) {
    if (iconv_handle == 0) {
      iconv_handle = iconv_open("UTF8", "GBK");
    }
    char inbuf[1024], outbuf[1024];
    size_t road_name_gbk_size = ntohs(record->road_name_size_plus_4) - 4, road_name_utf8_size = sizeof outbuf;
    char *road_name_gbk = inbuf, *road_name_utf8 = outbuf;
    memcpy(inbuf, record->road_name, road_name_gbk_size);
    road_name_utf8 = outbuf;
    iconv(iconv_handle, &road_name_gbk, &road_name_gbk_size, &road_name_utf8, &road_name_utf8_size);
    outbuf[sizeof outbuf - road_name_utf8_size] = 0;
    printf(" %s", outbuf);
  }
  puts("");
}
