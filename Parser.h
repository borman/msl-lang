#ifndef PARSER_h
#define PARSER_H

#include <string>
#include "ASTLexem.h"
#include "ASTExpr.h"
#include "ASTOperator.h"
#include "ListBuilder.h"

class Parser
{
  public:
    class Exception
    {
      public:
        Exception(const std::string &text, AST::Base *tokens)
          : m_text(text), m_tokens(tokens) {}
        std::string text() const { return m_text; }
        AST::Base *tokens() const { return m_tokens; }
      private:
        std::string m_text;
        AST::Base *m_tokens;
    };

    void feed(AST::Base *tokens);
    AST::Fun *peek();

  private:
    AST::Fun *readFun();
    AST::Operator *readBlock();
    AST::Operator *readOperator();
    AST::Expression *readExpr();
    AST::Expression *readSExpr();

    bool nextIsSym(AST::Symbol::Subtype t);
    void expectSym(AST::Symbol::Subtype t);
    bool nextIs(AST::Base::Type t);
    void expect(AST::Base::Type t);
    void expectLValue(AST::Expression *expr);
    void expectEqualSign();

    AST::Expression *fold(AST::Expression *formula, int prio);
    AST::Expression *foldAll(AST::Expression *formula);

    template<class T>
    T *head() { return m_tokens->as<T>(); }

    template<class T>
    T *takeHead()
    {
      T *h = head<T>();
      m_tokens = h->template next<AST::Base>();
      h->setNext(NULL);
      return h;
    }

    void popHead() { delete takeHead<AST::Base>(); }


    AST::Base *m_tokens;
    ListBuilder<AST::Base> m_token_queue;
};

#endif // PARSER_H
