#ifndef BUILTIN_H
#define BUILTIN_H

#include "Stack.h"
#include "Value.h"
#include "StringTable.h"
#include "Context.h"

/**
 * A builtin function call handler.
 * 
 * Dispatches function calls. Several AbstractBuiltin handlers may be
 * installed in a single Executor.
 */
class AbstractBuiltin
{
  public:
    virtual bool call(unsigned int name, Context &context) = 0;
};

class ListedBuiltin: public AbstractBuiltin
{
  public:
    typedef void (*Function)(ListedBuiltin *self, Context &context);
    struct Definition
    {
      const char *name;
      Function func;
    };

    ListedBuiltin(StringTable *strings, const Definition *defs, size_t count);
    ~ListedBuiltin();

    virtual bool call(unsigned int name, Context &context);

  private:
    struct Binding
    {
      unsigned int name;
      Function func;
    };

    Binding *m_bindings;
    size_t m_bindingCount;
};

#endif // BUILTIN_H
