#ifndef ASTPRINT_H
#define ASTPRINT_H

#include "ASTBase.h"
#include "Program.h"
#include "File.h"

namespace AST
{
  void printTokens(File *dest, Base *tokens);
  void printTree(File *dest, Base *ast, 
      unsigned int n_indent = 0, bool do_indent = true);
  void printBlock(File *dest, const char *name, Base *ast, 
      unsigned int n_indent = 0, bool one_line = false);
  void printCode(File *dest, const Program &prog, StringTable *strings);
}

#endif // ASTPRINT_H
