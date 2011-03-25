#include <cctype>
#include <cstdlib>
#include "Lexer.h"
#include "Symbols.h"
#include "ASTExpr.h"

using namespace std;
using namespace AST;

Lexer::~Lexer()
{
}

void Lexer::feed(Token *tokens)
{
  try
  {
    while (tokens != NULL)
    {
      m_ready.add(consume(tokens));
      Token *next = tokens->next<Token>();
      delete tokens;
      tokens = next;
    }
  }
  catch (const Lexer::Exception &)
  {
    // Delete leftover
    deleteChain(tokens);
    throw;
  }
}


static bool validateIdentifier(const string &str)
{
  for (size_t i=0; i<str.length(); i++)
  {
    const char c = str[i];
    if (!(isalnum(c) || c == '_' ))
      return false;
  }
  return true;
}

static bool validateReal(const string &str, double &result)
{
  bool neg = str[0] == '~';
  const char *p = str.c_str();
  if (neg)
    p++;
  if (*p == '\0')
    return false;

  double v = strtod(p, const_cast<char **>(&p));
  if (*p == '\0')
  {
    result = neg? -v : v;
    return true;
  }
  else
    return false;
}

static bool validateInt(const string &str, int &result)
{
  bool neg = str[0] == '~';
  const char *p = str.c_str();
  if (neg)
    p++;
  if (*p == '\0')
    return false;

  double v = strtol(p, const_cast<char **>(&p), 10);
  if (*p == '\0')
  {
    result = neg? -v : v;
    return true;
  }
  else
    return false;
}

static Infix::Subtype infixType(Symbol::Subtype t)
{
  switch (t)
  {
    case Symbol::Equals: return Infix::Equals;
    case Symbol::Less: return Infix::Less;
    case Symbol::Greater: return Infix::Greater;

    case Symbol::Plus: return Infix::Plus;
    case Symbol::Minus: return Infix::Minus;
    case Symbol::Mul: return Infix::Mul;
    case Symbol::Div: return Infix::Div;
    case Symbol::Mod: return Infix::Mod;

    case Symbol::And: return Infix::And;
    case Symbol::Or: return Infix::Or;

    default: abort();
  }
}

Base *Lexer::consume(Token *token)
{
  const string &str = token->text();

  // Literal
  if (token->isLiteral())
    return new Literal(Atom(str.c_str(), m_stringtable), token->region());

  // Symbol
  Symbols::Ref sym = Symbols::find(str);
  if (sym.isValid())
  {
    if (sym.type() > Symbol::InfixBegin && sym.type() < Symbol::InfixEnd)
      return new Infix(infixType(sym.type()), NULL, NULL, token->region());
    else
      return new Symbol(sym.type(), token->region());
  }

  // Boolean
  if (str == "true")
    return new Bool(true, token->region());
  if (str == "false")
    return new Bool(false, token->region());

  // Function name
  if (islower(str[0]))
  {
    if (!validateIdentifier(str))
      throw Exception("Invalid function name", str, token->region());
    return new FuncCall(Atom(str.c_str(), m_stringtable), NULL, token->region());
  }

  // Variable name
  if (isupper(str[0]))
  {
    if (!validateIdentifier(str))
      throw Exception("Invalid variable name", str, token->region());
    return new Variable(Atom(str.c_str(), m_stringtable), token->region());
  }

  // Array name
  if (str[0] == '$')
  {
    string name = str.substr(1); // Remove leading $
    if (!validateIdentifier(name))
      throw Exception("Invalid array name", str, token->region());
    return new ArrayItem(Atom(name.c_str(), m_stringtable), NULL, token->region());
  }

  // Real
  if (str.find('.') != string::npos)
  {
    double d;
    if (!validateReal(str, d))
      throw Exception("Invalid real number", str, token->region());
    return new Real(d, token->region());
  }

  // Only integer possible here
  int i;
  if (!validateInt(str, i))
    throw Exception("Invalid integer number", str, token->region());
  return new Int(i, token->region());
}
