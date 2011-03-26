#include <cstdio>
#include "Executor.h"

void Executor::exec(const Instruction &instr)
{
  switch (instr.opcode)
  {
    // Push to stack
    case Instruction::PushVar:
      push(variable(instr.arg.atom));
      break;

    case Instruction::PushInt:
      push(instr.arg.intval);
      break;

    case Instruction::PushReal:
      push(instr.arg.realval);
      break;

    case Instruction::PushBool:
      push(instr.arg.boolval);
      break;

    case Instruction::PushString:
      push(instr.arg.atom);
      break;

    case Instruction::PushArrayItem:
      trap(); // STUB
      break;


      // Pop from stack
    case Instruction::PopVar:
      setVariable(instr.arg.atom, pop());
      break;

    case Instruction::PopArrayItem:
      trap(); // STUB
      break;

    case Instruction::PopDelete:
      popdelete();
      break;


      // Tuple boundaries
    case Instruction::TupOpen:
      push(Value::TupOpen);
      break;

    case Instruction::TupClose:
      push(Value::TupClose);
      break;

    case Instruction::TupUnOpen:
      pop(Value::TupOpen);
      break;

    case Instruction::TupUnClose:
      pop(Value::TupClose);
      break;


      // Operations
    case Instruction::Add:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Int)
        push(left.d.intval + right.d.intval);
      else if (left.type == right.type && left.type == Value::Real)
        push(left.d.realval + right.d.realval);
      else 
        badType();
    } break;

    case Instruction::Sub:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Int)
        push(left.d.intval - right.d.intval);
      else if (left.type == right.type && left.type == Value::Real)
        push(left.d.realval - right.d.realval);
      else 
        badType();
    } break;

    case Instruction::Mul:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Int)
        push(left.d.intval * right.d.intval);
      else if (left.type == right.type && left.type == Value::Real)
        push(left.d.realval * right.d.realval);
      else 
        badType();
    } break;

    case Instruction::Div:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Int)
        push(left.d.intval / right.d.intval);
      else if (left.type == right.type && left.type == Value::Real)
        push(left.d.realval / right.d.realval);
      else 
        badType();
    } break;

    case Instruction::Mod:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Int)
        push(left.d.intval % right.d.intval);
      else 
        badType();
    } break;

    case Instruction::And:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Bool)
        push(left.d.boolval && right.d.boolval);
      else 
        badType();
    } break;

    case Instruction::Or:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Bool)
        push(left.d.boolval || right.d.boolval);
      else 
        badType();
    } break;


      // Tests
    case Instruction::TestLess:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Int)
        push(left.d.intval < right.d.intval);
      else if (left.type == right.type && left.type == Value::Real)
        push(left.d.realval < right.d.realval);
      else 
        badType();
    } break;

    case Instruction::TestGreater:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type && left.type == Value::Int)
        push(left.d.intval > right.d.intval);
      else if (left.type == right.type && left.type == Value::Real)
        push(left.d.realval > right.d.realval);
      else 
        badType();
    } break;

    case Instruction::TestEqual:
    {
      Value right = pop();
      Value left = pop();
      if (left.type == right.type)
      {
        switch (left.type)
        {
          case Value::Int:    push(left.d.intval == right.d.intval); break;
          case Value::Real:   push(left.d.realval == right.d.realval); break; // FIXME
          case Value::Bool:   push(left.d.boolval == right.d.boolval); break;
          case Value::String: push(left.d.strval == right.d.strval); break;
          default: badType();
        }
      }
      else 
        badType();
    } break;


      // Jumps
    case Instruction::Jump:
      jump(instr.arg.addr);
      break;

    case Instruction::JumpIfNot:
      if (!pop(Value::Bool).d.boolval)
        jump(instr.arg.addr);
      break;

    case Instruction::Call:
      call(instr.arg.atom);
      break;

    case Instruction::Return:
      ret();
      break;


      // Special
    case Instruction::Trap:
      trap();
      break;
  }
}

// ======== Helpers ========

void Executor::push(const Executor::Value &v)
{
  m_valstack.push(v);
}

Executor::Value Executor::pop()
{
  Value v = m_valstack.top();
  if (v.type == Value::TupOpen || v.type == Value::TupClose)
    badType();
  m_valstack.pop();
  return v;
}

void Executor::popdelete()
{
  unsigned int level = 1;
  do
  {
    Value v = m_valstack.top();
    m_valstack.pop();
    if (v.type == Value::TupClose)
      level++;
    else if (v.type == Value::TupOpen)
      level--;
  } while (level>0);
}

Executor::Value Executor::pop(Executor::Value::Type type)
{
  Value v = m_valstack.top();
  if (v.type != type)
    badType();
  m_valstack.pop();
  return v;
}

void Executor::trap()
{
  throw Exception(Exception::Trap, m_pc);
}

void Executor::badType()
{
  throw Exception(Exception::BadType, m_pc);
}

void Executor::jump(size_t addr)
{
  m_pc = addr-1; // Will be incremented automatically
}

Executor::Value Executor::variable(unsigned int name)
{
  if (m_vars.count(name) == 0)
    throw Exception(Exception::UndefVar, m_pc);
  return m_vars[name];
}

void Executor::setVariable(unsigned int name, const Executor::Value &val)
{
  switch (val.type)
  {
    case Value::Int:    printf("%u <- %d\n", name, val.d.intval); break;
    case Value::Real:   printf("%u <- %lf\n", name, val.d.realval); break;
    case Value::Bool:   printf("%u <- %s\n", name, val.d.boolval? "TRUE" : "FALSE"); break;
    case Value::String: printf("%u <- %u\n", name, val.d.strval); break;
    default: break;
  };
  m_vars[name] = val;
}

void Executor::call(unsigned int name, bool saveRet)
{
  printf("call %u\n", name);
  for (size_t i=0; i<m_prog.entryCount(); i++)
    if (m_prog.entry(i).name.id() == name)
    {
      if (saveRet)
        m_callStack.push(m_pc);
      jump(m_prog.entry(i).addr);
      return;
    }
  throw Exception(Exception::UndefFun, m_pc);
}

void Executor::ret()
{
  if (!m_callStack.empty())
  {
    printf("ret: @%04zu -> @%04zu\n", m_pc, m_callStack.top());
    jump(m_callStack.top()+1);
    m_callStack.pop();
  }
  else
    m_stopped = true;
}
    
void Executor::run(unsigned int entryFun)
{
  push(Value::TupOpen);
  push(Value::TupClose);
  call(entryFun, false);
  m_pc++;
  while (!m_stopped)
    step();
}

void Executor::step()
{
  exec(m_prog[m_pc]);
  m_pc++;
}
