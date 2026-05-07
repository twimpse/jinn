// data_storage.c - corrected version

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "data_storage.h"

#define MAX_TOTAL_SIZE (512 * 1024 * 1024)
#define MAX_FILE_COUNT 4096

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

int file_add(const char *filename, const void *data, size_t size, 
             int min_level, int group, unsigned int read_mask, 
             unsigned int write_mask, unsigned int exec_mask, uid_t owner)
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
  time_t now = time(NULL);
  entry->created = now;
  entry->last_access = now;
  entry->last_modified = now;
  entry->times_accessed = 0;
  entry->used = 1;
  
  entry->access.min_user_level = min_level;
  entry->access.owner_group = group;
  entry->access.read_mask = read_mask;
  entry->access.write_mask = write_mask;
  entry->access.exec_mask = exec_mask;
  entry->owner_id = owner;

  total_file_count++;
  total_storage_bytes += size;
  return 0;
}

struct file_entry *file_read(const char *filename, user_t *user)
{
  int idx = find_file_index(filename);
  if (idx == -1)
    return NULL;

  struct file_entry *entry = &file_store[idx];
  
  if (!can_access(entry, user, PERM_READ))
    return NULL;
  
  entry->last_access = time(NULL);
  entry->times_accessed++;
  return entry;
}

int file_delete(const char *filename, user_t *user)
{
  int idx = find_file_index(filename);
  if (idx == -1)
    return -1;

  struct file_entry *entry = &file_store[idx];
  
  if (!can_access(entry, user, PERM_WRITE))
    return -2;
  
  // Save size before clearing
  size_t file_size = entry->size;
  
  free(entry->filename);
  free(entry->data);
  memset(entry, 0, sizeof(struct file_entry));

  total_file_count--;
  total_storage_bytes -= file_size;  // Use saved size
  return 0;
}

int file_set_permissions(const char *filename, user_t *user, 
                         int min_level, int group, 
                         unsigned int read_mask, 
                         unsigned int write_mask, 
                         unsigned int exec_mask)
{
  int idx = find_file_index(filename);
  if (idx == -1)
    return -1;
  
  struct file_entry *entry = &file_store[idx];
  
  // Owner (by group) or admin (level 0) can change permissions
  if (user->user_level != 0 && user->group_number != entry->access.owner_group)
    return -2;
  
  entry->access.min_user_level = min_level;
  entry->access.owner_group = group;
  entry->access.read_mask = read_mask;
  entry->access.write_mask = write_mask;
  entry->access.exec_mask = exec_mask;
  entry->last_modified = time(NULL);
  
  return 0;
}

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
  return matches;
}

int can_access(struct file_entry *f, user_t *u, int op)
{
  if (!f || !u)
    return 0;
    
  // Group owner gets full access regardless of level
  if (u->group_number == f->access.owner_group)
    return 1;

  // Non-group members must meet level requirement
  if (u->user_level > f->access.min_user_level)
    return 0;

  // Check specific permission
  if (op == PERM_READ) 
    return (f->access.read_mask & 1);
  if (op == PERM_WRITE) 
    return (f->access.write_mask & 1);
  if (op == PERM_EXEC) 
    return (f->access.exec_mask & 1);

  return 0;
}