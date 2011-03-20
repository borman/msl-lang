#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include "ASTLexem.h"
#include "ListBuilder.h"

class Tokenizer
{
  public:
    class Exception
    {
      public:
        Exception(unsigned int r, unsigned int c, 
            const std::string &t = std::string())
          : m_row(r), m_col(c), m_text(t) {}

        unsigned int row() const { return m_row; }
        unsigned int col() const { return m_col; }
        std::string text() const { return m_text; }
      private:
        unsigned int m_row;
        unsigned int m_col;
        std::string m_text;
    };

    Tokenizer()
      : m_state(S_Whitespace), m_is_literal(false), m_row(0), m_col(0) {}
    ~Tokenizer();
    
    void feed(char c); 
    AST::Token *peek() { return m_tokens.takeAll(); }
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
    std::string m_current_token;
    ListBuilder<AST::Token> m_tokens;
    TextRegion m_region;
    unsigned int m_row;
    unsigned int m_col;
};

#endif // TOKENIZER_H

