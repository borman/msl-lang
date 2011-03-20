#ifndef TEXTREGION_H
#define TEXTREGION_H

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

#endif // TEXTREGION_H
