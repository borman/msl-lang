#include <cstring>
#include "Tokenizer.h"

Tokenizer::~Tokenizer()
{
  Lexem::Token *t = m_tokens.takeAll();
  while (t != NULL)
  {
    Lexem::Token *next = t->next<Lexem::Token>();
    delete t;
    t = next;
  }
}

void Tokenizer::feed(char c)
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
  return isalnum(c) || c == '.' || c == '$' || c == '_' || c == '~';
}

static inline bool isdelim(char c)
{
  static const char *delimiters = "[](),+-*/%><=&|";
  return strchr(delimiters, c) != NULL;
}

Tokenizer::State Tokenizer::stateWhitespace(char c)
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
    throw Exception(m_row, m_col, "Unexpected character after whitespace");
}

Tokenizer::State Tokenizer::stateQuoted(char c)
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

Tokenizer::State Tokenizer::stateQuotedEsc(char c)
{
  if (c == 'n')
    putChar('\n');
  else if (c == '"')
    putChar('"');
  else
    throw Exception(m_row, m_col, "Invalid escape sequence");
  return S_Quoted;
}

Tokenizer::State Tokenizer::stateToken(char c)
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
    throw Exception(m_row, m_col, "Unexpected character in token");
}

Tokenizer::State Tokenizer::stateComment(char c)
{
  if (c == '\n')
    return S_Whitespace;
  else
    return S_Comment;
}

void Tokenizer::putChar(char c)
{
  m_current_token += c;
}

void Tokenizer::beginToken(int row, int col)
{
  m_region.startRow = row;
  m_region.startCol = col;
}

void Tokenizer::endToken(int row, int col)
{
  m_region.endRow = row;
  m_region.endCol = col;
  m_tokens.add(new Lexem::Token(m_current_token, m_is_literal, m_region));
  m_current_token.clear();
  m_is_literal = false; 
}

void Tokenizer::setLiteral()
{
  m_is_literal = true;
}

