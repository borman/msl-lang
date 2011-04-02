#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "Executor.h"
#include "BasicBuiltin.h"
#include "ASTPrint.h"
#include "File.h"
#include "FileCharSource.h"

const char astFileName[] = "ast.lisp";
const char asmFileName[] = "asm.lst";

using namespace AST;

int main()
{
  Program program;
  StringTable strings;
  
  try
  {
    File astFile(astFileName);
    File asmFile(asmFileName);

    FileCharSource input(&cin);
    Lexer lexer(&input, &strings);
    Parser parser(&lexer);
    Compiler compiler(program);

    Fun *funs = parser.getAll();

    printBlock(&astFile, "program", funs);
    astFile.printf("\n");

    compiler.compile(funs);
    printCode(&asmFile, program, &strings);

    deleteChain(funs);
  }
  catch (const Lexer::Exception &e)
  {
    cout.printf("stdin:%u:%u: Tokenizer error: %s\n", e.row()+1, e.col()+1, e.text());
    return 1;
  }
  catch (const LexemGenerator::Exception &e)
  {
    const TextRegion &r = e.region();
    cout.printf("stdin:%u:%u-%u:%u: Lexer error: %s in \"%s\"\n",
           r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
           e.text(), e.token().c_str());
    return 1;
  }
  catch (const Parser::Exception &e)
  {
    const TextRegion &r = e.region();
    cout.printf("stdin:%u:%u-%u:%u: Parser error: %s\n", 
        r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
        e.text());
    return 1;
  }

  try
  {
    BasicBuiltin builtins(&strings);
    Executor executor(program, &strings);
    executor.addBuiltin(&builtins);
    executor.run(strings.id("main"));
  }
  catch (const Executor::Exception &e)
  {
    const char *descr = NULL;
    switch (e.type())
    {
      case Executor::Exception::BadType: descr = "Types incompatible"; break;
      case Executor::Exception::UndefVar: descr = "Undefined variable"; break;
      case Executor::Exception::UndefFun: descr = "Undefined function"; break;
      case Executor::Exception::Trap: descr = "Trap"; break;
    }
    cout.printf("Executor error: %s at %04zu\n", descr, e.addr());
    return 1;
  }

  return 0;
}


