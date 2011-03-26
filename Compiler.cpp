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
  // Define an entry point
  m_prog.addEntry(Program::EntryPoint(fun->name(), m_prog.nextAddr()));

  // Load & bind argument
  compilePopExpr(fun->arg());

  compileBlock(fun->body());

  // "noreturn" exit
  // TODO: Do not generate if return guaranteed 
  emit(Instruction::TupOpen);
  emit(Instruction::TupClose);
  emit(Instruction::Return);
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
#define OP(type) case Base::type: compile##type(op->as<type>()); break
    OP(Do);
    OP(Return);
    OP(Let);
    OP(If);
    OP(While);
    OP(For);
#undef OP
    default:
      break;
  }
}

void Compiler::compileDo(Do *ast)
{
  compilePushExpr(ast->expr());
  // Discard expression result
  emit(Instruction::PopDelete);
}

void Compiler::compileReturn(Return *ast)
{
  compilePushExpr(ast->expr());
  emit(Instruction::Return);
}

void Compiler::compileLet(Let *ast)
{
  compilePushExpr(ast->rvalue());
  compilePopExpr(ast->lvalue());
}

void Compiler::compileIf(If *ast)
{
  compilePushExpr(ast->condition());
  // JNOT @ELSE
  size_t j_else = emit(Instruction::JumpIfNot);
  compileBlock(ast->positive());
  size_t j_exit = 0;
  if (ast->negative() != NULL)
  {
    // JUMP @EXIT
    j_exit = emit(Instruction::Jump);
  }
  // @ELSE:
  m_prog[j_else].arg.addr = m_prog.nextAddr();
  if (ast->negative() != NULL)
  {
    compileBlock(ast->negative());
    // @EXIT:
    m_prog[j_exit].arg.addr = m_prog.nextAddr();
  }
}

void Compiler::compileWhile(While *ast)
{
  // @LOOP:
  size_t l_loop = m_prog.nextAddr();
  compilePushExpr(ast->condition());
  // JNOT @EXIT
  size_t j_exit = emit(Instruction::JumpIfNot);
  compileBlock(ast->body());
  // JUMP @LOOP
  emit(Instruction::Jump, l_loop);
  // @EXIT:
  m_prog[j_exit].arg.addr = m_prog.nextAddr();
}

void Compiler::compileFor(For *ast)
{
  // Not to be implemented: should be converted to 'while' at AST analysis
  emit(Instruction::Trap);
}

void Compiler::compilePushExpr(Expression *expr)
{
  switch (expr->type())
  {
#define EXPR(type) case Base::type: compilePush##type(expr->as<type>()); break
    EXPR(Int);
    EXPR(Bool);
    EXPR(Real);
    EXPR(Literal);
    EXPR(Variable);
    EXPR(FuncCall);
    EXPR(ArrayItem);
    EXPR(Tuple);
    EXPR(Selector);
    EXPR(Infix);
#undef EXPR

    default: 
    break;
  }
}

// ============ Trivial typed constants

void Compiler::compilePushInt(Int *expr)
{
  emit(Instruction::PushInt, expr->value());
}

void Compiler::compilePushReal(Real *expr)
{
  emit(Instruction::PushReal, expr->value());
}

void Compiler::compilePushBool(Bool *expr)
{
  emit(Instruction::PushBool, expr->value());
}

void Compiler::compilePushLiteral(Literal *expr)
{
  emit(Instruction::PushLiteral, expr->text());
}

void Compiler::compilePushVariable(Variable *expr)
{
  emit(Instruction::PushVar, expr->name());
}

// ============

void Compiler::compilePushFuncCall(FuncCall *expr)
{
  compilePushExpr(expr->arg());
  emit(Instruction::Call, expr->name());
}

void Compiler::compilePushArrayItem(ArrayItem *expr)
{
  compilePushExpr(expr->arg());
  emit(Instruction::PushArrayItem, expr->name());
}

void Compiler::compilePushTuple(Tuple *expr)
{
  emit(Instruction::TupOpen);
  Expression *contents = expr->contents();
  while (contents != NULL)
  {
    compilePushExpr(contents);
    contents = contents->next<Expression>();
  }
  emit(Instruction::TupClose);
}

void Compiler::compilePushSelector(Selector *expr)
{
  compilePushExpr(expr->condition());

  // JNOT @ELSE
  size_t j_else = emit(Instruction::JumpIfNot);

  // then ...
  compilePushExpr(expr->positive());
  // JUMP @EXIT
  size_t j_exit = emit(Instruction::Jump);

  // @ELSE:
  m_prog[j_else].arg.addr = m_prog.nextAddr();
  // else ...
  compilePushExpr(expr->negative());

  // @EXIT:
  m_prog[j_exit].arg.addr = m_prog.nextAddr();
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
  return Instruction::Trap;
}

void Compiler::compilePushInfix(Infix *expr)
{
  compilePushExpr(expr->left());
  compilePushExpr(expr->right());
  emit(infixInstr(expr->subtype()));
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
  emit(Instruction::PopVar, expr->name());
}

void Compiler::compilePopArrayItem(ArrayItem *expr)
{
  compilePushExpr(expr->arg());
  emit(Instruction::PopArrayItem, expr->name());
}

void Compiler::compilePopTuple(Tuple *expr)
{
  emit(Instruction::TupUnClose);
  // Reverse contents
  std::stack<Expression *> contents;
  for (Expression *e = expr->contents(); e != NULL; e = e->next<Expression>())
    contents.push(e);
  while (!contents.empty())
  {
    compilePopExpr(contents.top());
    contents.pop();
  }
  emit(Instruction::TupUnOpen);
}


