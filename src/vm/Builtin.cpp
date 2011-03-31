#include "Builtin.h"
#include "File.h"


BasicBuiltin::BasicBuiltin(StringTable *strings)
  : m_strings(strings)
{
  m_printId = strings->id("print");
  m_printlnId = strings->id("println");
  m_stackTraceId = strings->id("stackTrace");
}

bool BasicBuiltin::call(unsigned int name, Stack<Value> &stack)
{
  if (name==m_printId)
    print(stack);
  else if (name == m_printlnId)
    println(stack);
  else if (name == m_stackTraceId)
    stackTrace(stack);
  else
    return false;
  return true;
}


void BasicBuiltin::print(Stack<Value> &stack)
{
  unsigned int level = 0;
  Stack<Value> args;
  do
  {
    Value v = stack.top();
    stack.pop();
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
      case Value::Int: cout.printf("%d", v->intval); break;
      case Value::Real: cout.printf("%lf", v->realval); break;
      case Value::Bool: cout.printf("%s", v->boolval? "true": "false"); break;
      case Value::String: cout.printf("%s", m_strings->str(v->strval)); break;
      default: break;
    }
    cout.printf(" ");
  }

  // Void return
  stack.push(Value::TupOpen);
  stack.push(Value::TupClose);
}

void BasicBuiltin::println(Stack<Value> &stack)
{
  print(stack);
  cout.printf("\n");
}

void BasicBuiltin::stackTrace(Stack<Value> &stack)
{
  for (size_t i=0; i<stack.size(); i++)
  {
    const Value &v = stack[i];
    switch (v.type())
    {
      case Value::Int: cout.printf("%d", v->intval); break;
      case Value::Real: cout.printf("%lf", v->realval); break;
      case Value::Bool: cout.printf("%s", v->boolval? "true": "false"); break;
      case Value::String: cout.printf("%s", m_strings->str(v->strval)); break;
      case Value::TupOpen: cout.printf("["); break;
      case Value::TupClose: cout.printf("]"); break;
      default: break;
    }
    cout.printf(" ");
  }
  cout.printf("\n");
}
