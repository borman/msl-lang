#include "Scope.h"


Value Scope::getVar(unsigned int id) const
{
  try
  {
    return m_vars.at(id);
  }
  catch (Map<unsigned int, Value>::BadIndex)
  {
    throw VarNotFound();
  }
}

bool Scope::isVar(unsigned int id) const
{
  return m_vars.count(id) > 0;
}

void Scope::setVar(unsigned int id, const Value &val)
{
  m_vars[id] = val;
}
