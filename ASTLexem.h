#ifndef ASTLEXEM_H
#define ASTLEXEM_H

#include "ASTBase.h"

namespace AST
{
  // ===== A general token, neither checked, nor recognized
  class Token: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Token;

      Token(const std::string &t, bool l, const TextRegion &r)
        : Base(m_class_type, r), 
          m_is_literal(l), m_text(t) {} 

      std::string text() const { return m_text; }
      bool isLiteral() const { return m_is_literal; }
    private:
      bool m_is_literal;
      std::string m_text;
  };

  // ===== Intermediate lexem: will be replaced by parser
  class Symbol: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Symbol;

      enum Subtype
      {
        // Keywords
        Fun, End,
        Do,
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

