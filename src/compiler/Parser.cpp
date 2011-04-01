#include <cstdio>
#include "Parser.h"

using namespace AST;

// TODO: Track text regions

Fun *Parser::getAll()
{
  Fun *t;
  ListBuilder<Fun> funs;
  while ((t = getNext()) != NULL)
    funs.add(t);
  return funs.takeAll();
}

Fun *Parser::getNext()
{
  if (next<Base>() == NULL)
    return NULL;
  else
    return readFun();
}


Fun *Parser::readFun()
{
  consumeSym(Symbol::Fun);
  
  expect(Base::FuncCall);
  const Atom name = next<FuncCall>()->name();
  deleteNext();

  SafePtr<Expression> arg(readSExpr());
  expectLValue(arg.keep());
  SafePtr<Operator> body(readBlock());

  return new Fun(name, arg, body);
}

Operator *Parser::readBlock()
{
  ListBuilder<Operator> body;
  while (!nextIsSym(Symbol::End))
    body.add(readOperator());
  consumeSym(Symbol::End);
  return body.takeAll();
}

// ====== OPERATORS

// General case
Operator *Parser::readOperator()
{
  if (nextIsSym(Symbol::Do))
    return readOperatorDo();
  else if (nextIsSym(Symbol::Return))
    return readOperatorReturn();
  else if (nextIsSym(Symbol::If))
    return readOperatorIf();
  else if (nextIsSym(Symbol::For))
    return readOperatorFor();
  else if (nextIsSym(Symbol::While))
    return readOperatorWhile();
  else // <LEXPR> = <EXPR>
    return readOperatorLet();
}

// DO
Operator *Parser::readOperatorDo()
{
  consumeSym(Symbol::Do);
  SafePtr<Expression> expr(readExpr());
  return new Do(expr);
}

// RETURN
Operator *Parser::readOperatorReturn()
{
  consumeSym(Symbol::Return);
  SafePtr<Expression> expr(readExpr());
  return new Return(expr);
}

// IF
Operator *Parser::readOperatorIf()
{
  consumeSym(Symbol::If);
  SafePtr<Expression> cond(readExpr());
  consumeSym(Symbol::Then);
  SafePtr<Operator> positive(readBlock());
  SafePtr<Operator> negative(NULL);
  if (nextIsSym(Symbol::Else))
  {
    consumeSym(Symbol::Else);
    negative = readBlock();
  }
  return new If(cond, positive, negative);
}

// FOR
Operator *Parser::readOperatorFor()
{
  consumeSym(Symbol::For);
  expect(Base::Variable);
  SafePtr<Variable> var(takeNext<Variable>());
  consumeSym(Symbol::From);
  SafePtr<Expression> from(readExpr());
  consumeSym(Symbol::To);
  SafePtr<Expression> to(readExpr());
  SafePtr<Operator> body(readBlock());
  return new For(var, from, to, body);
}

// WHILE
Operator *Parser::readOperatorWhile()
{
  consumeSym(Symbol::While);
  SafePtr<Expression> cond(readExpr());
  SafePtr<Operator> body(readBlock());
  return new While(cond, body);
}

// LET
Operator *Parser::readOperatorLet()
{
  SafePtr<Expression> lvalue(readSExpr());
  expectLValue(lvalue);
  expectEqualSign();
  SafePtr<Expression> rvalue(readExpr());
  return new Let(lvalue, rvalue);
}

// ===== EXPRESSIONS

Expression *Parser::readExpr()
{
  ListBuilder<Expression> sexprs;
  bool finished = false;
  do
  {
    sexprs.add(readSExpr());
    if (nextIs(Base::Infix))
      sexprs.add(takeNext<Infix>());
    else
      finished = true;
  } while (!finished);
  return foldAll(sexprs.takeAll());
}

Expression *Parser::readSExpr()
{
  if (nextIs(Base::Int) || nextIs(Base::Real) || nextIs(Base::Bool)
   || nextIs(Base::Literal) || nextIs(Base::Variable))
  {
    return static_cast<Expression *>(takeNext<Base>());
  }
  else if (nextIs(Base::FuncCall))
    return readSExprFuncCall();
  else if (nextIs(Base::ArrayItem))
    return readSExprArrayItem();
  else if (nextIsSym(Symbol::If))
    return readSExprSelector();
  else if (nextIsSym(Symbol::LParen))
    return readSExprSubexpr();
  else if (nextIsSym(Symbol::LBracket))
    return readSExprTuple();
  else 
    throw Exception("Expression expected", next<Base>()->region());
}

