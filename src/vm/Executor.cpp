#include <cstddef>
#include "Executor.h"
#include "Builtin.h"
#include "File.h"

Executor::Executor(Program &program, StringTable *strings)
  : m_prog(program), m_pc(0), m_stopped(true) 
{
  m_context.strings = strings;
  for (size_t i=0; i<program.globalsCount(); i++)
    m_context.globals.setVar(program.global(i), 0);
}

void Executor::exec(const Instruction &instr)
{
  if (instr.isPush())
    m_context.push(execPush(instr));
  else if (instr.isBinOp())
  {
    Value right = m_context.pop();
    Value left = m_context.pop();
    m_context.push(execBinOp(instr, left, right));
  }
  else switch (instr.opcode)
  {
      // Pop from stack
    case Instruction::PopVar:
      m_context.setVar(instr.arg.atom, m_context.pop());
      break;
    case Instruction::PopArrayItem:
    {
      Value index = m_context.pop(Value::Int);
      Value val = m_context.pop();
      m_context.arrays.set(m_context.getVar(instr.arg.atom), index, val);
    } break;
    case Instruction::PopDelete:
      m_context.popdelete();
      break;

      // Tuple boundaries
    case Instruction::TupUnOpen:
      m_context.pop(Value::TupOpen);
      break;
    case Instruction::TupUnClose:
      m_context.pop(Value::TupClose);
      break;

      // Jumps
    case Instruction::Jump:
      jump(instr.arg.addr);
      break;
    case Instruction::JumpIfNot:
      if (!m_context.pop(Value::Bool).asBool())
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
    case Instruction::PushVar:       return m_context.getVar(instr.arg.atom);
    case Instruction::PushInt:       return instr.arg.intval;
    case Instruction::PushReal:      return instr.arg.realval;
    case Instruction::PushBool:      return instr.arg.boolval;
    case Instruction::PushString:    return Value(Value::String, instr.arg.atom);
    case Instruction::PushArrayItem: 
                                     return m_context.arrays.get(
                                         m_context.getVar(instr.arg.atom), 
                                         m_context.pop(Value::Int));
    case Instruction::Dup:           return m_context.stack.top();
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

// ========================================

void Executor::trap()
{
  throw Exception(Exception::Trap, m_pc);
}

void Executor::jump(size_t addr)
{
  m_pc = addr-1; // Will be incremented automatically
}


void Executor::call(StringTable::Ref name, bool saveRet)
{
  // Builtin
  for (size_t i=0; i<m_builtins.size(); i++)
    if (m_builtins[i]->call(name, m_context))
      return;

  // Declared
  for (size_t i=0; i<m_prog.entryCount(); i++)
    if (m_prog.entry(i).name.id() == name)
    {
      // Do call
      if (saveRet)
        m_callStack.push(m_pc);
      m_context.openScope(); // Open new variable scope
      jump(m_prog.entry(i).addr);
      return;
    }
  throw Exception(Exception::UndefFun, m_pc);
}

void Executor::ret()
{
  if (!m_callStack.empty())
  {
    m_context.closeScope(); // Close the variable scope
    jump(m_callStack.top()+1);
    m_callStack.pop();
  }
  else
    m_stopped = true;
}
    
// =============

void Executor::run(const char *entryName)
{
  run(m_context.strings->id(entryName));
}

void Executor::run(StringTable::Ref entryFun)
{
  try
  {
    m_context.push(Value::TupOpen);
    m_context.push(Value::TupClose);
    call(entryFun, false);
    m_pc++;
    m_stopped = false;
    while (!m_stopped)
      step();

    // Remove function result from stack
    m_context.popdelete();
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

