#ifndef ASTOPERATOR_H
#define ASTOPERATOR_H

#include "ASTExpr.h"

namespace AST
{
  // A proxy class for operator AST set 
  class Operator: public Base
  {
    public:
      Operator(Base::Type t, const TextRegion &r)
        : Base(t, r) {}
  };

  class Let: public Operator
  {
    public:
      static const Base::Type m_class_type = Base::Let;

      Let(Expression *lvalue, Expression *rvalue, const TextRegion &r = TextRegion())
        : Operator(m_class_type, r),
          m_lvalue(lvalue), m_rvalue(rvalue) {} 

      Expression *lvalue() const { return m_lvalue; }
      Expression *rvalue() const { return m_rvalue; }
    private:
      Expression *m_lvalue;
      Expression *m_rvalue;
  };

  class Do: public Operator
  {
    public:
      static const Base::Type m_class_type = Base::Do;

      Do(Expression *expr, const TextRegion &r = TextRegion())
        : Operator(m_class_type, r),
          m_expr(expr) {} 

      Expression *expr() const { return m_expr; }
    private:
      Expression *m_expr;
  };

  class Return: public Operator
  {
    public:
      static const Base::Type m_class_type = Base::Return;

      Return(Expression *expr, const TextRegion &r = TextRegion())
        : Operator(m_class_type, r),
          m_expr(expr) {} 

      Expression *expr() const { return m_expr; }
    private:
      Expression *m_expr;
  };

  class If: public Operator
  {
    public:
      static const Base::Type m_class_type = Base::If;

      If(Expression *condition, Operator *positive, 
         Operator *negative, const TextRegion &r = TextRegion())
        : Operator(m_class_type, r),
          m_condition(condition), m_positive(positive), m_negative(negative) {} 

      Expression *condition() const { return m_condition; }
      Operator *positive() const { return m_positive; }
      Operator *negative() const { return m_negative; }

    private:
      Expression *m_condition;
      Operator *m_positive;
      Operator *m_negative;
  };

  class While: public Operator
  {
    public:
      static const Base::Type m_class_type = Base::While;

      While(Expression *condition, Operator *body, const TextRegion &r = TextRegion())
        : Operator(m_class_type, r),
          m_condition(condition), m_body(body) {} 

      Expression *condition() const { return m_condition; }
      Operator *body() const { return m_body; }
    private:
      Expression *m_condition;
      Operator *m_body;
  };
  
  class For: public Operator
  {
    public:
      static const Base::Type m_class_type = Base::For;

      For(AST::Variable *var, Expression *from, Expression *to,
          Operator *body, const TextRegion &r = TextRegion())
        : Operator(m_class_type, r),
          m_var(var), m_from(from), m_to(to), m_body(body) {} 

      AST::Variable *var() const { return m_var; }
      Expression *from() const { return m_from; }
      Expression *to() const { return m_to; }
      Operator *body() const { return m_body; }
    private:
      AST::Variable *m_var;
      Expression *m_from;
      Expression *m_to;
      Operator *m_body;
  };

  class Fun: public Base
  {
    public:
      static const Base::Type m_class_type = Base::Fun;

      Fun(const Atom &name, Expression *arg, Operator *body, const TextRegion &r = TextRegion())
        : Base(m_class_type, r),
          m_name(name), m_arg(arg), m_body(body) {} 

      Atom name() const { return m_name; }
      Expression *arg() const { return m_arg; }
      Operator *body() const { return m_body; }
    private:
      Atom m_name;
      Expression *m_arg;
      Operator *m_body;
  };
}

#endif // ASTOPERATOR_H
