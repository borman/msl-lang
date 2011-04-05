#ifndef BASICBUILTIN_H
#define BASICBUILTIN_H

#include "Builtin.h"

/**
 * Implements some important builtin functions that cannot be implemented
 * in script language.
 */
class BasicBuiltin: public ListedBuiltin
{
  public:
    BasicBuiltin(StringTable *strings);

  private:
    static void printValue(const Value &value, const Context &context, bool escape=false);

    static void array(ListedBuiltin *self, Context &context);
    static void size(ListedBuiltin *self, Context &context);
    static void print(ListedBuiltin *self, Context &context);
    static void println(ListedBuiltin *self, Context &context);
    static void stackTrace(ListedBuiltin *self, Context &context);

    static const ListedBuiltin::Definition defs[];
};

#endif // BASICBUILTIN_H
