#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <cassert>
#include <string>
#include "Lexem.h"

class Symbols
{
  public:
    class Ref
    {
      public:
        bool isValid() const { return index>=0; }
        Lexem::Symbol::Subtype type() const 
          { assert(index>=0); return symbolTbl[index].type; }
      private:
        Ref(int i): index(i) {}
        int index;

        friend class Symbols;
    };

    static Ref find(const std::string &str);
    static const char *name(Lexem::Symbol::Subtype type);

  private:
    struct Definition
    {
      const char *str;
      Lexem::Symbol::Subtype type;
    };

    static const Definition symbolTbl[];
    static const int symbolTblSize;

    friend class Ref;
};


#endif // SYMBOLS_H

