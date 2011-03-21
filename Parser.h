#ifndef PARSER_h
#define PARSER_H

class Parser
{
  public:
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
    void expect(AST::Base::Type t);
    void expectLValue(Expression *expr);
    void expectEqualSign();


    template<class T>
    T *head() { return m_tokens->as<T>(); }

    template<class T>
    void takeHead()
    {
      T *h = head<T>();
      m_tokens = h->next<AST::Base>();
      h->setNext(NULL);
      return h;
    }

    void popHead() { delete takeHead<AST::Base>(); }


    AST::Base *m_tokens;
    ListBuilder<AST::Base> m_token_queue;
};

#endif // PARSER_H
