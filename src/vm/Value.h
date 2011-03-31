#ifndef VALUE_H
#define VALUE_H

#include "File.h"

class Value
{
  public:
    // Exception
    class TypeMismatch {};

    enum Type
    {
      TupOpen, TupClose,
      Int, Real, Bool, String
    };
    union Data
    {
      int intval;
      double realval;
      bool boolval;
      unsigned int strval;
    };

    Value(int i = 0):      m_type(Int)    { d.intval = i; }
    Value(double r):       m_type(Real)   { d.realval = r; }
    Value(bool b):         m_type(Bool)   { d.boolval = b; }
    Value(unsigned int s): m_type(String) { d.strval = s; }
    Value(Type t):         m_type(t)      {}

    const Data *operator ->() const { return &d; }
    Type type() const { return m_type; }

    Value operator +(const Value &v) const;
    Value operator -(const Value &v) const;
    Value operator *(const Value &v) const;
    Value operator /(const Value &v) const;
    Value operator %(const Value &v) const;
    Value operator >(const Value &v) const;
    Value operator <(const Value &v) const;
    Value operator >=(const Value &v) const;
    Value operator <=(const Value &v) const;
    Value operator ==(const Value &v) const;
    Value operator &&(const Value &v) const;
    Value operator ||(const Value &v) const;

  private:
    Type m_type;
    Data d;
};

#endif // VALUE_H
