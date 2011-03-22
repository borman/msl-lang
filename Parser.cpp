#include <cstdio>
#include "Parser.h"

using namespace AST;

void Parser::feed(Base *tokens)
{
  m_token_queue.add(tokens);
}

Fun *Parser::peek()
{
  m_tokens = m_token_queue.takeAll();
  ListBuilder<Fun> funs;
  while (m_tokens != NULL)
    funs.add(readFun());
  return funs.takeAll();
}


Fun *Parser::readFun()
{
  expectSym(Symbol::Fun);
  
  expect(Base::FuncCall);
  const std::string name = head<FuncCall>()->name();
  popHead();

  Expression *arg = readSExpr();
  Operator *body = readBlock();

  return new Fun(name, arg, body);
}

Operator *Parser::readBlock()
{
  ListBuilder<Operator> body;
  while (!nextIsSym(Symbol::End))
    body.add(readOperator());
  expectSym(Symbol::End);
  return body.takeAll();
}

Operator *Parser::readOperator()
{
  if (nextIsSym(Symbol::Do))
  {
    expectSym(Symbol::Do);
    Expression *expr = readExpr();
    return new Do(expr);
  }
  else if (nextIsSym(Symbol::If))
  {
    expectSym(Symbol::If);
    Expression *cond = readExpr();
    expectSym(Symbol::Then);
    Operator *positive = readBlock();
    Operator *negative = NULL;
    if (nextIsSym(Symbol::Else))
    {
      expectSym(Symbol::Else);
      negative = readBlock();
    }
    return new If(cond, positive, negative);
  }
  else if (nextIsSym(Symbol::For))
  {
    expectSym(Symbol::For);
    expect(Base::Variable);
    Variable *var = takeHead<Variable>();
    expectSym(Symbol::From);
    Expression *from = readExpr();
    expectSym(Symbol::To);
    Expression *to = readExpr();
    Operator *body = readBlock();
    return new For(var, from, to, body);
  }
  else if (nextIsSym(Symbol::While))
  {
    expectSym(Symbol::While);
    Expression *cond = readExpr();
    Operator *body = readBlock();
    return new While(cond, body);
  }
  else // <LEXPR> = <EXPR>
  {
    Expression *lvalue = readSExpr();
    expectLValue(lvalue);
    expectEqualSign();
    Expression *rvalue = readExpr();
    return new Let(lvalue, rvalue);
  }
}

Expression *Parser::readExpr()
{
  ListBuilder<Expression> sexprs;
  bool finished = false;
  do
  {
    sexprs.add(readSExpr());
    if (nextIs(Base::Infix))
      sexprs.add(takeHead<Infix>());
    else
      finished = true;
  } while (!finished);
  return foldAll(sexprs.takeAll());
}

Expression *Parser::readSExpr()
{
  if (nextIs(Base::Int) || nextIs(Base::Real) || nextIs(Base::Bool)
      || nextIs(Base::Literal) || nextIs(Base::Variable))
    return static_cast<Expression *>(takeHead<Base>());
  else if (nextIs(Base::FuncCall))
  {
    FuncCall *func = takeHead<FuncCall>();
    Expression *arg = readSExpr();
    func->bind(arg);
    return func;
  }
  else if (nextIs(Base::ArrayItem))
  {
    ArrayItem *array = takeHead<ArrayItem>();
    Expression *arg = readSExpr();
    array->bind(arg);
    return array;
  }
  else if (nextIsSym(Symbol::If))
  {
    expectSym(Symbol::If);
    Expression *cond = readExpr();
    expectSym(Symbol::Then);
    Expression *positive = readExpr();
    expectSym(Symbol::Else);
    Expression *negative = readExpr();
    return new Selector(cond, positive, negative);
  }
  else if (nextIsSym(Symbol::LParen))
  {
    expectSym(Symbol::LParen);
    Expression *inside = readExpr();
    expectSym(Symbol::RParen);
    return inside;
  }
  else if (nextIsSym(Symbol::LBracket))
  {
    ListBuilder<AST::Expression> exprs;
    expectSym(Symbol::LBracket);
    while (!nextIsSym(Symbol::RBracket))
    {
      exprs.add(readExpr());
      if (nextIsSym(Symbol::Comma))
        expectSym(Symbol::Comma);
      else
        break;
    }
    expectSym(Symbol::RBracket);
    return new Tuple(exprs.takeAll());
  }
  else throw Exception("Expression expected", m_tokens);
}

// ====== HELPERS ======

Expression *Parser::foldAll(Expression *formula)
{
  // STUB
  return formula;
}

bool Parser::nextIsSym(AST::Symbol::Subtype t)
{
  return nextIs(Base::Symbol)
      && head<Symbol>()->subtype() == t;
}

void Parser::expectSym(AST::Symbol::Subtype t)
{
  if (!nextIsSym(t))
    throw Exception("Symbol expectation failed", m_tokens);
  else
    popHead();
}

bool Parser::nextIs(AST::Base::Type t)
{
  return head<Base>() != NULL 
      && head<Base>()->type() == t;
}

void Parser::expect(AST::Base::Type t)
{
  if (!nextIs(t))
    throw Exception("Class expectation failed", m_tokens);
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
    throw Exception("LValue expected", m_tokens);
}

void Parser::expectEqualSign()
{
  if (nextIs(Base::Infix) && head<Infix>()->subtype() == Infix::Equals)
    popHead();
  else
    throw Exception("Assignment operator expected", m_tokens);
}

