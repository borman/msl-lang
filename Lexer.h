#ifndef LEXER_H
#define LEXER_H

#include "Lexem.h"
#include "ListBuilder.h"

class Lexer
{
  public:
    class Exception
    {
      public:
        Exception(const std::string &text, const std::string &token, 
                  const Lexem::TextRegion &region) 
          : m_text(text), m_token(token), m_region(region) {}

        std::string text() const { return m_text; }
        std::string token() const { return m_token; }
        Lexem::TextRegion region() const { return m_region; }

      private:
        std::string m_text;
        std::string m_token;
        Lexem::TextRegion m_region;
    };

    ~Lexer();

    void feed(Lexem::Token *tokens);
    Lexem::Base *peek() { return m_ready.takeAll(); }

  private:
    Lexem::Base *consume(Lexem::Token *token);

    ListBuilder<Lexem::Base> m_ready;
};

#endif // LEXER_H
