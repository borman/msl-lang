#ifndef FILECHARSOURCE_H
#define FILECHARSOURCE_H

#include "DataSource.h"
#include "File.h"

class FileCharSource: public DataSource<int>
{
  public:
    FileCharSource(File *src): m_file(src) {}
    // Reimplemented from DataSource<int>
    virtual int getNext() { return m_file->getc(); }

  private:
    File *m_file;
};

#endif // FILECHARSOURCE_H
