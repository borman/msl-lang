#ifndef LEXER_H
#define LEXER_H

#include "String.h"
#include "AST.h"
#include "ListBuilder.h"
#include "LexemGenerator.h"

class Lexer
{
  public:
    class Exception
    {
      public:
        Exception(unsigned int row, unsigned int col, 
            const char *text = NULL)
          : m_row(row), m_col(col), m_text(text) {}

        unsigned int row() const { return m_row; }
        unsigned int col() const { return m_col; }
        const char *text() const { return m_text; }
      private:
        unsigned int m_row;
        unsigned int m_col;
        const char *m_text;
    };

    Lexer(StringTable *table)
      : m_state(S_Whitespace), m_is_literal(false), 
        m_row(0), m_col(0), m_lexgen(table) {}
    
    void feed(char c); 
    AST::Base *peek() { return m_tokens.takeAll(); }
  private:
    enum State
    {
      S_Whitespace,
      S_Quoted,
      S_QuotedEsc,
      S_Token,
      S_Comment
    };

    State stateWhitespace(char c);
    State stateQuoted(char c);
    State stateQuotedEsc(char c);
    State stateToken(char c);
    State stateComment(char c);

    void putChar(char c);
    void beginToken(int row, int col);
    void endToken(int row, int col);
    void setLiteral();
  
    State m_state;
    bool m_is_literal;
    String m_current_token;
    ListBuilder<AST::Base> m_tokens;
    TextRegion m_region;
    unsigned int m_row;
    unsigned int m_col;

    LexemGenerator m_lexgen;
};

#endif // LEXER_H

