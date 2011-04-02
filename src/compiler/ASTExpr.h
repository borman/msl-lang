#ifndef ASTEXPR_H
#define ASTEXPR_H

#include "ASTBase.h"
#include "Atom.h"

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
  
  template<class Value, Base::Type Tag>
  class Constant: public Expression
  {
    public:
      static const Base::Type m_class_type = Tag;

      Constant(Value v, const TextRegion &r = TextRegion())
        : Expression(m_class_type, r), 
          m_value(v) {} 

      Value value() const { return m_value; }
    private:
      Value m_value;
  };

  typedef Constant<Atom, Base::Literal> Literal;
  typedef Constant<int, Base::Int> Int;
  typedef Constant<double, Base::Real> Real;
  typedef Constant<bool, Base::Bool> Bool;

  // ===== Complex AST items

  class Variable: public Expression
  {
    public:
      static const Base::Type m_class_type = Base::Variable;

      Variable(const Atom &name, const TextRegion &r = TextRegion())
        : Expression(m_class_type, r), 
          m_name(name) {} 

      Atom name() const { return m_name; }
    private:
      Atom m_name;
  };

  template<Base::Type Tag>
  class Unary: public Expression
  {
    public:
      static const Base::Type m_class_type = Tag;

      Unary(const Atom &name, Expression *arg, const TextRegion &r = TextRegion())
        : Expression(m_class_type, r), 
          m_name(name), m_arg(arg) {} 

      Atom name() const { return m_name; }
      Expression *arg() const { return m_arg; }

      void bind(Expression *arg) { m_arg = arg; }
    private:
      Atom m_name;
      Expression *m_arg;
  };

  typedef Unary<Base::FuncCall> FuncCall;
  typedef Unary<Base::ArrayItem> ArrayItem;

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

      Infix(Subtype subtype, Expression *left, Expression *right, 
            const TextRegion &r = TextRegion())
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
