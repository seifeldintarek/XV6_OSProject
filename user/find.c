#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

// Structure to hold directory information
struct dir_info
{
  char path[64]; // Reduced from 128 to 64 bytes
  int file_count;
};

#define MAX_SUBDIRS 4 // Reduced from 8 to 4: 4 * 68 bytes ≈ 272 bytes

// Count files in a directory (non-recursively)
int count_files(char *path)
{
  int fd, count = 0;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, 0)) < 0)
    return 0;

  if (fstat(fd, &st) < 0)
  {
    close(fd);
    return 0;
  }

  if (st.type != T_DIR)
  {
    close(fd);
    return 0;
  }

  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0)
      continue;
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;
    count++;
  }

  close(fd);
  return count;
}

// Find max element and return its index
int find_max_index(struct dir_info *dirs, int n, int *used)
{
  int max_idx = -1;
  int max_count = -1;
  int i;

  for (i = 0; i < n; i++)
  {
    if (!used[i] && dirs[i].file_count > max_count)
    {
      max_count = dirs[i].file_count;
      max_idx = i;
    }
  }

  return max_idx;
}

// Recursive find function
void find(char *path, char *target)
{
  char buf[64]; // Reduced from 128 to 64 bytes
  char *p;
  int fd;
  struct dirent de;
  struct stat st;
  struct dir_info subdirs[MAX_SUBDIRS];
  int used[MAX_SUBDIRS]; // Track which subdirs we've processed
  int subdir_count = 0;
  int i, max_idx;

  // Open the directory
  if ((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  // Handle if path is a file
  if (st.type == T_FILE)
  {
    // Extract filename from path
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
      ;
    p++;
    if (strcmp(p, target) == 0)
      printf("%s\n", path);
    close(fd);
    return;
  }

  if (st.type != T_DIR)
  {
    close(fd);
    return;
  }

  // Build path buffer
  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
  {
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  // First pass: check files in current directory and collect subdirectories
  while (read(fd, &de, sizeof(de)) == sizeof(de))
  {
    if (de.inum == 0)
      continue;

    // Skip . and ..
    if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
      continue;

    // Build full path
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    // Get stat for this entry
    if (stat(buf, &st) < 0)
    {
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    // If it's a file and matches target, print it
    if (st.type == T_FILE)
    {
      if (strcmp(de.name, target) == 0)
        printf("%s\n", buf);
    }
    // If it's a directory, add to subdirs list
    else if (st.type == T_DIR)
    {
      if (subdir_count < MAX_SUBDIRS)
      {
        // Store directory info
        strcpy(subdirs[subdir_count].path, buf);
        subdirs[subdir_count].file_count = count_files(buf);
        used[subdir_count] = 0;

        // Only count non-empty directories
        if (subdirs[subdir_count].file_count > 0)
        {
          subdir_count++;
        }
      }
      else
      {
        // If we hit the limit, just search it immediately without prioritization
        int fc = count_files(buf);
        if (fc > 0)
        {
          find(buf, target);
        }
      }
    }
  }

  close(fd);

  // Process subdirectories in order of file count (most files first)
  // Using selection approach instead of full sort to save stack space
  for (i = 0; i < subdir_count; i++)
  {
    max_idx = find_max_index(subdirs, subdir_count, used);
    if (max_idx >= 0)
    {
      used[max_idx] = 1;
      find(subdirs[max_idx].path, target);
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(2, "Usage: find <filename>\n");
    exit(1);
  }

  // Start search from current directory "."
  find(".", argv[1]);
  exit(0);
}
