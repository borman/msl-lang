#ifndef LISTBUILDER_H
#define LISTBUILDER_H

#include <cstddef>

template<class T>
class ListBuilder
{
  public:
    ListBuilder(): m_head(NULL), m_tail(NULL) {} 
    ~ListBuilder() { deleteChain(m_head); }

    void add(T *item)
    {
      if (m_tail == NULL)
        m_head = m_tail = item;
      else
      {
        m_tail->setNext(item);
        while (m_tail->template next<T>() != NULL)
          m_tail = m_tail->template next<T>();
      }
    }

    T *takeAll()
    {
      T *res = m_head;
      m_head = m_tail = NULL;
      return res;
    }

    T *takeFirst()
    {
      if (m_head == m_tail)
        return takeAll();
      T *head = m_head;
      m_head = m_head->template next<T>();
      head->setNext(NULL);
      return head;
    }

    bool empty() const { return m_head == NULL; }
  private:
    T *m_head;
    T *m_tail;
};

#endif // LISTBUILDER_H

