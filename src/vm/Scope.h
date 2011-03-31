#ifndef SCOPE_H
#define SCOPE_H

#include "Value.h"
#include "Map.h"

class Scope
{
  public:
    // Exception
    class VarNotFound {};

    Value getVar(unsigned int id) const;
    void setVar(unsigned int id, const Value &val);
  private:
    Map<unsigned int, Value> m_vars;
};

#endif // SCOPE_H
