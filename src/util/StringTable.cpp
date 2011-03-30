#include <cstring>
#include "StringTable.h"

StringTable::~StringTable()
{
  for (size_t i=0; i<m_strs.size(); i++)
    delete[] m_strs[i];
}

unsigned int StringTable::id(const char *str)
{
  unsigned int ret;
  if (find(str, ret))
    return ret;
  else
    return add(str);
}

bool StringTable::find(const char *str, unsigned int &where)
{
  for (size_t i=0; i<m_strs.size(); i++)
    if (strcmp(str, m_strs[i]) == 0)
    {
      where = i;
      return true;
    }
  return false;
}

unsigned int StringTable::add(const char *str)
{
  size_t length = strlen(str);
  char *stored = new char[length+1];
  memcpy(stored, str, length+1);
  m_strs.push_back(stored);
  return m_strs.size()-1;
}

