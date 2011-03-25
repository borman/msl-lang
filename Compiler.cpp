#include <cstdio>
#include <stack>
#include "Compiler.h"

using namespace AST;

void Compiler::feed(Fun *funs)
{
  while (funs != NULL)
  {
    compileFun(funs);
    funs = funs->next<Fun>();
  }
}

void Compiler::compileFun(Fun *fun)
{
  fprintf(m_out, ";\n; FUN %s\n", fun->name().c_str());
  compilePopExpr(fun->arg());
  compileBlock(fun->body());
  fprintf(m_out, "; ENDFUN %s\n", fun->name().c_str());
}

void Compiler::compileBlock(Operator *block)
{
  while (block != NULL)
  {
    compileOperator(block);
    block = block->next<Operator>();
  }
}

void Compiler::compileOperator(Operator *op)
{
  switch (op->type())
  {
    case Base::Do: 
      compileDo(op->as<Do>()); 
      break;
    case Base::Return: 
      compileReturn(op->as<Return>()); 
      break;
    case Base::Let: 
      compileLet(op->as<Let>()); 
      break;
    case Base::If:
      compileIf(op->as<If>()); 
      break;
    case Base::While:
      compileWhile(op->as<While>()); 
      break;
    case Base::For:
      compileFor(op->as<For>()); 
      break;

    default:
      break;
  }
}

void Compiler::compileDo(Do *ast)
{
  fprintf(m_out, "; DO\n");
  compilePushExpr(ast->expr());
  fprintf(m_out, "popdelete\n");
}

void Compiler::compileReturn(Return *ast)
{
  fprintf(m_out, "; RETURN\n");
  compilePushExpr(ast->expr());
  fprintf(m_out, "ret\n");
}

void Compiler::compileLet(Let *ast)
{
  fprintf(m_out, "; LET\n");
  compilePushExpr(ast->rvalue());
  compilePopExpr(ast->lvalue());
}

void Compiler::compileIf(If *ast)
{
  const unsigned int n = m_counter++;

  fprintf(m_out, "; IF\n");
  compilePushExpr(ast->condition());
  fprintf(m_out, "jnot\t\t@else_%u\n", n);
  compileBlock(ast->positive());
  if (ast->negative() != NULL)
    fprintf(m_out, "jump\t\t@if-exit_%u\n", n);
  fprintf(m_out, "@else_%u:\n", n);
  if (ast->negative() != NULL)
  {
    compileBlock(ast->negative());
    fprintf(m_out, "@if-exit_%u:\n", n);
  }
}

void Compiler::compileWhile(While *ast)
{
  const unsigned int n = m_counter++;

  fprintf(m_out, "@loop_%u:\n", n);
  compilePushExpr(ast->condition());
  fprintf(m_out, "jnot\t\t@while-exit_%u\n", n);
  compileBlock(ast->body());
  fprintf(m_out, "@while-exit_%u:\n", n);
}

void Compiler::compileFor(For *ast)
{
  const unsigned int n = m_counter++;
  fprintf(m_out, "; STUB FOR\n");
}

void Compiler::compilePushExpr(Expression *expr)
{
  switch (expr->type())
  {
    case Base::Int:
      compilePushInt(expr->as<Int>());
      break;
    case Base::Bool:
      compilePushBool(expr->as<Bool>());
      break;
    case Base::Real:
      compilePushReal(expr->as<Real>());
      break;
    case Base::Literal:
      compilePushLiteral(expr->as<Literal>());
      break;
    case Base::Variable:
      compilePushVariable(expr->as<Variable>());
      break;
    case Base::FuncCall:
      compilePushFuncCall(expr->as<FuncCall>());
      break;
    case Base::ArrayItem:
      compilePushArrayItem(expr->as<ArrayItem>());
      break;
    case Base::Tuple:
      compilePushTuple(expr->as<Tuple>());
      break;
    case Base::Selector:
      compilePushSelector(expr->as<Selector>());
      break;
    case Base::Infix:
      compilePushInfix(expr->as<Infix>());
      break;

    default:
      break;
  }
}

