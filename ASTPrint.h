#ifndef ASTPRINT_H
#define ASTPRINT_H

#include "ASTBase.h"

namespace AST
{
  void printTokens(FILE *dest, Base *tokens);
  void printTree(FILE *dest, Base *ast, unsigned int n_indent = 0);
  void printBlock(FILE *dest, const char *name, Base *ast, unsigned int n_indent = 0);
}

#endif // ASTPRINT_H
