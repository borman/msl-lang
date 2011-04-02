#ifndef BUILTIN_H
#define BUILTIN_H

#include "Stack.h"
#include "Value.h"
#include "StringTable.h"
#include "Executor.h"

class AbstractBuiltin
{
  public:
    virtual bool call(unsigned int name, Executor::Context &context) = 0;
};

class ListedBuiltin: public AbstractBuiltin
{
  public:
    typedef void (*Function)(Executor::Context &context);
    struct Definition
    {
      const char *name;
      Function func;
    };

    ListedBuiltin(StringTable *strings, const Definition *defs, size_t count);
    ~ListedBuiltin();

    virtual bool call(unsigned int name, Executor::Context &context);

  private:
    struct Binding
    {
      unsigned int name;
      Function func;
    };

    Binding *m_bindings;
    size_t m_bindingCount;
};

class BasicBuiltin: public ListedBuiltin
{
  public:
    BasicBuiltin(StringTable *strings);

  private:
    static void printValue(const Value &value, const Executor::Context &context, bool escape=false);

    static void array(Executor::Context &context);
    static void size(Executor::Context &context);
    static void print(Executor::Context &context);
    static void println(Executor::Context &context);
    static void stackTrace(Executor::Context &context);

    static const ListedBuiltin::Definition defs[];
};

#endif // BUILTIN_H
