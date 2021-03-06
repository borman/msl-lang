#include <cstdio>
#include <cstdarg>
#include "LoadedProgram.h"
#include "FileCharSource.h"
#include "File.h"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "Symbols.h"
#include "ASTPrint.h"

LoadedProgram::LoadedProgram(DataSource<int> &src)
{
  load(src);
}

LoadedProgram::LoadedProgram(const char *filename)
{
  File file(filename, File::Read);
  FileCharSource fileSrc(&file);
  load(fileSrc);
}

void LoadedProgram::load(DataSource<int> &src)
{
  try
  {
    Lexer lexer(&src, &m_strings);
    Parser parser(&lexer);
    Compiler compiler(*this);

    // (Read -> Tokenize -> Lex -> Parse) chain
    AST::TopLevel *ast;
    while ((ast = parser.getNext()) != NULL)
    {
#if DEBUG_OUTPUT
      AST::printTree(&cerr, ast);
      cerr.printf("\n");
#endif
      if (ast->type() == AST::Base::Fun)
        compiler.compile(ast->as<AST::Fun>());
      else if (ast->type() == AST::Base::GlobalVar)
        addGlobal(ast->as<AST::GlobalVar>()->var()->name().id());
      deleteChain(ast);
    }
#ifdef DEBUG_OUTPUT
    cerr.printf("\n");
    AST::printCode(&cerr, *this, &m_strings);
#endif
  }
  catch (const Lexer::Exception &e)
  {
    error("%u:%u: Tokenizer error: %s", e.row()+1, e.col()+1, e.text());
  }
  catch (const LexemGenerator::Exception &e)
  {
    const TextRegion &r = e.region();
    error("%u:%u-%u:%u: Lexer error: %s in \"%s\"",
           r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
           e.text(), e.token().c_str());
  }
  catch (const Parser::SymbolExpected &e)
  {
    const TextRegion &r = e.region();
    error("%u:%u-%u:%u: Parser error: '%s' expected", 
        r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
        Symbols::name(e.symbol()));
  }
  catch (const Parser::Exception &e)
  {
    const TextRegion &r = e.region();
    error("%u:%u-%u:%u: Parser error: %s expected", 
        r.startRow+1, r.startCol+1, r.endRow+1, r.endCol+1,
        e.text());
  }
  catch (...)
  {
    error("LoadedProgram: unknown exception");
  }
}
    
void LoadedProgram::error(const char *format, ...)
{
  static const size_t errBufSize = 100;

  va_list args;
  va_start(args, format);
  char buf[errBufSize];
  vsnprintf(buf, errBufSize, format, args);
  va_end(args);

  throw Exception(buf);
}
