#ifndef ASTBASE_H
#define ASTBASE_H

#include <cassert>
#include <string>
#include "TextRegion.h"

namespace AST
{
  class Base
  {
    public:
      enum Type
      {
        None,
        // Intermediate forms
        Token,
        Symbol, 
        // Constants
        Literal,
        Real,
        Int,
        Bool,
        // Complex expression items
        Variable,
        FuncCall,
        ArrayItem,
        Tuple,
        Expression,
        Selector,
        Infix,
        // Higher-level items
        Let,
        Do,
        If,
        While,
        For,
        // Top-level item
        Fun
      };

      static const Type m_class_type = None;

      Base(Type t, const TextRegion &r)
        : m_type(t), m_region(r), m_next(NULL) {}
      virtual ~Base() {}

      Type type() const { return m_type; }
      TextRegion region() const { return m_region; }

      // Simple runtime type-checking
      template<class T>
        T *as() 
        { 
          assert(T::m_class_type == None || T::m_class_type==type()); 
          return static_cast<T *>(this); 
        }
      template<class T>
        T *next() { return m_next==NULL? NULL : m_next->as<T>(); }

      void setNext(Base *l) { m_next = l; }

    private:
      Type m_type;
      TextRegion m_region;
      Base *m_next;
  };
} // namespace AST

#endif // ASTBASE_H

