#ifndef AST_H
#define AST_H

#include "ASTBase.h"
#include "ASTExpr.h"
#include "ASTLexem.h"
#include "ASTOperator.h"
#include "ASTTopLevel.h"

namespace AST
{
  // Delete root and all nodes accessible from root
  void deleteChain(Base *root);

  /**
   * A simple smart pointer.
   * One-time usage supported.
   * Owns the pointer-to-chain until its value is taken.
   * To preserve ownership use the keep() method.
   *
   * Intent: to avoid memory leaks while throwing exceptions.
   */
  template<class T>
  class SafePtr
  {
    public:
      SafePtr(T *p)
        : m_pointer(p), m_own(true) {}
      ~SafePtr() 
      {
        if (m_own) 
          deleteChain(m_pointer); 
      }

      // Preserve ownership
      T *keep() 
      { 
        return m_pointer; 
      }
      // Renounce ownership
      operator T *() 
      {
        m_own = false; 
        return m_pointer; 
      }
      // Access method. Preserves ownership.
      T *operator ->() 
      { 
        return m_pointer; 
      }
      // Change the guarded pointer
      void operator =(T *p) 
      {
        if (m_own)
          deleteChain(m_pointer);
        m_pointer = p;
        m_own = true;
      }
    private:
      SafePtr(const SafePtr &sp) {}
      void operator =(const SafePtr &sp) {}

      T *m_pointer;
      bool m_own;
  };
}

#endif // AST_H

