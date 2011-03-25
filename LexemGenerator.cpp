#include <cctype>
#include <cstring>
#include <cstdlib>
#include "LexemGenerator.h"
#include "Symbols.h"
#include "ASTExpr.h"

using namespace AST;

static bool validateIdentifier(const char *str)
{
  size_t len = strlen(str);
  for (size_t i=0; i<len; i++)
  {
    const char c = str[i];
    if (!(isalnum(c) || c == '_' ))
      return false;
  }
  return true;
}

static bool validateReal(const char *str, double &result)
{
  bool neg = str[0] == '~';
  if (neg)
    str++;
  if (str[0] == '\0')
    return false;

  double v = strtod(str, const_cast<char **>(&str));
  if (str[0] == '\0')
  {
    result = neg? -v : v;
    return true;
  }
  else
    return false;
}

static bool validateInt(const char *str, int &result)
{
  bool neg = str[0] == '~';
  if (neg)
    str++;
  if (str[0] == '\0')
    return false;

  double v = strtol(str, const_cast<char **>(&str), 10);
  if (str[0] == '\0')
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

Base *LexemGenerator::make(const char *str, bool isLiteral, const TextRegion &region)
{
  // Literal
  if (isLiteral)
    return new Literal(atom(str), region);

  // Symbol
  Symbols::Ref sym = Symbols::find(str);
  if (sym.isValid())
  {
    if (sym.type() > Symbol::InfixBegin && sym.type() < Symbol::InfixEnd)
      return new Infix(infixType(sym.type()), NULL, NULL, region);
    else
      return new Symbol(sym.type(), region);
  }

  // Boolean
  if (strcmp(str, "true") == 0)
    return new Bool(true, region);
  if (strcmp(str, "false") == 0)
    return new Bool(false, region);

  // Function name
  if (islower(str[0]))
  {
    if (!validateIdentifier(str))
      throw Exception("Invalid function name", str, region);
    return new FuncCall(atom(str), NULL, region);
  }

  // Variable name
  if (isupper(str[0]))
  {
    if (!validateIdentifier(str))
      throw Exception("Invalid variable name", str, region);
    return new Variable(atom(str), region);
  }

  // Array name
  if (str[0] == '$')
  {
    const char *name = str+1; // Remove leading $
    if (!validateIdentifier(name))
      throw Exception("Invalid array name", str, region);
    return new ArrayItem(atom(name), NULL, region);
  }

  // Real
  if (strchr(str, '.') != NULL)
  {
    double d;
    if (!validateReal(str, d))
      throw Exception("Invalid real number", str, region);
    return new Real(d, region);
  }

  // Only integer possible here
  int i;
  if (!validateInt(str, i))
    throw Exception("Invalid integer number", str, region);
  return new Int(i, region);
}
