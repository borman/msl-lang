#include <cstdio>
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "Symbols.h"
#include "AST.h"
#include "ASTPrint.h"

const char *tokensFileName = "tokens.txt";
const char *astFileName = "ast.l";
const char *asmFileName = "asm.lst";

using namespace AST;

int main()
{
  FILE *tokensFile = fopen(tokensFileName, "w");
  if (tokensFile == NULL)
  {
    printf("Failed to open token output file \"%s\", defaulting to stderr\n", tokensFileName);
    tokensFile = stderr;
  }
  else
    printf("Saving tokens to \"%s\"\n", tokensFileName);

  FILE *astFile = fopen(astFileName, "w");
  if (astFile == NULL)
  {
    printf("Failed to open AST output file \"%s\", defaulting to stderr\n", astFileName);
    astFile = stderr;
  }
  else
    printf("Saving AST to \"%s\"\n", astFileName);

  FILE *asmFile = fopen(asmFileName, "w");
  if (asmFile == NULL)
  {
    printf("Failed to open ASM output file \"%s\", defaulting to stderr\n", asmFileName);
    asmFile = stderr;
  }
  else
    printf("Saving ASM to \"%s\"\n", asmFileName);
  
  try
  {
    StringTable strings;
    Lexer lexer(&strings);
    Parser parser;
    Compiler compiler(asmFile);

    int c;
    while ((c = getchar()) != EOF)
      lexer.feed(c);
    lexer.feed('\n');
    Base *tokens = lexer.peek();

    printTokens(tokensFile, tokens);

    parser.feed(tokens);
    Fun *funs = parser.peek();
    printf("Program parsed ok\n");

    printBlock(astFile, "program", funs);
    fprintf(astFile, "\n");

    compiler.feed(funs);
    printf("Program compiled ok\n");

    deleteChain(funs);
  }
  catch (const Lexer::Exception &e)
  {
    printf("stdin:%u:%u: Tokenizer error: %s\n", e.row()+1, e.col()+1, e.text());
  }
  catch (const LexemGenerator::Exception &e)
  {
    const TextRegion &r = e.region();
    printf("stdin:%u:%u-%u:%u: Lexer error: %s in \"%s\"\n",
           r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
           e.text(), e.token().c_str());
  }
  catch (const Parser::Exception &e)
  {
    const TextRegion &r = e.region();
    printf("stdin:%u:%u-%u:%u: Parser error: %s\n", 
        r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
        e.text());
  }

  if (tokensFile != stderr)
    fclose(tokensFile);
  if (astFile != stderr)
    fclose(astFile);
  if (asmFile != stderr)
    fclose(asmFile);

  return 0;
}


