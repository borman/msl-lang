#ifndef ARRAY_STORAGE_H
#define ARRAY_STORAGE_H

#include "Value.h"
#include "Vector.h"

class ArrayStorage 
{
  public:
    // Exceptions
    class BadSize {};
    class BadRef {};
    class BadIndex {}; 

    ArrayStorage();
    ~ArrayStorage();

    Value alloc(int size);
    void free(const Value &ref);

    void set(const Value &ref, const Value &index, const Value &val);
    Value get(const Value &ref, const Value &index) const;

  private:
    void checkRef(const Value &ref) const;
    void checkIndex(const Value &ref, const Value &index) const;

    Vector<Vector<Value> *> m_arrays;
};

#endif // ARRAY_STORAGE_H

