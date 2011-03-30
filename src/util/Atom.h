#ifndef ATOM_H
#define ATOM_H

#include "StringTable.h"

class Atom
{
  public:
    Atom(const char *str, StringTable *table)
      : m_id(table->id(str)), m_table(table) {}
    Atom(unsigned int id=0, StringTable *table=NULL)
      : m_id(id), m_table(table) {}

    const char *c_str() const { return m_table->str(m_id); }
    unsigned int id() const { return m_id; }

  private:
    unsigned int m_id;
    StringTable *m_table;
};

#endif // ATOM_H

