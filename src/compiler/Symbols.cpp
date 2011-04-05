#include <cstring>
#include "Symbols.h"

using namespace AST;

const Symbols::Definition Symbols::symbolTbl[] =
{
  {"fun",   Symbol::Fun     },
  {"global",Symbol::Global  },
  {"end",   Symbol::End     },
  {"do",    Symbol::Do      },
  {"return",Symbol::Return  },
  {"for",   Symbol::For     },
  {"from",  Symbol::From    },
  {"to",    Symbol::To      },
  {"while", Symbol::While   },
  {"if",    Symbol::If      },
  {"then",  Symbol::Then    },
  {"else",  Symbol::Else    },
  {"=",     Symbol::Equals  },
  {"<",     Symbol::Less    },
  {">",     Symbol::Greater },
  {"+",     Symbol::Plus    },
  {"-",     Symbol::Minus   },
  {"*",     Symbol::Mul     },
  {"/",     Symbol::Div     },
  {"%",     Symbol::Mod     },
  {"and",   Symbol::And     },
  {"or",    Symbol::Or      },
  {",",     Symbol::Comma   },
  {"(",     Symbol::LParen  },
  {")",     Symbol::RParen  },
  {"[",     Symbol::LBracket},
  {"]",     Symbol::RBracket}
};

const int Symbols::symbolTblSize = sizeof(Symbols::symbolTbl)/sizeof(Symbols::Definition);

Symbols::Ref Symbols::find(const char *str)
{
  for (int i=0; i<symbolTblSize; i++)
    if (0 == strcmp(str, symbolTbl[i].str))
      return Ref(i);
  return Ref(-1);
}

const char *Symbols::name(Symbol::Subtype type)
{
  for (int i=0; i<symbolTblSize; i++)
    if (type == symbolTbl[i].type)
      return symbolTbl[i].str;
  return NULL;
}
