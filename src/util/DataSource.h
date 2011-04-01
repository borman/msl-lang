#ifndef DATASOURCE_H
#define DATASOURCE_H

template<class T>
class DataSource
{
  public:
    virtual T getNext() = 0;
};

#endif // DATASOURCE_H