void Compiler::compilePushInt(Int *expr)
{
  fprintf(m_out, "pushint\t\t%d\n", expr->value());
}

void Compiler::compilePushReal(Real *expr)
{
  fprintf(m_out, "pushreal\t%lf\n", expr->value());
}

void Compiler::compilePushBool(Bool *expr)
{
  fprintf(m_out, "pushbool\t%s\n", expr->value()? "TRUE" : "FALSE");
}

void Compiler::compilePushLiteral(Literal *expr)
{
  fprintf(m_out, "pushstring\t\"%s\"\n", expr->text().c_str());
}

void Compiler::compilePushVariable(Variable *expr)
{
  fprintf(m_out, "pushvar\t\t%s\n", expr->name().c_str());
}

void Compiler::compilePushFuncCall(FuncCall *expr)
{
  compilePushExpr(expr->arg());
  fprintf(m_out, "call\t\t%s\n", expr->name().c_str());
}

void Compiler::compilePushArrayItem(ArrayItem *expr)
{
  compilePushExpr(expr->arg());
  fprintf(m_out, "pusharritem\t%s\n", expr->name().c_str());
}

void Compiler::compilePushTuple(Tuple *expr)
{
  fprintf(m_out, "tupopen\n");
  Expression *contents = expr->contents();
  while (contents != NULL)
  {
    compilePushExpr(contents);
    contents = contents->next<Expression>();
  }
  fprintf(m_out, "tupclose\n");
}

void Compiler::compilePushSelector(Selector *expr)
{
  const unsigned int n = m_counter++;

  fprintf(m_out, "; Selector\n");
  compilePushExpr(expr->condition());
  fprintf(m_out, "jnot\t\t@else_%u\n", n);
  compilePushExpr(expr->positive());
  fprintf(m_out, "jump\t\t@if-exit_%u\n", n);
  fprintf(m_out, "@else_%u:\n", n);
  compilePushExpr(expr->positive());
  fprintf(m_out, "@if-exit_%u:\n", n);
}

static const char *infix(Infix::Subtype t)
{
  switch (t)
  {
    case Infix::Equals: return "tstequal";
    case Infix::Less: return "tstless";
    case Infix::Greater: return "tstgreater";
    case Infix::Plus: return "add";
    case Infix::Minus: return "sub";
    case Infix::Mul: return "mul";
    case Infix::Div: return "div";
    case Infix::Mod: return "mod";
    case Infix::And: return "and";
    case Infix::Or: return "or";
    default: return NULL;
  }
}

void Compiler::compilePushInfix(Infix *expr)
{
  compilePushExpr(expr->left());
  compilePushExpr(expr->right());
  fprintf(m_out, "%s\n", infix(expr->subtype()));
}

void Compiler::compilePopExpr(Expression *expr)
{
  switch (expr->type())
  {
    case Base::Variable:
      compilePopVariable(expr->as<Variable>());
      break;
    case Base::ArrayItem:
      compilePopArrayItem(expr->as<ArrayItem>());
      break;
    case Base::Tuple:
      compilePopTuple(expr->as<Tuple>());
      break;

    default:
      break;
  }
}

void Compiler::compilePopVariable(Variable *expr)
{
  fprintf(m_out, "popvar\t\t%s\n", expr->name().c_str());
}

void Compiler::compilePopArrayItem(ArrayItem *expr)
{
  compilePushExpr(expr->arg());
  fprintf(m_out, "poparritem\t%s\n", expr->name().c_str());
}

void Compiler::compilePopTuple(Tuple *expr)
{
  fprintf(m_out, "tupunclose\n");
  // Reverse contents
  std::stack<Expression *> contents;
  for (Expression *e = expr->contents(); e != NULL; e = e->next<Expression>())
    contents.push(e);
  while (!contents.empty())
  {
    compilePopExpr(contents.top());
    contents.pop();
  }
  fprintf(m_out, "tupunopen\n");
}


