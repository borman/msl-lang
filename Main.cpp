#include <cstdio>
#include "Tokenizer.h"
#include "Lexer.h"
#include "Parser.h"
#include "Symbols.h"
#include "AST.h"
#include "ASTPrint.h"

const char *tokensFileName = "tokens.txt";
const char *astFileName = "ast.l";

using namespace AST;

int main()
{
  Tokenizer tokenizer;
  Lexer lexer;
  Parser parser;

  FILE *tokensFile = fopen(tokensFileName, "w");
  if (tokensFile == NULL)
  {
    printf("Failed to open tokens file \"%s\", defaulting to stderr\n", tokensFileName);
    tokensFile = stderr;
  }
  else
    printf("Saving tokens to \"%s\"\n", tokensFileName);

  FILE *astFile = fopen(astFileName, "w");
  if (astFile == NULL)
  {
    printf("Failed to open AST file \"%s\", defaulting to stderr\n", astFileName);
    astFile = stderr;
  }
  else
    printf("Saving AST to \"%s\"\n", astFileName);
  
  try
  {
    int c;
    while ((c = getchar()) != EOF)
      tokenizer.feed(c);
    tokenizer.feed('\n');
    lexer.feed(tokenizer.peek());
    Base *tokens = lexer.peek();

    printTokens(tokensFile, tokens);

    parser.feed(tokens);
    Fun *funs = parser.peek();
    printf("Program parsed ok\n");

    printBlock(astFile, "program", funs);
    fprintf(astFile, "\n");

    deleteChain(funs);
  }
  catch (const Tokenizer::Exception &e)
  {
    printf("stdin:%u:%u: Tokenizer error: %s\n", e.row()+1, e.col()+1, e.text().c_str());
  }
  catch (const Lexer::Exception &e)
  {
    const TextRegion &r = e.region();
    printf("stdin:%u:%u-%u:%u: Lexer error: %s in \"%s\"\n",
           r.startRow, r.startCol, r.endRow, r.endCol,
           e.text().c_str(), e.token().c_str());
  }
  catch (const Parser::Exception &e)
  {
    const TextRegion &r = e.region();
    printf("stdin:%u:%u-%u:%u: Parser error: %s\n", 
        r.startRow, r.startCol, r.endRow, r.endCol,
        e.text().c_str());
    printTokens(stderr, e.tokens());
    deleteChain(e.tokens());
  }

  if (tokensFile != stderr)
    fclose(tokensFile);
  if (astFile != stderr)
    fclose(astFile);

  return 0;
}


