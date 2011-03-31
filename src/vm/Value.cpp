#include "Value.h"

/*
Value Value::operator +(const Value &v) const
{
  if ((m_type == Int || m_type == Real)
      && (v.m_type == Int || v.m_type == Real))
  {
    if (m_type == Int && v.m_type == Int)
      return d.intval + v.d.intval;
    else
      return (m_type == Int? double(d.intval)   : d.realval)
         + (v.m_type == Int? double(v.d.intval) : v.d.realval);
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
      return d.intval _OP v.d.intval; \
    else \
      return (m_type == Int? double(d.intval)   : d.realval) \
        _OP (v.m_type == Int? double(v.d.intval) : v.d.realval); \
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
  return ! ((*this < v)->boolval);
}

Value Value::operator <=(const Value &v) const
{
  return ! ((*this > v)->boolval);
}

Value Value::operator %(const Value &v) const
{
  if (m_type == Int && v.m_type == Int)
    return d.intval % v.d.intval;
  else 
    throw TypeMismatch();
}

Value Value::operator &&(const Value &v) const
{
  if (m_type == Bool && v.m_type == Bool)
    return d.boolval && v.d.boolval;
  else
    throw TypeMismatch();
}

Value Value::operator ||(const Value &v) const
{
  if (m_type == Bool && v.m_type == Bool)
    return d.boolval || v.d.boolval;
  else
    throw TypeMismatch();
}

Value Value::operator ==(const Value &v) const
{
  if (m_type != v.m_type)
    return false;
  switch (m_type)
  {
    case Int: return d.intval == v.d.intval;
    case Real: return d.realval == v.d.intval; // FIXME: EPSILON
    case Bool: return d.boolval == v.d.boolval;
    case String: return d.strval == v.d.strval;
    default:
      throw TypeMismatch();
  }
}

