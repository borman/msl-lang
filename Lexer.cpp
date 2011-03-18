#include <cstring>
#include "Lexer.h"

Lexer::~Lexer()
{
  Token *t = m_tokens.takeAll();
  while (t != NULL)
  {
    Token *next = t->next<Token>();
    delete t;
    t = next;
  }
}

void Lexer::feed(char c)
{
  switch (m_state)
  {
    case S_Whitespace:
      m_state = stateWhitespace(c);
      break;
    case S_Quoted:
      m_state = stateQuoted(c);
      break;
    case S_QuotedEsc:
      m_state = stateQuotedEsc(c);
      break;
    case S_Token:
      m_state = stateToken(c);
      break;
    case S_Comment:
      m_state = stateComment(c);
      break;
  }
  if (c != '\n')
    m_col++;
  else
  {
    m_row++;
    m_col = 0;
  }
}

static inline bool istoken(char c)
{
  return isalnum(c) || c == '.' || c == '$' || c == '_';
}

static inline bool isdelim(char c)
{
  static const char *delimiters = "[](),+-*/%><=&|";
  return strchr(delimiters, c) != NULL;
}

Lexer::State Lexer::stateWhitespace(char c)
{
  if (isspace(c))
    return S_Whitespace;
  else if (isdelim(c))
  {
    beginToken(m_row, m_col);
    putChar(c);
    endToken(m_row, m_col);
    return S_Whitespace;
  }
  else if (istoken(c))
  {
    beginToken(m_row, m_col);
    putChar(c);
    return S_Token;
  }
  else if (c == '"')
  {
    beginToken(m_row, m_col);
    setLiteral();
    return S_Quoted;
  }
  else if (c == ';')
    return S_Comment;
  else 
    throw LexerException(m_row, m_col, "Unexpected character after whitespace");
}

Lexer::State Lexer::stateQuoted(char c)
{
  if (c == '"')
  {
    endToken(m_row, m_col);
    return S_Whitespace;
  }
  else if (c == '\\')
    return S_QuotedEsc;
  else
  {
    putChar(c);
    return S_Quoted;
  }
}

Lexer::State Lexer::stateQuotedEsc(char c)
{
  if (c == 'n')
    putChar('\n');
  else if (c == '"')
    putChar('"');
  else
    throw LexerException(m_row, m_col, "Invalid escape sequence");
  return S_Quoted;
}

Lexer::State Lexer::stateToken(char c)
{
  if (istoken(c))
  {
    putChar(c);
    return S_Token; 
  }
  else if (isspace(c))
  {
    endToken(m_row, m_col-1);
    return S_Whitespace;
  }
  else if (isdelim(c))
  {
    endToken(m_row, m_col-1);
    beginToken(m_row, m_col);
    putChar(c);
    endToken(m_row, m_col);
    return S_Whitespace;
  }
  else if (c == '"')
  {
    endToken(m_row, m_col);
    return S_Quoted;
  }
  else if (c == ';')
  {
    endToken(m_row, m_col-1);
    return S_Comment;
  }
  else
    throw LexerException(m_row, m_col, "Unexpected character in token");
}

Lexer::State Lexer::stateComment(char c)
{
  if (c == '\n')
    return S_Whitespace;
  else
    return S_Comment;
}

void Lexer::putChar(char c)
{
  m_current_token += c;
}

void Lexer::beginToken(int row, int col)
{
  m_region.startRow = row;
  m_region.startCol = col;
}

void Lexer::endToken(int row, int col)
{
  m_region.endRow = row;
  m_region.endCol = col;
  m_tokens.add(new Token(m_current_token, m_is_literal, m_region));
  m_current_token.clear();
  m_is_literal = false; 
}

void Lexer::setLiteral()
{
  m_is_literal = true;
}

