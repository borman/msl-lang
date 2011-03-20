#include <cstdio>
#include "Tokenizer.h"
#include "Lexer.h"
#include "Symbols.h"

using namespace Lexem;

int main()
{
  Tokenizer tokenizer;
  Lexer lexer;
  
  try
  {
    int c;
    while ((c = getchar()) != EOF)
      tokenizer.feed(c);
    tokenizer.feed('\n');
    lexer.feed(tokenizer.peek());
  }
  catch (const Tokenizer::Exception &e)
  {
    printf("stdin:%u:%u: Tokenizer error: %s\n", e.row()+1, e.col()+1, e.text().c_str());
    return 1;
  }
  catch (const Lexer::Exception &e)
  {
    const TextRegion &r = e.region();
    printf("stdin:%u:%u-%u-%u: Lexer error: %s in \"%s\"\n",
           r.startRow, r.startCol, r.endRow, r.endCol,
           e.text().c_str(), e.token().c_str());
    return 1;
  }

  Base *tokens = lexer.peek();
  for(Base *t = tokens; t != NULL; t = t->next<Base>())
  {
    const TextRegion &r = t->region();
    printf("<%u:%u - %u:%u>\t", 
           r.startRow, r.startCol,
           r.endRow, r.endCol);
    switch (t->type())
    {
      case Base::Symbol:
        printf("SYM  %s", Symbols::name(t->as<Symbol>()->subtype()));
        break;

      case Base::Bool:
        printf("BOOL %s", t->as<Bool>()->value()? "TRUE": "FALSE");
        break;

      case Base::Int:
        printf("INT  %d", t->as<Int>()->value());
        break;

      case Base::Real:
        printf("REAL %lf", t->as<Real>()->value());
        break;

      case Base::Literal:
        printf("LIT  %s", t->as<Literal>()->text().c_str());
        break;

      case Base::FuncCall:
        printf("FUN  %s", t->as<FuncCall>()->name().c_str());
        break;
      
      case Base::Variable:
        printf("VAR  %s", t->as<Variable>()->name().c_str());
        break;

      case Base::ArrayItem:
        printf("ARR  %s", t->as<ArrayItem>()->name().c_str());
        break;

      default:
        printf("UNK");
    }
    printf("\n");
  }

  return 0;
}
