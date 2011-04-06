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

    Value getVar(StringTable::Ref id) const;
    void setVar(StringTable::Ref id, const Value &val);
    bool isVar(StringTable::Ref id) const;
  private:
    Map<StringTable::Ref, Value> m_vars;
};

#endif // SCOPE_H
