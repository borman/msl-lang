#include <cstdio>
#include "Lexer.h"

int main()
{
  Lexer lex;
  int c;
  
  try
  {
    while ((c = getchar()) != EOF)
      lex.feed(c);
    lex.feed('\n');
  }
  catch (const LexerException &e)
  {
    printf("stdin:%u:%u: Lexer error: %s\n", e.row()+1, e.col()+1, e.text().c_str());
    Token *token = lex.peek();
    while (token)
    {
      Token *next = token->next<Token>();
      delete token;
      token = next;
    }
    return 1;
  }

  Token *tokens = lex.peek();
  for(Token *t = tokens; t != NULL; t = t->next<Token>())
  {
    Lexem::TextRegion r = t->region();
    printf("<%u:%u - %u:%u>%c %s\n", 
           r.startRow, r.startCol,
           r.endRow, r.endCol,
           t->literal()? 's' : ' ',
           t->text().c_str());
  }

  return 0;
}
