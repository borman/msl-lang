#ifndef LEXEM_H
#define LEXEM_H

#include <cassert>
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
        None,
        // Intermediate forms
        Token,
        Symbol, 
        // Constants
        Literal,
        Real,
        Int,
        Bool,
        // Complex expression items
        Variable,
        FuncCall,
        ArrayItem,
        Tuple,
        Expression,
        Selector,
        Infix,
        // Higher-level items
        Let,
        Do,
        If,
        While,
        For,
        // Top-level item
        Fun
      };

      static const Type m_class_type = None;

      Base(Type t, const TextRegion &r)
        : m_type(t), m_region(r), m_next(NULL) {}
      virtual ~Base() {}

      Type type() const { return m_type; }
      TextRegion region() const { return m_region; }

      // Simple runtime type-checking
      template<class T>
        T *as() 
        { 
          assert(T::m_class_type == None || T::m_class_type==type()); 
          return static_cast<T *>(this); 
        }
      template<class T>
        T *next() { return m_next==NULL? NULL : m_next->as<T>(); }

      void setNext(Base *l) { m_next = l; }

    private:
      Type m_type;
      TextRegion m_region;
      Base *m_next;
  };

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

  // ===== Intermediate lexems: will be replced by parser

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

  // ===== Constants

  class Literal: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Literal;

      Literal(const std::string &s, const TextRegion &r)
        : Base(m_class_type, r), 
          m_text(s) {} 

      std::string text() const { return m_text; }
    private:
      std::string m_text;
  };

  class Int: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Int;

      Int(int v, const TextRegion &r)
        : Base(m_class_type, r), 
          m_value(v) {} 

      int value() const { return m_value; }
    private:
      int m_value;
  };

  class Real: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Real;

      Real(double v, const TextRegion &r)
        : Base(m_class_type, r), 
          m_value(v) {} 

      double value() const { return m_value; }
    private:

      double m_value;
  };

  class Bool: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Bool;

      Bool(bool v, const TextRegion &r)
        : Base(m_class_type, r), 
          m_value(v) {} 

      bool value() const { return m_value; }
    private:
      bool m_value;
  };

  // ===== Complex AST items

  class Variable: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Variable;

      Variable(const std::string &name, const TextRegion &r)
        : Base(m_class_type, r), 
          m_name(name) {} 

      std::string name() const { return m_name; }
    private:
      std::string m_name;
  };

  class FuncCall: public Base
  {
    public:
      static const Base::Type m_class_type = Base::FuncCall;

      FuncCall(const std::string &name, Base *arg, const TextRegion &r)
        : Base(m_class_type, r), 
          m_name(name), m_arg(arg) {} 

      std::string name() const { return m_name; }
      Base *arg() const { return m_arg; }

      void bind(Base *arg) { m_arg=arg; }
    private:
      std::string m_name;
      Base *m_arg;
  };

  class ArrayItem: public Base
  {
    public:
      static const Base::Type m_class_type = Base::ArrayItem;

      ArrayItem(const std::string &name, Base *arg, const TextRegion &r)
        : Base(m_class_type, r),
          m_name(name), m_arg(arg) {} 

      std::string name() const { return m_name; }
      Base *arg() const { return m_arg; }

      void bind(Base *arg) { m_arg=arg; }
    private:
      std::string m_name;
      Base *m_arg;
  };

  class Tuple: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Tuple;

      Tuple(Base *contents, const TextRegion &r)
        : Base(m_class_type, r), 
          m_contents(contents) {} 

      Base *contents() const { return m_contents; }
    private:
      Base *m_contents;
  };
  
  class Expression: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Expression;

      Expression(Base *contents, const TextRegion &r)
        : Base(m_class_type, r), 
          m_contents(contents) {} 

      Base *contents() const { return m_contents; }
    private:
      Base *m_contents;
  };

  class Selector: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Selector;

      Selector(Base *condition, Base *positive, Base *negative, const TextRegion &r)
        : Base(m_class_type, r),
          m_condition(condition), m_positive(positive), m_negative(negative) {} 

      Base *condition() const { return m_condition; }
      Base *positive() const { return m_positive; }
      Base *negative() const { return m_negative; }

      void bind(Base *condition, Base *positive, Base *negative) 
        { m_condition=condition; m_positive=positive; m_negative=negative; }
    private:
      Base *m_condition;
      Base *m_positive;
      Base *m_negative;
  };

  class Infix: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Infix;

      enum Subtype
      {
        Equals, Less, Greater,
        Plus, Minus, Mul, Div, Mod,
        And, Or
      };

      Infix(Subtype subtype, Base *left, Base *right, const TextRegion &r)
        : Base(m_class_type, r),
          m_subtype(subtype), m_left(left), m_right(right) {} 

      Subtype subtype() const { return m_subtype; }
      Base *left() const { return m_left; }
      Base *right() const { return m_right; }

      void bind(Base *left, Base *right) { m_left=left; m_right=right; }
    private:
      Subtype m_subtype;
      Base *m_left;
      Base *m_right;
  };

  // Higher-level
  class Let: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Let;

      Let(Base *lvalue, Base *rvalue, const TextRegion &r)
        : Base(m_class_type, r),
          m_lvalue(lvalue), m_rvalue(rvalue) {} 

      Base *lvalue() const { return m_lvalue; }
      Base *rvalue() const { return m_rvalue; }
    private:
      Base *m_lvalue;
      Base *m_rvalue;
  };

  class Do: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Do;

      Do(Base *expr, const TextRegion &r)
        : Base(m_class_type, r),
          m_expr(expr) {} 

      Base *expr() const { return m_expr; }
    private:
      Base *m_expr;
  };

  class If: public Base
  {
    public:
      static const Base::Type m_class_type = Base::If;

      If(Base *condition, Base *positive, Base *negative, const TextRegion &r)
        : Base(m_class_type, r),
          m_condition(condition), m_positive(positive), m_negative(negative) {} 

      Base *condition() const { return m_condition; }
      Base *positive() const { return m_positive; }
      Base *negative() const { return m_negative; }

    private:
      Base *m_condition;
      Base *m_positive;
      Base *m_negative;
  };

  class While: public Base
  {
    public:
      static const Base::Type m_class_type = Base::While;

      While(Base *condition, Base *body, const TextRegion &r)
        : Base(m_class_type, r),
          m_condition(condition), m_body(body) {} 

      Base *condition() const { return m_condition; }
      Base *body() const { return m_body; }
    private:
      Base *m_condition;
      Base *m_body;
  };
  
  class For: public Base
  {
    public:
      static const Base::Type m_class_type = Base::For;

      For(Lexem::Variable *var, Base *from, Base *to, Base *body, const TextRegion &r)
        : Base(m_class_type, r),
          m_var(var), m_from(from), m_to(to), m_body(body) {} 

      Lexem::Variable *var() const { return m_var; }
      Base *from() const { return m_from; }
      Base *to() const { return m_to; }
      Base *body() const { return m_body; }
    private:
      Lexem::Variable *m_var;
      Base *m_from;
      Base *m_to;
      Base *m_body;
  };

  class Fun: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Fun;

      Fun(const std::string &name, Base *arg, Base *body, const TextRegion &r)
        : Base(m_class_type, r),
          m_name(name), m_arg(arg), m_body(body) {} 

      std::string name() const { return m_name; }
      Base *arg() const { return m_arg; }
      Base *body() const { return m_body; }
    private:
      std::string m_name;
      Base *m_arg;
      Base *m_body;
  };
} // namespace Lexem

#endif // LEXEM_H

