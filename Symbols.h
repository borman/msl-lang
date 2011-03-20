#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <cassert>
#include <string>
#include "ASTLexem.h"

class Symbols
{
  public:
    class Ref
    {
      public:
        bool isValid() const { return index>=0; }
        AST::Symbol::Subtype type() const 
          { assert(index>=0); return symbolTbl[index].type; }
      private:
        Ref(int i): index(i) {}
        int index;

        friend class Symbols;
    };

    static Ref find(const std::string &str);
    static const char *name(AST::Symbol::Subtype type);

  private:
    struct Definition
    {
      const char *str;
      AST::Symbol::Subtype type;
    };

    static const Definition symbolTbl[];
    static const int symbolTblSize;

    friend class Ref;
};


#endif // SYMBOLS_H

