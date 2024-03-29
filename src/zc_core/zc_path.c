#ifndef zc_path_h
#define zc_path_h

#include "zc_cstring.c"

char* path_new_append(char* root, char* component);
char* path_new_remove_last_component(char* path);
char* path_new_extension(char* path);
char* path_new_filename(char* path);
char* path_new_normalize(char* path, char* execpath);

#endif

#if __INCLUDE_LEVEL__ == 0

#include "zc_memory.c"
#include <limits.h>
#ifdef __linux__
#include <linux/limits.h>
#endif
#include <string.h>

char* path_new_append(char* root, char* component)
{
  if (root[strlen(root) - 1] == '/')
    return cstr_new_format(PATH_MAX + NAME_MAX, "%s%s", root, component);
  else
    return cstr_new_format(PATH_MAX + NAME_MAX, "%s/%s", root, component);
}

char* path_new_remove_last_component(char* path)
{
  int index;
  for (index = strlen(path) - 2; index > -1; --index)
  {
    if (path[index] == '/')
    {
      index++;
      break;
    }
  }
  char* str = CAL(index + 1, NULL, cstr_describe);
  memcpy(str, path, index);
  return str;
}

char* path_new_extension(char* path)
{
  int index;
  for (index = strlen(path) - 1; index > -1; --index)
  {
    if (path[index] == '.')
    {
      index++;
      break;
    }
  }

  int   len = strlen(path) - index;
  char* ext = CAL(len + 1, NULL, cstr_describe);
  memcpy(ext, path + index, len);
  return ext;
}

char* path_new_filename(char* path)
{
  int dotindex;
  for (dotindex = strlen(path) - 1; dotindex > -1; --dotindex)
  {
    if (path[dotindex] == '.') break;
  }

  if (dotindex == -1) dotindex = strlen(path) - 1;

  int slashindex;
  for (slashindex = strlen(path) - 1; slashindex > -1; --slashindex)
  {
    if (path[slashindex] == '/')
    {
      slashindex++;
      break;
    }
  }
  if (slashindex == -1) slashindex = 0;
  int   len   = dotindex - slashindex;
  char* title = CAL(len + 1, NULL, cstr_describe);
  memcpy(title, path + slashindex, len);
  return title;
}

char* path_new_normalize(char* path, char* execpath)
{
  char* result = NULL;

  if (path[0] == '~') // if starts with tilde, insert home dir
    result = cstr_new_format(PATH_MAX + NAME_MAX, "%s%s", getenv("HOME"), path + 1);
  else if (path[0] != '/') // if doesn't start with / insert base dir
    result = cstr_new_format(PATH_MAX + NAME_MAX, "%s/%s", execpath, path);
  else
    result = cstr_new_cstring(path);

  // if ends with '/' remove it
  if (result[strlen(result) - 1] == '/') result[strlen(result) - 1] = '\0';

  return result;
}

#endif
