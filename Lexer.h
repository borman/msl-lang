#ifndef LEXER_H
#define LEXER_H

#include <string>
#include "Lexem.h"
#include "ListBuilder.h"

class LexerException
{
  public:
    LexerException(unsigned int r, unsigned int c, 
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

class Lexer
{
  public:
    Lexer()
      : m_state(S_Whitespace), m_is_literal(false), m_row(0), m_col(0) {}
    ~Lexer();
    
    void feed(char c); 
    Token *peek() { return m_tokens.takeAll(); }
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
    ListBuilder<Token> m_tokens;
    Lexem::TextRegion m_region;
    unsigned int m_row;
    unsigned int m_col;
};

#endif // LEXER_H

