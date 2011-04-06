#ifndef STRINGTABLE_H
#define STRINGTABLE_H

#include "Vector.h"

class StringTable
{
  public:
    typedef unsigned int Ref;

    ~StringTable();

    const char *str(Ref id) const { return m_strs[id]; }
    Ref id(const char *str);
  private:
    bool find(const char *str, Ref &where);
    Ref add(const char *str);
    Vector<char *> m_strs;
};

#endif // STRINGTABLE_H

