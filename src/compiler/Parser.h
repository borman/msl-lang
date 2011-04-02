#ifndef PARSER_h
#define PARSER_H

#include "AST.h"
#include "ListBuilder.h"
#include "DataSource.h"

class Parser: public DataSource<AST::Fun *>
{
  public:
    class Exception
    {
      public:
        Exception(const char *text, const TextRegion &region = TextRegion())
          : m_text(text), m_region(region) {}
        const char *text() const { return m_text; }
        TextRegion region() const { return m_region; }
      private:
        const char *m_text;
        TextRegion m_region;
    };
    class SymbolExpected: public Exception
    {
      public:
        SymbolExpected(AST::Symbol::Subtype s, const TextRegion &region = TextRegion())
          : Exception("Symbol", region), m_symbol(s) {}
        AST::Symbol::Subtype symbol() const { return m_symbol; }
      private:
        AST::Symbol::Subtype m_symbol;
    };

    Parser(DataSource<AST::Base *> *source): m_source(source), m_next(NULL) {}
    ~Parser() { delete m_next; }

    virtual AST::Fun *getNext();
    AST::Fun *getAll();

  private:
    AST::Fun *readFun();
    AST::Operator *readBlock();
    AST::Operator *readOperator();
    AST::Operator *readOperatorReturn();
    AST::Operator *readOperatorIf();
    AST::Operator *readOperatorFor();
    AST::Operator *readOperatorWhile();
    AST::Expression *readExpr();
    AST::Expression *readSExpr();
    AST::Expression *readSExprFuncCall();
    AST::Expression *readSExprArrayItem();
    AST::Expression *readSExprSelector();
    AST::Expression *readSExprSubexpr();
    AST::Expression *readSExprTuple();

    bool nextIsSym(AST::Symbol::Subtype t);
    void consumeSym(AST::Symbol::Subtype t);
    bool nextIs(AST::Base::Type t);
    void expect(AST::Base::Type t);
    void expectLValue(AST::Expression *expr);
    void expectEqualSign();

    AST::Expression *fold(AST::Expression *formula, int prio);
    AST::Expression *foldAll(AST::Expression *formula);

    template<class T>
    T *next() 
    {
      if (m_next == NULL)
        m_next = m_source->getNext();
      return m_next->as<T>(); 
    }

    template<class T>
    T *takeNext()
    {
      T *n = next<T>();
      m_next = m_source->getNext();
      return n;
    }

    void deleteNext() { delete takeNext<AST::Base>(); }

    DataSource<AST::Base *> *m_source;
    AST::Base *m_next;
};

#endif // PARSER_H
