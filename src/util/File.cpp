#include <cstdio>
#include <cerrno>
#include <cstdarg>
#include "File.h"

File cout(stdout);
File cerr(stderr);
File cin(stdin);

File::File(const char *name, Mode mode)
  : m_own(true)
{
  const char *s_mode;
  switch (mode)
  {
    case Read: s_mode = "r"; break;
    case Write: s_mode = "w"; break;
  }
  m_file = fopen(name, s_mode);
  if (m_file == NULL)
    throw Exception(errno);
}

File::File(FILE *exfile)
  : m_file(exfile), m_own(false)
{
}

File::~File()
{
  if (m_own && m_file != NULL)
    close();
}

int File::printf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  int retval = vfprintf(m_file, format, args);
  va_end(args);
  if (retval<0)
    throw Exception(errno);
  return retval;
}

int File::getc()
{
  return fgetc(m_file);
}

void File::close()
{
  if (0 != fclose(m_file))
    throw Exception(errno);
  m_file = NULL;
}
