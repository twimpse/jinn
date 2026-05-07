#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TOTAL_SIZE (512 * 1024 * 1024)
#define MAX_FILE_COUNT 4096

struct file_entry
{
  char *filename;
  void *data;
  size_t size;
  time_t created;
  time_t last_access;
  unsigned int times_accessed;
  int used;                     // marks slot as used
};

static struct file_entry file_store[MAX_FILE_COUNT];
static size_t total_storage_bytes = 0;
static int total_file_count = 0;

static int find_file_index(const char *filename)
{
  for (int i = 0; i < MAX_FILE_COUNT; i++)
    if (file_store[i].used && strcmp(file_store[i].filename, filename) == 0)
      return i;
  return -1;
}

static int find_free_slot(void)
{
  for (int i = 0; i < MAX_FILE_COUNT; i++)
    if (!file_store[i].used)
      return i;
  return -1;
}

int file_can_add(size_t file_size)
{
  if (total_file_count >= MAX_FILE_COUNT)
    return 0;
  if (total_storage_bytes + file_size > MAX_TOTAL_SIZE)
    return 0;
  return 1;
}

void file_storage_info(int *file_count, size_t *total_bytes)
{
  if (file_count)
    *file_count = total_file_count;
  if (total_bytes)
    *total_bytes = total_storage_bytes;
}

int file_add(const char *filename, const void *data, size_t size)
{
  if (!filename || !data || size == 0)
    return -1;
  if (!file_can_add(size))
    return -2;
  if (find_file_index(filename) != -1)
    return -3;

  int slot = find_free_slot();
  if (slot == -1)
    return -4;

  struct file_entry *entry = &file_store[slot];
  entry->filename = strdup(filename);
  entry->data = malloc(size);
  if (!entry->filename || !entry->data)
    {
      free(entry->filename);
      free(entry->data);
      entry->used = 0;
      return -4;
    }

  memcpy(entry->data, data, size);
  entry->size = size;
  entry->created = time(NULL);
  entry->last_access = entry->created;
  entry->last_modified = entry->created;
  entry->used = 1;

  total_file_count++;
  total_storage_bytes += size;
  return 0;
}

struct file_entry *file_read(const char *filename)
{
  int idx = find_file_index(filename);
  if (idx == -1)
    return NULL;

  file_store[idx].last_access = time(NULL);
  file_store[idx].times_accessed++;
  return &file_store[idx];
}


int file_delete(const char *filename)
{
  int idx = find_file_index(filename);
  if (idx == -1)
    return -1;

  struct file_entry *entry = &file_store[idx];
  free(entry->filename);
  free(entry->data);
  memset(entry, 0, sizeof(struct file_entry));

  total_file_count--;
  total_storage_bytes -= entry->size;
  return 0;
}
/*
useage:

char **files = file_list();
for (int i = 0; files[i]; i++)
    printf("%s\n", files[i]);
file_list_free(files);
*/


// Returns array of filenames, caller must free with file_list_free()
char **file_list(void)
{
  char **names = malloc(sizeof(char *) * (total_file_count + 1));
  if (!names)
    return NULL;

  int idx = 0;
  for (int i = 0; i < MAX_FILE_COUNT; i++)
    if (file_store[i].used)
      names[idx++] = strdup(file_store[i].filename);
  names[idx] = NULL;
  return names;
}

void file_list_free(char **list)
{
  if (!list)
    return;
  for (int i = 0; list[i]; i++)
    free(list[i]);
  free(list);
}

// Search by partial name match
char **file_search(const char *pattern)
{
  char **matches = malloc(sizeof(char *) * (total_file_count + 1));
  if (!matches)
    return NULL;

  int idx = 0;
  for (int i = 0; i < MAX_FILE_COUNT; i++)
    if (file_store[i].used && strstr(file_store[i].filename, pattern))
      matches[idx++] = strdup(file_store[i].filename);
  matches[idx] = NULL;
  return matches;               // free with file_list_free()
}
