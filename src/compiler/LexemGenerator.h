#ifndef LEXEMGENERATOR_H
#define LEXEMGENERATOR_H

#include "String.h"
#include "AST.h"
#include "ListBuilder.h"

class LexemGenerator
{
  public:
    class Exception
    {
      public:
        Exception(const char *text, const char *token, 
                  const TextRegion &region) 
          : m_text(text), m_token(token), m_region(region) {}

        const char *text() const { return m_text; }
        String token() const { return m_token; }
        TextRegion region() const { return m_region; }

      private:
        const char *m_text;
        String m_token;
        TextRegion m_region;
    };

    LexemGenerator(StringTable *table)
      : m_stringtable(table) {}

    AST::Base *make(const char *str, bool isLiteral, const TextRegion &region);

  private:
    Atom atom(const char *str) { return Atom(str, m_stringtable); }

    StringTable *m_stringtable;
};

#endif // LEXEMGENERATOR_H
