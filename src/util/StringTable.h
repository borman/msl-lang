#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include "Vector.h"

class StringTable
{
  public:
    ~StringTable();

    const char *str(unsigned int id) const { return m_strs[id]; }
    unsigned int id(const char *str);
  private:
    bool find(const char *str, unsigned int &where);
    unsigned int add(const char *str);
    Vector<char *> m_strs;
};

#endif // STRINGTABLE_H

