#include "Value.h"

void Value::ensureType(Value::Type t) const
{
  if (type() != t)
    throw TypeMismatch();
}

double Value::toReal() const
{
  if (m_type == Int)
    return asInt();
  else if (m_type == Real)
    return asReal();
  else throw TypeMismatch();
}

/*
Value operator +(const Value &a, const Value &b) const
{
  if ((a.type() == Value::Int || a.type() == Value::Real)
      && (b.type() == Value::Int || b.type() == Value::Real))
  {
    if (a.type() == Value::Int && b.type() == Value::Int)
      return a.asInt() + b.asInt();
    else
      return (a.type() == Value::Int? double(a.asInt())   : d.asReal)
         + (b.type() == Value::Int? double(b.asInt()) : v.a.asReal());
  }
  else 
    throw Value::TypeMismatch();
}
*/

#define VAL_OPERATOR(_OP) \
Value operator _OP(const Value &a, const Value &b) \
{ \
  if ((a.type() == Value::Int || a.type() == Value::Real) \
      && (b.type() == Value::Int || b.type() == Value::Real)) \
  { \
    if (a.type() == Value::Int && b.type() == Value::Int) \
      return a.asInt() _OP b.asInt(); \
    else \
      return a.toReal() _OP b.toReal(); \
  } \
  else \
    throw Value::TypeMismatch(); \
} 

VAL_OPERATOR(+)
VAL_OPERATOR(-)
VAL_OPERATOR(*)
VAL_OPERATOR(/)
VAL_OPERATOR(>)
VAL_OPERATOR(<)

Value operator >=(const Value &a, const Value &b) 
{
  return ! ((a<b).asBool());
}

Value operator <=(const Value &a, const Value &b) 
{
  return ! ((a>b).asBool());
}

Value operator %(const Value &a, const Value &b) 
{
  if (a.type() == Value::Int && b.type() == Value::Int)
    return a.asInt() % b.asInt();
  else 
    throw Value::TypeMismatch();
}

Value operator &&(const Value &a, const Value &b) 
{
  if (a.type() == Value::Bool && b.type() == Value::Bool)
    return a.asBool() && b.asBool();
  else
    throw Value::TypeMismatch();
}

Value operator ||(const Value &a, const Value &b) 
{
  if (a.type() == Value::Bool && b.type() == Value::Bool)
    return a.asBool() || b.asBool();
  else
    throw Value::TypeMismatch();
}

Value operator ==(const Value &a, const Value &b) 
{
  if (a.type() != b.type())
    return false;
  switch (a.type())
  {
    case Value::Int: return a.asInt() == b.asInt();
    case Value::Real: return a.asReal() == b.asReal(); // FIXME: EPSILON
    case Value::Bool: return a.asBool() == b.asBool();
    case Value::String: return a.asString() == b.asString();
    default:
      throw Value::TypeMismatch();
  }
}


