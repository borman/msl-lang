#ifndef AST_H
#define AST_H

#include "ASTBase.h"
#include "ASTExpr.h"
#include "ASTLexem.h"
#include "ASTOperator.h"

namespace AST
{
  // Delete root and all nodes accessible from root
  void deleteChain(Base *root);
}

#endif // AST_H

