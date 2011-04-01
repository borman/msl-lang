#include "Value.h"

/*
Value Value::operator +(const Value &v) const
{
  if ((m_type == Int || m_type == Real)
      && (v.m_type == Int || v.m_type == Real))
  {
    if (m_type == Int && v.m_type == Int)
      return d.asInt + v.d.asInt;
    else
      return (m_type == Int? double(d.asInt)   : d.asReal)
         + (v.m_type == Int? double(v.d.asInt) : v.d.asReal);
  }
  else 
    throw TypeMismatch();
}
*/

#define VAL_OPERATOR(_OP) \
Value Value::operator _OP(const Value &v) const \
{ \
  if ((m_type == Int || m_type == Real) \
      && (v.m_type == Int || v.m_type == Real)) \
  { \
    if (m_type == Int && v.m_type == Int) \
      return d.asInt _OP v.d.asInt; \
    else \
      return (m_type == Int? double(d.asInt)   : d.asReal) \
        _OP (v.m_type == Int? double(v.d.asInt) : v.d.asReal); \
  } \
  else \
    throw TypeMismatch(); \
} 

VAL_OPERATOR(+)
VAL_OPERATOR(-)
VAL_OPERATOR(*)
VAL_OPERATOR(/)
VAL_OPERATOR(>)
VAL_OPERATOR(<)

Value Value::operator >=(const Value &v) const
{
  return ! ((*this < v)->asBool);
}

Value Value::operator <=(const Value &v) const
{
  return ! ((*this > v)->asBool);
}

Value Value::operator %(const Value &v) const
{
  if (m_type == Int && v.m_type == Int)
    return d.asInt % v.d.asInt;
  else 
    throw TypeMismatch();
}

Value Value::operator &&(const Value &v) const
{
  if (m_type == Bool && v.m_type == Bool)
    return d.asBool && v.d.asBool;
  else
    throw TypeMismatch();
}

Value Value::operator ||(const Value &v) const
{
  if (m_type == Bool && v.m_type == Bool)
    return d.asBool || v.d.asBool;
  else
    throw TypeMismatch();
}

Value Value::operator ==(const Value &v) const
{
  if (m_type != v.m_type)
    return false;
  switch (m_type)
  {
    case Int: return d.asInt == v.d.asInt;
    case Real: return d.asReal == v.d.asInt; // FIXME: EPSILON
    case Bool: return d.asBool == v.d.asBool;
    case String: return d.asHandle == v.d.asHandle;
    default:
      throw TypeMismatch();
  }
}

