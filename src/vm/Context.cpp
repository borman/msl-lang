#include "Context.h"

void Context::push(const Value &v)
{
  stack.push(v);
}

Value Context::pop()
{
  Value v = stack.top();
  stack.pop();
  return v;
}

void Context::popdelete()
{
  unsigned int level = 0;
  do
  {
    Value v = stack.top();
    stack.pop();
    if (v.type() == Value::TupClose)
      level++;
    else if (v.type() == Value::TupOpen)
      level--;
  } while (level>0);
}

Value Context::pop(Value::Type type)
{
  Value v = stack.top();
  if (v.type() != type)
    throw BadType();
  stack.pop();
  return v;
}

Value Context::getVar(unsigned int name)
{
  return scope.top().getVar(name);
}

void Context::setVar(unsigned int name, const Value &val)
{
  return scope.top().setVar(name, val);
}

void Context::openScope()
{
  scope.push(Scope());
}

void Context::closeScope()
{
  scope.pop();
}

