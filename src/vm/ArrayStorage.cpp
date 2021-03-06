#include "ArrayStorage.h"

ArrayStorage::ArrayStorage()
{
}

ArrayStorage::~ArrayStorage()
{
  for (size_t i=0; i<m_arrays.size(); i++)
    delete m_arrays[i];
}


Value ArrayStorage::alloc(int size)
{
  if (size < 0)
    throw BadSize();

  size_t pos = m_arrays.size();
  Vector<Value> *new_val = new Vector<Value>(size);

  // Find empty space
  for (size_t i=0; i<m_arrays.size(); i++)
    if (m_arrays[i] == NULL)
      pos = i; 

  if (pos<m_arrays.size())
    m_arrays[pos] = new_val;
  else
    m_arrays.push_back(new_val);

  return Value(Value::Array, pos);
}

void ArrayStorage::free(const Value &ref)
{
  checkRef(ref);
  delete m_arrays[ref.asArray()];
  m_arrays[ref.asArray()] = NULL;
}


void ArrayStorage::set(const Value &ref, const Value &index, const Value &val)
{
  checkIndex(ref, index);
  Vector<Value> *array = m_arrays[ref.asArray()];
  (*array)[index.asInt()-1] = val;
}

Value ArrayStorage::get(const Value &ref, const Value &index) const
{
  checkIndex(ref, index);
  return (*m_arrays[ref.asArray()])[index.asInt()-1];
}

Vector<Value> *ArrayStorage::getArray(const Value &ref)
{
  checkRef(ref);
  return m_arrays[ref.asArray()];
}

const Vector<Value> *ArrayStorage::getArray(const Value &ref) const
{
  checkRef(ref);
  return m_arrays[ref.asArray()];
}

void ArrayStorage::checkRef(const Value &ref) const
{
  if (ref.type() != Value::Array
      || ref.asArray() >= m_arrays.size() 
      || m_arrays[ref.asArray()] == NULL)
    throw BadRef();
}

void ArrayStorage::checkIndex(const Value &ref, const Value &index) const
{
  checkRef(ref);
  
  if (index.type() != Value::Int 
      || index.asInt() <= 0 
      || static_cast<size_t>(index.asInt()) > m_arrays[ref.asArray()]->size())
    throw BadIndex();
}
