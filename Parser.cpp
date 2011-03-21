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
}

Expression *Parser::readSExpr()
{
}

