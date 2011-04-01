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
      Int, Real, Bool, String, Array
    };
    union Data
    {
      int asInt;
      double asReal;
      bool asBool;
      unsigned int asHandle;
    };

    Value(int i=0):                  m_type(Int)    { d.asInt = i; }
    Value(double r):                 m_type(Real)   { d.asReal = r; }
    Value(bool b):                   m_type(Bool)   { d.asBool = b; }
    Value(Type t, unsigned int h=0): m_type(t)      { d.asHandle = h; }

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
