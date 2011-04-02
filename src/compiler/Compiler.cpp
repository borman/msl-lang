#include <cstdio>
#include "Stack.h"
#include "Compiler.h"

using namespace AST;

void Compiler::compile(Fun *funs)
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
  compilePop(fun->arg());

  compileBlock(fun->body());

  // "noreturn" exit
  // FIXME: Do not generate if return guaranteed 
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
  emit(Instruction::Trace, op); // Trace

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
  compilePush(ast->expr());
  // Discard expression result
  emit(Instruction::PopDelete);
}

void Compiler::compileReturn(Return *ast)
{
  compilePush(ast->expr());
  emit(Instruction::Return);
}

void Compiler::compileLet(Let *ast)
{
  compilePush(ast->rvalue());
  compilePop(ast->lvalue());
}

void Compiler::compileIf(If *ast)
{
  compilePush(ast->condition());
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
  compilePush(ast->condition());
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
  // Prologue
  compilePush(ast->from());
  compilePop(ast->var());

  // Test
  size_t l_loop = m_prog.nextAddr();
  compilePush(ast->to());
  compilePush(ast->var());
  emit(Instruction::TestGreaterEqual);
  size_t j_exit = emit(Instruction::JumpIfNot); // jump @exit

  // Body
  compileBlock(ast->body());
  compilePush(ast->var());
  emit(Instruction::PushInt, 1);
  emit(Instruction::Add);
  compilePop(ast->var());
  emit(Instruction::Jump, l_loop);

  //@exit:
  m_prog[j_exit].arg.addr = m_prog.nextAddr();
}

void Compiler::compilePush(Expression *expr)
{
  switch (expr->type())
  {
#define EXPR(type) case Base::type: compilePush(expr->as<type>()); break
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

void Compiler::compilePush(Int *expr)
{
  emit(Instruction::PushInt, expr->value());
}

void Compiler::compilePush(Real *expr)
{
  emit(Instruction::PushReal, expr->value());
}

void Compiler::compilePush(Bool *expr)
{
  emit(Instruction::PushBool, expr->value());
}

void Compiler::compilePush(Literal *expr)
{
  emit(Instruction::PushString, expr->value());
}

void Compiler::compilePush(Variable *expr)
{
  emit(Instruction::PushVar, expr->name());
}

// ============

void Compiler::compilePush(FuncCall *expr)
{
  compilePush(expr->arg());
  emit(Instruction::Call, expr->name());
}

void Compiler::compilePush(ArrayItem *expr)
{
  compilePush(expr->arg());
  emit(Instruction::PushArrayItem, expr->name());
}

void Compiler::compilePush(Tuple *expr)
{
  emit(Instruction::TupOpen);
  Expression *contents = expr->contents();
  while (contents != NULL)
  {
    compilePush(contents);
    contents = contents->next<Expression>();
  }
  emit(Instruction::TupClose);
}

void Compiler::compilePush(Selector *expr)
{
  compilePush(expr->condition());

  // JNOT @ELSE
  size_t j_else = emit(Instruction::JumpIfNot);

  // then ...
  compilePush(expr->positive());
  // JUMP @EXIT
  size_t j_exit = emit(Instruction::Jump);

  // @ELSE:
  m_prog[j_else].arg.addr = m_prog.nextAddr();
  // else ...
  compilePush(expr->negative());

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

void Compiler::compilePush(Infix *expr)
{
  compilePush(expr->left());
  compilePush(expr->right());
  emit(infixInstr(expr->subtype()));
}

void Compiler::compilePop(Expression *expr)
{
  switch (expr->type())
  {
    case Base::Variable:
      compilePop(expr->as<Variable>());
      break;
    case Base::ArrayItem:
      compilePop(expr->as<ArrayItem>());
      break;
    case Base::Tuple:
      compilePop(expr->as<Tuple>());
      break;

    default:
      break;
  }
}

void Compiler::compilePop(Variable *expr)
{
  emit(Instruction::PopVar, expr->name());
}

void Compiler::compilePop(ArrayItem *expr)
{
  compilePush(expr->arg());
  emit(Instruction::PopArrayItem, expr->name());
}

void Compiler::compilePop(Tuple *expr)
{
  emit(Instruction::TupUnClose);
  // Reverse contents
  Stack<Expression *> contents;
  for (Expression *e = expr->contents(); e != NULL; e = e->next<Expression>())
    contents.push(e);
  while (!contents.empty())
  {
    compilePop(contents.top());
    contents.pop();
  }
  emit(Instruction::TupUnOpen);
}


