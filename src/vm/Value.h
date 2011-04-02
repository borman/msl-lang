#ifndef VALUE_H
#define VALUE_H

#include "File.h"

/**
 * A Value is a variant-typed box with some data.
 *
 * Complex data structures like strings and arrays are stored as handles.
 */
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

    Value(int i=0):                  m_type(Int)    { d.asInt    = i; }
    Value(double r):                 m_type(Real)   { d.asReal   = r; }
    Value(bool b):                   m_type(Bool)   { d.asBool   = b; }
    Value(Type t, unsigned int h=0): m_type(t)      { d.asHandle = h; }

    Type type() const { return m_type; }

    int          asInt()    const { ensureType(Int);    return d.asInt;    }
    double       asReal()   const { ensureType(Real);   return d.asReal;   }
    bool         asBool()   const { ensureType(Bool);   return d.asBool;   }
    unsigned int asString() const { ensureType(String); return d.asHandle; }
    unsigned int asArray()  const { ensureType(Array);  return d.asHandle; }

    double toReal() const;
  private:
    void ensureType(Type t) const;
    union Data
    {
      int asInt;
      double asReal;
      bool asBool;
      unsigned int asHandle;
    };

    Type m_type;
    Data d;
};

Value operator +(const Value &a, const Value &b);
Value operator -(const Value &a, const Value &b);
Value operator *(const Value &a, const Value &b);
Value operator /(const Value &a, const Value &b);
Value operator %(const Value &a, const Value &b);
Value operator >(const Value &a, const Value &b);
Value operator <(const Value &a, const Value &b);
Value operator >=(const Value &a, const Value &b);
Value operator <=(const Value &a, const Value &b);
Value operator ==(const Value &a, const Value &b);
Value operator &&(const Value &a, const Value &b);
Value operator ||(const Value &a, const Value &b);


#endif // VALUE_H
