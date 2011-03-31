#ifndef STACK_H
#define STACK_H

template<class T>
class Stack
{
  public:
    Stack()
      : m_top(NULL) {}

    ~Stack()
    {
      while (!empty())
        pop();
    }

    T &top() { return m_top->data; }

    void pop() 
    {
      Node *t = m_top;
      m_top = m_top->next;
      delete t;
    }

    void push(const T &v)
    {
      m_top = new Node(v, m_top);
    }

    bool empty() const { return m_top == NULL; }

  private:
    struct Node
    {
      Node(const T &d, Node *n=NULL): next(n), data(d) {}
      Node *next;
      T data;
    };
    Node *m_top;
};

#endif // STACK_H
