#ifndef LEXEM_H
#define LEXEM_H

#include <string>

class Lexem
{
  public:
    struct TextRegion
    {
      TextRegion(unsigned int sr=0, unsigned int sc=0, 
                 unsigned int er=0, unsigned int ec=0)
        : startRow(sr), startCol(sc), endRow(er), endCol(ec) {}
      unsigned int startRow;
      unsigned int startCol;
      unsigned int endRow;
      unsigned int endCol;
    };
    enum Type
    {
      Invalid,
      Token
    };

    Lexem(Type t, const TextRegion &r)
      : m_type(t), m_region(r), m_next(NULL) {}

    Type type() const { return m_type; }
    TextRegion region() const { return m_region; }

    template<class T>
      T *next() { return static_cast<T *>(m_next); }
    void setNext(Lexem *l) { m_next = l; }

  private:
    static const Type m_class_type = Invalid;

    Type m_type;
    TextRegion m_region;
    Lexem *m_next;
};

// ===== Specializations

class Token: public Lexem
{
  public:
    Token(const std::string &t, bool l, const TextRegion &r)
      : Lexem(Lexem::Token, r), m_is_literal(l), m_text(t) {} 

    std::string text() const { return m_text; }
    bool literal() const { return m_is_literal; }
  private:
    static const Type m_class_type = Lexem::Token;

    bool m_is_literal;
    std::string m_text;
};

#endif // LEXEM_H

