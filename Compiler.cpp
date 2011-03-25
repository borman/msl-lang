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

  // "noreturn" exit
  emit(Instruction(Instruction::TupOpen));
  emit(Instruction(Instruction::TupClose));
  emit(Instruction(Instruction::Return));
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
  emit(Instruction(Instruction::PopDelete));
}

void Compiler::compileReturn(Return *ast)
{
  fprintf(m_out, "; RETURN\n");
  compilePushExpr(ast->expr());
  fprintf(m_out, "ret\n");
  emit(Instruction(Instruction::Return));
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
  size_t j_else = emit(Instruction(Instruction::JumpIfNot));
  compileBlock(ast->positive());
  size_t j_exit = 0;
  if (ast->negative() != NULL)
  {
    fprintf(m_out, "jump\t\t@if-exit_%u\n", n);
    j_exit = emit(Instruction(Instruction::Jump));
  }
  fprintf(m_out, "@else_%u:\n", n);
  m_prog[j_else].arg.addr = m_prog.nextAddr();
  if (ast->negative() != NULL)
  {
    compileBlock(ast->negative());
    fprintf(m_out, "@if-exit_%u:\n", n);
    m_prog[j_exit].arg.addr = m_prog.nextAddr();
  }
}

void Compiler::compileWhile(While *ast)
{
  const unsigned int n = m_counter++;

  fprintf(m_out, "@loop_%u:\n", n);
  size_t l_loop = m_prog.nextAddr();
  compilePushExpr(ast->condition());
  fprintf(m_out, "jnot\t\t@while-exit_%u\n", n);
  size_t j_exit = emit(Instruction(Instruction::JumpIfNot));
  compileBlock(ast->body());
  fprintf(m_out, "jump\t\t@loop_%u\n", n);
  emit(Instruction(Instruction::Jump, l_loop));
  fprintf(m_out, "@while-exit_%u:\n", n);
  m_prog[j_exit].arg.addr = m_prog.nextAddr();
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
  emit(Instruction(Instruction::PushInt, expr->value()));
}

void Compiler::compilePushReal(Real *expr)
{
  fprintf(m_out, "pushreal\t%lf\n", expr->value());
  emit(Instruction(Instruction::PushReal, expr->value()));
}

void Compiler::compilePushBool(Bool *expr)
{
  fprintf(m_out, "pushbool\t%s\n", expr->value()? "TRUE" : "FALSE");
  emit(Instruction(Instruction::PushBool, expr->value()));
}

void Compiler::compilePushLiteral(Literal *expr)
{
  fprintf(m_out, "pushstring\t\"%s\"\n", expr->text().c_str());
  emit(Instruction(Instruction::PushLiteral, expr->text()));
}

void Compiler::compilePushVariable(Variable *expr)
{
  fprintf(m_out, "pushvar\t\t%s\n", expr->name().c_str());
  emit(Instruction(Instruction::PushVar, expr->name()));
}

void Compiler::compilePushFuncCall(FuncCall *expr)
{
  compilePushExpr(expr->arg());
  fprintf(m_out, "call\t\t%s\n", expr->name().c_str());
  emit(Instruction(Instruction::Call, expr->name()));
}

void Compiler::compilePushArrayItem(ArrayItem *expr)
{
  compilePushExpr(expr->arg());
  fprintf(m_out, "pusharritem\t%s\n", expr->name().c_str());
  emit(Instruction(Instruction::PushArrayItem, expr->name()));
}

void Compiler::compilePushTuple(Tuple *expr)
{
  fprintf(m_out, "tupopen\n");
  emit(Instruction(Instruction::TupOpen));
  Expression *contents = expr->contents();
  while (contents != NULL)
  {
    compilePushExpr(contents);
    contents = contents->next<Expression>();
  }
  fprintf(m_out, "tupclose\n");
  emit(Instruction(Instruction::TupOpen));
}

void Compiler::compilePushSelector(Selector *expr)
{
  const unsigned int n = m_counter++;

  fprintf(m_out, "; Selector\n");
  compilePushExpr(expr->condition());
  fprintf(m_out, "jnot\t\t@else_%u\n", n);
  size_t j_else = emit(Instruction(Instruction::JumpIfNot));
  compilePushExpr(expr->positive());
  fprintf(m_out, "jump\t\t@if-exit_%u\n", n);
  size_t j_exit = emit(Instruction(Instruction::Jump));
  fprintf(m_out, "@else_%u:\n", n);
  m_prog[j_else].arg.addr = m_prog.nextAddr();
  compilePushExpr(expr->negative());
  fprintf(m_out, "@if-exit_%u:\n", n);
  m_prog[j_exit].arg.addr = m_prog.nextAddr();
}

static const char *infixStr(Infix::Subtype t)
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

static Instruction::Opcode infixInstr(Infix::Subtype t)
{
  switch (t)
  {
    case Infix::Equals: return Instruction::TestEqual;
    case Infix::Less: return Instruction::TestLess;
    case Infix::Greater: return Instruction::TestGreater;
    case Infix::Plus: return Instruction::Add;
    case Infix::Minus: return Instruction::Sub;
    case Infix::Mul: return Instruction::Mul;
    case Infix::Div: return Instruction::Div;
    case Infix::Mod: return Instruction::Mod;
    case Infix::And: return Instruction::And;
    case Infix::Or: return Instruction::Or;
  }
}

void Compiler::compilePushInfix(Infix *expr)
{
  compilePushExpr(expr->left());
  compilePushExpr(expr->right());
  fprintf(m_out, "%s\n", infixStr(expr->subtype()));
  emit(Instruction(infixInstr(expr->subtype())));
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
  emit(Instruction(Instruction::PopVar, expr->name()));
}

void Compiler::compilePopArrayItem(ArrayItem *expr)
{
  compilePushExpr(expr->arg());
  fprintf(m_out, "poparritem\t%s\n", expr->name().c_str());
  emit(Instruction(Instruction::PopArrayItem, expr->name()));
}

void Compiler::compilePopTuple(Tuple *expr)
{
  fprintf(m_out, "tupunclose\n");
  emit(Instruction(Instruction::TupUnClose));
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
  emit(Instruction(Instruction::TupUnOpen));
}


