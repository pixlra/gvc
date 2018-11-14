//
// Created by rmonteiro on 24-10-2018.
//

#ifndef GVC_GVCBLOCKUNIT_H
#define GVC_GVCBLOCKUNIT_H

#include "TypeDef.h"

class GvcFrameUnit;

class GvcBlockUnit
{
private:

    GvcFrameUnit*      m_pcFrame;                                ///< picture class pointer
    unsigned int          m_uiBUAddr;                            ///< CTU (also known as LCU) address in a slice (Raster-scan address, as opposed to tile-scan/encoding order).
    unsigned int          m_uiBUPelX;                             ///< CU position in a pixel (X)
    unsigned int          m_uiBUPelY;                             ///< CU position in a pixel (Y)
    unsigned int          m_uiNumPartition;                       ///< total number of minimum partitions in a CU
    int           m_unitSize;                             ///< size of a "minimum partition"

public:
    GvcBlockUnit();
    ~GvcBlockUnit();


  void          create                        ( ChromaFormat chromaFormatIDC, unsigned int uiNumPartition, unsigned int uiWidth, unsigned int uiHeight, int unitSize);
    void          destroy                       ( );

    void          initBU                       ( GvcFrameUnit* pcPic, unsigned int ctuRsAddr );

  GvcFrameUnit* getFrame                    ( )                                                          { return m_pcFrame;                            }
    unsigned int&         getCtuRsAddr                  ( )                                                          { return m_uiBUAddr;                        }
    unsigned int          getCUPelX                     ( )                                                   { return m_uiBUPelX;                         }
    unsigned int          getCUPelY                     ( )                                                     { return m_uiBUPelY;                         }
    unsigned int&         getTotalNumPart               ( )                                                          { return m_uiNumPartition;    }
};

#endif //GVC_GVCBLOCKUNIT_H

