#ifndef ASTTOPLEVEL
#define ASTTOPLEVEL

#include "ASTExpr.h"
#include "ASTOperator.h"

namespace AST
{
  // A proxy class for operator AST set 
  class TopLevel: public Base
  {
    public:
      TopLevel(Base::Type t, const TextRegion &r)
        : Base(t, r) {}
  };

  class Fun: public TopLevel
  {
    public:
      static const Base::Type m_class_type = Base::Fun;

      Fun(const Atom &name, Expression *arg, Operator *body, const TextRegion &r = TextRegion())
        : TopLevel(m_class_type, r),
          m_name(name), m_arg(arg), m_body(body) {} 

      Atom name() const { return m_name; }
      Expression *arg() const { return m_arg; }
      Operator *body() const { return m_body; }
    private:
      Atom m_name;
      Expression *m_arg;
      Operator *m_body;
  };

  class GlobalVar: public TopLevel
  {
    public:
      static const Base::Type m_class_type = Base::GlobalVar;

      GlobalVar(AST::Variable *var, const TextRegion &r = TextRegion())
        : TopLevel(m_class_type, r), m_var(var) {}

      AST::Variable *var() const { return m_var; }
    private:
      AST::Variable *m_var;
  };
}

#endif // ASTTOPLEVEL
