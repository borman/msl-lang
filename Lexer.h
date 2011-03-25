#ifndef LEXER_H
#define LEXER_H

#include "AST.h"
#include "ListBuilder.h"

class Lexer
{
  public:
    class Exception
    {
      public:
        Exception(const char *text, const std::string &token, 
                  const TextRegion &region) 
          : m_text(text), m_token(token), m_region(region) {}

        const char *text() const { return m_text; }
        std::string token() const { return m_token; }
        TextRegion region() const { return m_region; }

      private:
        const char *m_text;
        std::string m_token;
        TextRegion m_region;
    };

    Lexer(StringTable *table)
      : m_stringtable(table) {}
    ~Lexer();

    void feed(AST::Token *tokens);
    AST::Base *peek() { return m_ready.takeAll(); }

  private:
    AST::Base *consume(AST::Token *token);

    StringTable *m_stringtable;
    ListBuilder<AST::Base> m_ready;
};

#endif // LEXER_H
