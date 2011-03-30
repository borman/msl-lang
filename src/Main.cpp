#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "Executor.h"
#include "ASTPrint.h"
#include "File.h"

const char tokensFileName[] = "tokens.txt";
const char astFileName[] = "ast.lisp";
const char asmFileName[] = "asm.lst";

using namespace AST;

int main()
{
  Program program;
  StringTable strings;
  
  try
  {
    File tokensFile(tokensFileName);
    File astFile(astFileName);
    File asmFile(asmFileName);

    Lexer lexer(&strings);
    Parser parser;
    Compiler compiler(program);

    int c;
    while ((c = cin.getc()) != EOF)
      lexer.feed(c);
    lexer.feed('\n');
    Base *tokens = lexer.peek();

    printTokens(&tokensFile, tokens);

    parser.feed(tokens);
    Fun *funs = parser.peek();
    cout.printf("Program parsed ok\n");

    printBlock(&astFile, "program", funs);
    astFile.printf("\n");

    compiler.compile(funs);
    cout.printf("Program compiled ok\n");
    printCode(&asmFile, program, &strings);

    deleteChain(funs);
  }
  catch (const Lexer::Exception &e)
  {
    cout.printf("stdin:%u:%u: Tokenizer error: %s\n", e.row()+1, e.col()+1, e.text());
  }
  catch (const LexemGenerator::Exception &e)
  {
    const TextRegion &r = e.region();
    cout.printf("stdin:%u:%u-%u:%u: Lexer error: %s in \"%s\"\n",
           r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
           e.text(), e.token().c_str());
  }
  catch (const Parser::Exception &e)
  {
    const TextRegion &r = e.region();
    cout.printf("stdin:%u:%u-%u:%u: Parser error: %s\n", 
        r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
        e.text());
  }

  Executor executor(program, &strings);
  executor.run(strings.id("main"));

  return 0;
}


