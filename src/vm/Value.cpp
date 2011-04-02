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


// (int*int) -> int
// (real*real) | (int*real) | (real*int) -> real
#define VAL_NUM_OPERATOR(_OP) \
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

// (a*a) -> a
#define VAL_OPERATOR(_TYPE, _OP) \
Value operator _OP(const Value &a, const Value &b) \
{ \
  if (a.type() == Value::_TYPE && b.type() == Value::_TYPE) \
    return a.as##_TYPE() _OP b.as##_TYPE(); \
  else  \
    throw Value::TypeMismatch(); \
}

VAL_NUM_OPERATOR(+)
VAL_NUM_OPERATOR(-)
VAL_NUM_OPERATOR(*)
VAL_NUM_OPERATOR(/)
VAL_NUM_OPERATOR(>)
VAL_NUM_OPERATOR(<)

VAL_OPERATOR(Int, %)
VAL_OPERATOR(Bool, &&)
VAL_OPERATOR(Bool, ||)

Value operator >=(const Value &a, const Value &b) 
{
  return ! ((a<b).asBool());
}

Value operator <=(const Value &a, const Value &b) 
{
  return ! ((a>b).asBool());
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


