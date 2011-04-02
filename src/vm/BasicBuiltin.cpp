#include "BasicBuiltin.h"


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

void BasicBuiltin::array(Context &context)
{
  Value size = context.pop(Value::Int);
  context.push(context.arrays.alloc(size.asInt()));
}

void BasicBuiltin::size(Context &context)
{
  Value array = context.pop(Value::Array);
  context.push(static_cast<int>(context.arrays.getArray(array)->size()));
}

void BasicBuiltin::printValue(const Value &v, const Context &context, bool escape)
{
    switch (v.type())
    {
      case Value::Int: 
        cout.printf("%d", v.asInt()); 
        break; 
      case Value::Real: 
        cout.printf("%lf", v.asReal()); 
        break;
      case Value::Bool: 
        cout.printf("%s", v.asBool()? "true" : "false"); 
        break;
      case Value::String: 
        cout.printf(escape? "\"%s\"" : "%s", context.strings->str(v.asString())); 
        break;
      case Value::TupOpen:
        cout.printf("[");
        break;
      case Value::TupClose:
        cout.printf("]");
        break;
      case Value::Array:
      {
        const Vector<Value> &items = *(context.arrays.getArray(v));
        cout.printf("(");
        for (size_t i=0; i<items.size(); i++)
        {
          if (i>0)
            cout.printf(" ");
          printValue(items[i], context, true);
        }
        cout.printf(")");
      } break;
      default:
        break;
    }
}

void BasicBuiltin::print(Context &context)
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
    printValue(args.top(), context);
    cout.printf(" ");
    args.pop();
  }

  // Void return
  context.stack.push(Value::TupOpen);
  context.stack.push(Value::TupClose);
}

void BasicBuiltin::println(Context &context)
{
  print(context);
  cout.printf("\n");
}

void BasicBuiltin::stackTrace(Context &context)
{
  for (size_t i=0; i<context.stack.size(); i++)
  {
    printValue(context.stack[i], context);
    cout.printf(" ");
  }
  cout.printf("\n");
}
