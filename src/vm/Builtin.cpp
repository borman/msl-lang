#include "Builtin.h"
#include "File.h"

ListedBuiltin::ListedBuiltin(StringTable *strings, const Definition *defs, size_t count)
  : m_bindings(new Binding[count]), m_bindingCount(count)
{
  for (size_t i=0; i<count; i++)
  {
    m_bindings[i].name = strings->id(defs[i].name);
    m_bindings[i].func = defs[i].func;
  }
}

ListedBuiltin::~ListedBuiltin()
{
  delete[] m_bindings;
}

bool ListedBuiltin::call(unsigned int name, Context &context)
{
  for (size_t i=0; i<m_bindingCount; i++)
    if (name == m_bindings[i].name)
    {
      m_bindings[i].func(this, context);
      return true;
    }
  return false;
}

