#include <cstddef>
#include "Executor.h"
#include "File.h"

void Executor::exec(const Instruction &instr)
{
  if (instr.isPush())
    push(execPush(instr));
  else if (instr.isBinOp())
  {
    Value right = pop();
    Value left = pop();
    push(execBinOp(instr, left, right));
  }
  else switch (instr.opcode)
  {
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
    case Instruction::TupUnOpen:
      pop(Value::TupOpen);
      break;
    case Instruction::TupUnClose:
      pop(Value::TupClose);
      break;

      // Jumps
    case Instruction::Jump:
      jump(instr.arg.addr);
      break;
    case Instruction::JumpIfNot:
      if (!pop(Value::Bool)->boolval)
        jump(instr.arg.addr);
      break;
    case Instruction::Call:
      call(instr.arg.atom);
      break;
    case Instruction::Return:
      ret();
      break;

      // Special
    case Instruction::Trace:
      break;
    default:
    case Instruction::Trap:
      trap();
      break;
  }
}

Value Executor::execPush(const Instruction &instr)
{
  switch (instr.opcode) 
  {
    case Instruction::PushVar:       return getVariable(instr.arg.atom);
    case Instruction::PushInt:       return instr.arg.intval;
    case Instruction::PushReal:      return instr.arg.realval;
    case Instruction::PushBool:      return instr.arg.boolval;
    case Instruction::PushString:    return instr.arg.atom;
    case Instruction::PushArrayItem: trap(); // STUB
    case Instruction::Dup:           return m_valStack.top();
    case Instruction::TupOpen:       return Value::TupOpen;
    case Instruction::TupClose:      return Value::TupClose;
    default:                         trap(); return 0;
  }
}

Value Executor::execBinOp(const Instruction &instr, 
    const Value &left, const Value &right)
{
  switch (instr.opcode)
  {
    case Instruction::Add:              return left + right;
    case Instruction::Sub:              return left - right;
    case Instruction::Mul:              return left * right;
    case Instruction::Div:              return left / right;
    case Instruction::Mod:              return left % right;
    case Instruction::And:              return left && right;
    case Instruction::Or:               return left || right;
    case Instruction::TestLess:         return left < right;
    case Instruction::TestGreater:      return left > right;
    case Instruction::TestEqual:        return left == right;
    case Instruction::TestLessEqual:    return left <= right;
    case Instruction::TestGreaterEqual: return left >= right;
    default:                            trap(); return 0;
  }
}

// ======== Helpers ========

void Executor::push(const Value &v)
{
  m_valStack.push(v);
}

Value Executor::pop()
{
  Value v = m_valStack.top();
  if (v.type() == Value::TupOpen || v.type() == Value::TupClose)
    badType();
  m_valStack.pop();
  return v;
}

void Executor::popdelete()
{
  unsigned int level = 0;
  do
  {
    Value v = m_valStack.top();
    m_valStack.pop();
    if (v.type() == Value::TupClose)
      level++;
    else if (v.type() == Value::TupOpen)
      level--;
  } while (level>0);
}

Value Executor::pop(Value::Type type)
{
  Value v = m_valStack.top();
  if (v.type() != type)
    badType();
  m_valStack.pop();
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

Value Executor::getVariable(unsigned int name)
{
  return m_scope.top().getVar(name);
}

void Executor::setVariable(unsigned int name, const Value &val)
{
#if 0
  const char *s_name = m_strings->str(name);
  switch (val.type())
  {
    case Value::Int:    cerr.printf("@%04zu: %s <- %d\n", 
                            m_pc, s_name, val->intval); break;
    case Value::Real:   cerr.printf("@%04zu: %s <- %lf\n", 
                            m_pc, s_name, val->realval); break;
    case Value::Bool:   cerr.printf("@%04zu: %s <- %s\n", 
                            m_pc, s_name, val->boolval? "TRUE" : "FALSE"); break;
    case Value::String: cerr.printf("@%04zu: %s <- %u\n", 
                            m_pc, s_name, val->strval); break;
    default: break;
  };
#endif
  return m_scope.top().setVar(name, val);
}

void Executor::call(unsigned int name, bool saveRet)
{
#if 0
  cerr.printf("call %s\n", m_strings->str(name));
#endif

  // Builtin
  for (size_t i=0; i<m_builtins.size(); i++)
    if (m_builtins[i]->call(name, m_valStack))
      return;

  // Declared
  for (size_t i=0; i<m_prog.entryCount(); i++)
    if (m_prog.entry(i).name.id() == name)
    {
      // Do call
      if (saveRet)
        m_callStack.push(m_pc);
      m_scope.push(Scope()); // Open new variable scope
      jump(m_prog.entry(i).addr);
      return;
    }
  throw Exception(Exception::UndefFun, m_pc);
}

void Executor::ret()
{
  if (!m_callStack.empty())
  {
#if 0
    cerr.printf("ret: @%04zu -> @%04zu\n", m_pc, m_callStack.top());
#endif
    m_scope.pop(); // Close the variable scope
    jump(m_callStack.top()+1);
    m_callStack.pop();
  }
  else
    m_stopped = true;
}
    
void Executor::run(unsigned int entryFun)
{
  try
  {
    push(Value::TupOpen);
    push(Value::TupClose);
    call(entryFun, false);
    m_pc++;
    m_stopped = false;
    while (!m_stopped)
      step();

    // Remove function result from stack
    popdelete();
  }
  catch (Value::TypeMismatch)
  {
    throw Exception(Exception::BadType, m_pc);
  }
  catch (Scope::VarNotFound)
  {
    throw Exception(Exception::UndefVar, m_pc);
  }
}

void Executor::step()
{
  exec(m_prog[m_pc]);
  m_pc++;
}

void Executor::addBuiltin(AbstractBuiltin *b)
{
  m_builtins.push_back(b);
}

