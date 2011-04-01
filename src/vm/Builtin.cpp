#include "Builtin.h"
#include "File.h"


BasicBuiltin::BasicBuiltin(StringTable *strings)
  : m_strings(strings)
{
  m_arrayId = strings->id("array");
  m_printId = strings->id("print");
  m_printlnId = strings->id("println");
  m_stackTraceId = strings->id("stackTrace");
}

bool BasicBuiltin::call(unsigned int name, Executor::Context &context)
{
  if (name==m_arrayId)
    array(context);
  else if (name==m_printId)
    print(context);
  else if (name == m_printlnId)
    println(context);
  else if (name == m_stackTraceId)
    stackTrace(context);
  else
    return false;
  return true;
}


void BasicBuiltin::array(Executor::Context &context)
{
  Value size = context.pop(Value::Int);
  context.push(context.arrays.alloc(size->asInt));
}

void BasicBuiltin::print(Executor::Context &context)
{
  unsigned int level = 0;
  Stack<Value> args;
  do
  {
    Value v = context.pop();
    if (v.type() == Value::TupClose)
      level++;
    else if (v.type() == Value::TupOpen)
      level--;
    else
      args.push(v);
  } while (level>0);

  while (!args.empty())
  {
    Value v = args.top();
    args.pop();
    switch (v.type())
    {
      case Value::Int: cout.printf("%d", v->asInt); break;
      case Value::Real: cout.printf("%lf", v->asReal); break;
      case Value::Bool: cout.printf("%s", v->asBool? "true": "false"); break;
      case Value::String: cout.printf("%s", m_strings->str(v->asHandle)); break;
      default: break;
    }
    cout.printf(" ");
  }

  // Void return
  context.stack.push(Value::TupOpen);
  context.stack.push(Value::TupClose);
}

void BasicBuiltin::println(Executor::Context &context)
{
  print(context);
  cout.printf("\n");
}

void BasicBuiltin::stackTrace(Executor::Context &context)
{
  for (size_t i=0; i<context.stack.size(); i++)
  {
    const Value &v = context.stack[i];
    switch (v.type())
    {
      case Value::Int: cout.printf("%d", v->asInt); break;
      case Value::Real: cout.printf("%lf", v->asReal); break;
      case Value::Bool: cout.printf("%s", v->asBool? "true": "false"); break;
      case Value::String: cout.printf("%s", m_strings->str(v->asHandle)); break;
      case Value::TupOpen: cout.printf("["); break;
      case Value::TupClose: cout.printf("]"); break;
      default: break;
    }
    cout.printf(" ");
  }
  cout.printf("\n");
}
