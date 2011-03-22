#ifndef ASTEXPR_H
#define ASTEXPR_H

#include "ASTBase.h"

namespace AST
{
  // A proxy class for expression AST set 
  class Expression: public Base
  {
    public:
      Expression(Base::Type t, const TextRegion &r)
        : Base(t, r) {}
  };

  // ===== Constants

  class Literal: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Literal;

      Literal(const std::string &s, const TextRegion &r)
        : Expression(m_class_type, r), 
          m_text(s) {} 

      std::string text() const { return m_text; }
    private:
      std::string m_text;
  };

  class Int: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Int;

      Int(int v, const TextRegion &r)
        : Expression(m_class_type, r), 
          m_value(v) {} 

      int value() const { return m_value; }
    private:
      int m_value;
  };

  class Real: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Real;

      Real(double v, const TextRegion &r)
        : Expression(m_class_type, r), 
          m_value(v) {} 

      double value() const { return m_value; }
    private:
      double m_value;
  };

  class Bool: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Bool;

      Bool(bool v, const TextRegion &r)
        : Expression(m_class_type, r), 
          m_value(v) {} 

      bool value() const { return m_value; }
    private:
      bool m_value;
  };

  // ===== Complex AST items

  class Variable: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Variable;

      Variable(const std::string &name, const TextRegion &r)
        : Expression(m_class_type, r), 
          m_name(name) {} 

      std::string name() const { return m_name; }
    private:
      std::string m_name;
  };

  class FuncCall: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::FuncCall;

      FuncCall(const std::string &name, Expression *arg, const TextRegion &r)
        : Expression(m_class_type, r), 
          m_name(name), m_arg(arg) {} 

      std::string name() const { return m_name; }
      Expression *arg() const { return m_arg; }

      void bind(Expression *arg) { m_arg=arg; }
    private:
      std::string m_name;
      Expression *m_arg;
  };

  class ArrayItem: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::ArrayItem;

      ArrayItem(const std::string &name, Expression *arg, const TextRegion &r)
        : Expression(m_class_type, r),
          m_name(name), m_arg(arg) {} 

      std::string name() const { return m_name; }
      Expression *arg() const { return m_arg; }

      void bind(Expression *arg) { m_arg=arg; }
    private:
      std::string m_name;
      Expression *m_arg;
  };

  class Tuple: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Tuple;

      Tuple(Expression *contents, const TextRegion &r = TextRegion())
        : Expression(m_class_type, r), 
          m_contents(contents) {} 

      Expression *contents() const { return m_contents; }
    private:
      Expression *m_contents;
  };

  class Selector: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Selector;

      Selector(Expression *condition, Expression *positive, 
               Expression *negative, const TextRegion &r = TextRegion())
        : Expression(m_class_type, r),
          m_condition(condition), m_positive(positive), m_negative(negative) {} 

      Expression *condition() const { return m_condition; }
      Expression *positive() const { return m_positive; }
      Expression *negative() const { return m_negative; }

      void bind(Expression *condition, Expression *positive, Expression *negative) 
        { m_condition=condition; m_positive=positive; m_negative=negative; }
    private:
      Expression *m_condition;
      Expression *m_positive;
      Expression *m_negative;
  };

  class Infix: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Infix;

      enum Subtype
      {
        Equals, Less, Greater,
        Plus, Minus, Mul, Div, Mod,
        And, Or
      };

      Infix(Subtype subtype, Expression *left, Expression *right, const TextRegion &r)
        : Expression(m_class_type, r),
          m_subtype(subtype), m_left(left), m_right(right) {} 

      Subtype subtype() const { return m_subtype; }
      Expression *left() const { return m_left; }
      Expression *right() const { return m_right; }

      void bind(Expression *left, Expression *right) { m_left=left; m_right=right; }
    private:
      Subtype m_subtype;
      Expression *m_left;
      Expression *m_right;
  };
}

#endif // ASTEXPR_H
