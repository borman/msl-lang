#ifndef BUILTIN_H
#define BUILTIN_H

#include "Stack.h"
#include "Value.h"
#include "StringTable.h"

class AbstractBuiltin
{
  public:
    virtual bool call(unsigned int name, Stack<Value> &stack) = 0;
};

class BasicBuiltin: public AbstractBuiltin
{
  public:
    BasicBuiltin(StringTable *strings);
    virtual bool call(unsigned int name, Stack<Value> &stack);

  private:
    void print(Stack<Value> &stack);
    void println(Stack<Value> &stack);
    void stackTrace(Stack<Value> &stack);

    unsigned int m_printId;
    unsigned int m_printlnId;
    unsigned int m_stackTraceId;
    StringTable *m_strings;
};

#endif // BUILTIN_H
