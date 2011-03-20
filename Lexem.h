#ifndef LEXEM_H
#define LEXEM_H

#include <string>

namespace Lexem
{
  struct TextRegion
  {
    TextRegion(unsigned int sr=0, unsigned int sc=0, 
        unsigned int er=0, unsigned int ec=0)
      : startRow(sr), startCol(sc), endRow(er), endCol(ec) {}
    unsigned int startRow;
    unsigned int startCol;
    unsigned int endRow;
    unsigned int endCol;
  };

  class Base
  {
    public:
      enum Type
      {
        Invalid,
        Token,
        Symbol, // for, if, then, etc...
        Identifier, // <VNAME>, <ANAME>, <FNAME>
        Literal,
        Real,
        Int,
        Bool
      };

      Base(Type t, const TextRegion &r)
        : m_type(t), m_region(r), m_next(NULL) {}
      virtual ~Base() {}

      Type type() const { return m_type; }
      TextRegion region() const { return m_region; }

      template<class T>
        T *as() { return static_cast<T *>(this); }
      template<class T>
        T *next() { return m_next->as<T>(); }

      void setNext(Base *l) { m_next = l; }

    private:
      static const Type m_class_type = Invalid;

      Type m_type;
      TextRegion m_region;
      Base *m_next;
  };

  // ===== A general token, neither checked, nor recognized

  class Token: public Base
  {
    public:
      Token(const std::string &t, bool l, const TextRegion &r)
        : Base(Base::Token, r), m_is_literal(l), m_text(t) {} 

      std::string text() const { return m_text; }
      bool isLiteral() const { return m_is_literal; }
    private:
      static const Base::Type m_class_type = Base::Token;

      bool m_is_literal;
      std::string m_text;
  };

  // ===== Token specializations

  class Symbol: public Base
  {
    public:
      enum Subtype
      {
        // Keywords
        Fun, End,
        Do,
        For, From, To,
        While,
        If, Then, Else,
        // Infix operators
        Equals, Less, Greater,
        Plus, Minus, Mul, Div, Mod,
        And, Or,
        // Delimiters
        Comma, 
        LParen, RParen,
        LBracket, RBracket
      };

      Symbol(Subtype t, const TextRegion &r)
        : Base(Base::Symbol, r), m_subtype(t) {} 

      Subtype subtype() const { return m_subtype; }
    private:
      static const Base::Type m_class_type = Base::Symbol;

      Subtype m_subtype;
  };

  class Identifier: public Base
  {
    public:
      enum Subtype
      {
        Variable,
        Array,
        Function
      };

      Identifier(Subtype t, const std::string &s, const TextRegion &r)
        : Base(Base::Identifier, r), m_subtype(t), m_text(s) {} 

      Subtype subtype() const { return m_subtype; }
      std::string text() const { return m_text; }
    private:
      static const Base::Type m_class_type = Base::Identifier;

      Subtype m_subtype;
      std::string m_text;
  };

  class Literal: public Base
  {
    public:
      Literal(const std::string &s, const TextRegion &r)
        : Base(Base::Literal, r), m_text(s) {} 

      std::string text() const { return m_text; }
    private:
      static const Base::Type m_class_type = Base::Literal;

      std::string m_text;
  };

  class Int: public Base
  {
    public:
      Int(int v, const TextRegion &r)
        : Base(Base::Int, r), m_value(v) {} 

      int value() const { return m_value; }
    private:
      static const Base::Type m_class_type = Base::Int;

      int m_value;
  };

  class Real: public Base
  {
    public:
      Real(double v, const TextRegion &r)
        : Base(Base::Real, r), m_value(v) {} 

      double value() const { return m_value; }
    private:
      static const Base::Type m_class_type = Base::Real;

      double m_value;
  };

  class Bool: public Base
  {
    public:
      Bool(bool v, const TextRegion &r)
        : Base(Base::Bool, r), m_value(v) {} 

      bool value() const { return m_value; }
    private:
      static const Base::Type m_class_type = Base::Bool;

      bool m_value;
  };
} // namespace Lexem

#endif // LEXEM_H

