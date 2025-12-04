#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("usage: mv old_path new_path\n");
    exit(1);
  }

  char *old_path = argv[1];
  char *new_path = argv[2];
  struct stat st_old, st_new;

  // Check if source file exists
  if (stat(old_path, &st_old) < 0)
  {
    printf("mv: cannot stat source '%s'\n", old_path);
    exit(1);
  }

  // Check if destination exists
  int dest_exists = (stat(new_path, &st_new) == 0);

  // If destination exists and is a directory, construct new path with basename
  char new_file_path[512];
  if (dest_exists && st_new.type == T_DIR)
  {
    char *basename = old_path;

    // Find the last '/' in old_path to get basename
    for (char *p = old_path; *p; p++)
    {
      if (*p == '/')
        basename = p + 1;
    }

    // Manually construct the path (no snprintf in xv6)
    char *dst = new_file_path;
    char *src = new_path;
    while (*src)
    {
      *dst++ = *src++;
    }
    *dst++ = '/';
    src = basename;
    while (*src)
    {
      *dst++ = *src++;
    }
    *dst = '\0';

    new_path = new_file_path;
  }

  // Create hard link to new location
  if (link(old_path, new_path) < 0)
  {
    printf("mv: link failed from '%s' to '%s'\n", old_path, new_path);
    exit(1);
  }

  // Remove old link
  if (unlink(old_path) < 0)
  {
    printf("mv: failed to unlink source file '%s'. File may now exist as two copies.\n", old_path);
    exit(1);
  }

  exit(0);
}
