#include "Builtin.h"
#include "File.h"

ListedBuiltin::ListedBuiltin(StringTable *strings, const Definition *defs, size_t count)
  : m_bindings(new Binding[count]), m_bindingCount(count)
{
  for (size_t i=0; i<count; i++)
  {
    m_bindings[i].name = strings->id(defs[i].name);
    m_bindings[i].func = defs[i].func;
  }
}

ListedBuiltin::~ListedBuiltin()
{
  delete[] m_bindings;
}

bool ListedBuiltin::call(unsigned int name, Executor::Context &context)
{
  for (size_t i=0; i<m_bindingCount; i++)
    if (name == m_bindings[i].name)
    {
      m_bindings[i].func(context);
      return true;
    }
  return false;
}

// =============== Basic

const ListedBuiltin::Definition BasicBuiltin::defs[] =
{
  {"array", BasicBuiltin::array},
  {"size", BasicBuiltin::size},
  {"print", BasicBuiltin::print},
  {"println", BasicBuiltin::println},
};

BasicBuiltin::BasicBuiltin(StringTable *strings)
  : ListedBuiltin(strings, defs, sizeof(defs)/sizeof(ListedBuiltin::Definition))
{
}

void BasicBuiltin::array(Executor::Context &context)
{
  Value size = context.pop(Value::Int);
  context.push(context.arrays.alloc(size->asInt));
}

void BasicBuiltin::size(Executor::Context &context)
{
  Value array = context.pop(Value::Array);
  context.push(static_cast<int>(context.arrays.getArray(array)->size()));
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
      case Value::Int: 
        cout.printf("%d", v->asInt); 
        break; 
      case Value::Real: 
        cout.printf("%lf", v->asReal); 
        break;
      case Value::Bool: 
        cout.printf("%s", v->asBool? "true": "false"); 
        break;
      case Value::String: 
        cout.printf("%s", context.strings->str(v->asHandle)); 
        break;
      default: 
        break;
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
      case Value::String: cout.printf("%s", context.strings->str(v->asHandle)); break;
      case Value::TupOpen: cout.printf("["); break;
      case Value::TupClose: cout.printf("]"); break;
      default: break;
    }
    cout.printf(" ");
  }
  cout.printf("\n");
}
