#ifndef CONTEXT_H
#define CONTEXT_H

#include "Stack.h"
#include "Value.h"
#include "ArrayStorage.h"
#include "StringTable.h"
#include "Scope.h"


/**
 * A Context is a high-level part of the Executor's state.
 *
 * A lower-level part includes program counter, call stack, etc. 
 * Generally, you don't want to mess with it.
 */
struct Context
{
  class BadType {};

  // Stack operations
  void push(const Value &v);
  Value pop();
  Value pop(Value::Type t);
  void popdelete();

  // Variable referencing
  Value getVar(unsigned int name);
  void setVar(unsigned int name, const Value &val);

  // Push/pop context on function entry/return
  void openScope();
  void closeScope();

  Stack<Value> stack;
  Scope globals;
  Stack<Scope> locals;
  ArrayStorage arrays;
  StringTable *strings;
};

#endif // CONTEXT_H
