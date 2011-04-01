#ifndef LEXER_H
#define LEXER_H

#include "String.h"
#include "AST.h"
#include "ListBuilder.h"
#include "LexemGenerator.h"
#include "DataSource.h"

class Lexer: public DataSource<AST::Base *>
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

    Lexer(DataSource<int> *source, StringTable *table)
      : m_state(S_Whitespace), m_is_literal(false), 
        m_row(0), m_col(0), m_source(source), m_lexgen(table) {}
    
    // Reimplemented from DataSource<AST::Base *>
    virtual AST::Base *getNext();

  private:
    enum State
    {
      S_Whitespace,
      S_Quoted,
      S_QuotedEsc,
      S_Token,
      S_Comment
    };

    void feed(char c); 

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

    DataSource<int> *m_source;
    LexemGenerator m_lexgen;
};

#endif // LEXER_H

