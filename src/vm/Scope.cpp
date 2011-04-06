#include "Scope.h"


Value Scope::getVar(StringTable::Ref id) const
{
  try
  {
    return m_vars.at(id);
  }
  catch (Map<StringTable::Ref, Value>::BadIndex)
  {
    throw VarNotFound();
  }
}

bool Scope::isVar(StringTable::Ref id) const
{
  return m_vars.count(id) > 0;
}

void Scope::setVar(StringTable::Ref id, const Value &val)
{
  m_vars[id] = val;
}
