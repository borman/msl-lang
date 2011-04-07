#ifndef ATOM_H
#define ATOM_H

#include <cstring>
#include "StringTable.h"

class Atom
{
  public:
    Atom(const char *str, StringTable *table)
      : m_id(table->id(str)), m_table(table) {}
    Atom(StringTable::Ref id=0, StringTable *table=NULL)
      : m_id(id), m_table(table) {}

    const char *c_str() const { return m_table->str(m_id); }
    StringTable::Ref id() const { return m_id; }

    bool operator ==(const char *str) const { return 0 == strcmp(c_str(), str); }
  private:
    StringTable::Ref m_id;
    StringTable *m_table;
};

#endif // ATOM_H

