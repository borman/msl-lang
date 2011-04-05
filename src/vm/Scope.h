#ifndef SCOPE_H
#define SCOPE_H

#include "Value.h"
#include "Map.h"

/**
 * A Scope is a key-value storage of variables
 *
 * Scopes are stacked to implement local variables
 */
class Scope
{
  public:
    // Exception
    class VarNotFound {};

    Value getVar(unsigned int id) const;
    void setVar(unsigned int id, const Value &val);
    bool isVar(unsigned int id) const;
  private:
    Map<unsigned int, Value> m_vars;
};

#endif // SCOPE_H
