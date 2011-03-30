#ifndef ASTPRINT_H
#define ASTPRINT_H

#include "ASTBase.h"
#include "Program.h"

namespace AST
{
  void printTokens(FILE *dest, Base *tokens);
  void printTree(FILE *dest, Base *ast, 
      unsigned int n_indent = 0, bool do_indent = true);
  void printBlock(FILE *dest, const char *name, Base *ast, 
      unsigned int n_indent = 0, bool one_line = false);
  void printCode(FILE *dest, const Program &prog, StringTable *strings);
}

#endif // ASTPRINT_H
