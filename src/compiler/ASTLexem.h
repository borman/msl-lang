#ifndef ASTLEXEM_H
#define ASTLEXEM_H

#include "ASTBase.h"

namespace AST
{
  // ===== Intermediate lexem: will be replaced by parser
  class Symbol: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Symbol;

      enum Subtype
      {
        // Keywords
        Fun, Global, End,
        Do, Return,
        For, From, To,
        While,
        If, Then, Else,
        // Infix operators
        InfixBegin,
        Equals, Less, Greater,
        Plus, Minus, Mul, Div, Mod,
        And, Or,
        InfixEnd,
        // Delimiters
        Comma, 
        LParen, RParen,
        LBracket, RBracket
      };

      Symbol(Subtype t, const TextRegion &r)
        : Base(m_class_type, r),
          m_subtype(t) {} 

      Subtype subtype() const { return m_subtype; }
    private:
      Subtype m_subtype;
  };
}

#endif // ASTLEXEM_H

