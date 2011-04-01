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

class BasicBuiltin: public AbstractBuiltin
{
  public:
    BasicBuiltin(StringTable *strings);
    virtual bool call(unsigned int name, Executor::Context &context);

  private:
    void array(Executor::Context &context);
    void print(Executor::Context &context);
    void println(Executor::Context &context);
    void stackTrace(Executor::Context &context);

    unsigned int m_arrayId;
    unsigned int m_printId;
    unsigned int m_printlnId;
    unsigned int m_stackTraceId;
    StringTable *m_strings;
};

#endif // BUILTIN_H