// FUNCTION CALL
Expression *Parser::readSExprFuncCall()
{
  SafePtr<FuncCall> func(takeNext<FuncCall>());
  SafePtr<Expression> arg(readSExpr());
  func->bind(arg);
  return func;
}

// ARRAY ITEM
Expression *Parser::readSExprArrayItem()
{
  SafePtr<ArrayItem> array(takeNext<ArrayItem>());
  SafePtr<Expression> arg(readSExpr());
  array->bind(arg);
  return array;
}

// SELECTOR
Expression *Parser::readSExprSelector()
{
  consumeSym(Symbol::If);
  SafePtr<Expression> cond(readExpr());
  consumeSym(Symbol::Then);
  SafePtr<Expression> positive(readExpr());
  consumeSym(Symbol::Else);
  SafePtr<Expression> negative(readExpr());
  return new Selector(cond, positive, negative);
}

// (SUBEXPRESSION)
Expression *Parser::readSExprSubexpr()
{
  consumeSym(Symbol::LParen);
  SafePtr<Expression> inside(readExpr());
  consumeSym(Symbol::RParen);
  return inside;
}

// TUPLE
Expression *Parser::readSExprTuple()
{
  ListBuilder<Expression> exprs;
  consumeSym(Symbol::LBracket);
  while (!nextIsSym(Symbol::RBracket))
  {
    exprs.add(readExpr());
    if (nextIsSym(Symbol::Comma))
      consumeSym(Symbol::Comma);
    else
      break;
  }
  consumeSym(Symbol::RBracket);
  return new Tuple(exprs.takeAll());
}

// ====== HELPERS ======

int infixPriority(Infix::Subtype t)
{
  switch (t)
  {
    case Infix::Mul: return 1;
    case Infix::Div: return 1;
    case Infix::Mod: return 1;
    case Infix::Plus: return 2;
    case Infix::Minus: return 2;
    case Infix::Equals: return 3;
    case Infix::Less: return 3;
    case Infix::Greater: return 3;
    case Infix::And: return 4;
    case Infix::Or: return 4;
  }
  return 0;
}

Expression *Parser::fold(Expression *formula, int prio)
{
  ListBuilder<Expression> folded;
  Expression *tail = formula;
  while (tail != NULL)
  {
    // Detach first
    Expression *first = tail;
    tail = tail->next<Expression>();
    first->setNext(NULL);

    if (tail == NULL)
      folded.add(first);
    else
    {
      // Detach second
      Infix *op = tail->as<Infix>();
      tail = tail->next<Expression>();
      op->setNext(NULL);

      if (prio == infixPriority(op->subtype()))
      {
        // Detach third
        Expression *third = tail;
        tail = tail->next<Expression>();
        third->setNext(NULL);

        // Reattach bound
        op->bind(first, third);
        op->setNext(tail);
        tail = op;
      }
      else
      {
        folded.add(first);
        folded.add(op);
      }
    }
  }
  return folded.takeAll();
}

Expression *Parser::foldAll(Expression *formula)
{
  for (int i=1; i<5; i++)
    formula = fold(formula, i);
  return formula;
}

bool Parser::nextIsSym(Symbol::Subtype t)
{
  return nextIs(Base::Symbol)
      && next<Symbol>()->subtype() == t;
}

void Parser::consumeSym(Symbol::Subtype t)
{
  if (!nextIsSym(t))
    throw Exception("Symbol expectation failed", next<Base>()->region());
  else
    deleteNext();
}

bool Parser::nextIs(Base::Type t)
{
  return next<Base>() != NULL 
      && next<Base>()->type() == t;
}

void Parser::expect(Base::Type t)
{
  if (!nextIs(t))
    throw Exception("Class expectation failed", next<Base>()->region());
}

void Parser::expectLValue(Expression *expr)
{
  if (expr->type() == Base::Variable || expr->type() == Base::ArrayItem)
    return; // Ok
  else if (expr->type() == Base::Tuple)
  {
    Expression *contents = expr->as<Tuple>()->contents();
    while (contents != NULL)
    {
      expectLValue(contents);
      contents = contents->next<Expression>();
    }
  }
  else
    throw Exception("LValue expected", expr->region());
}

void Parser::expectEqualSign()
{
  if (nextIs(Base::Infix) && next<Infix>()->subtype() == Infix::Equals)
    deleteNext();
  else
    throw Exception("Assignment operator expected", next<Base>()->region());
}

