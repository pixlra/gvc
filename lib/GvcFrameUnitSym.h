//
// Created by rmonteiro on 24-10-2018.
//

#ifndef GVC_GVCFRAMEUNITSYM_H
#define GVC_GVCFRAMEUNITSYM_H

#include "GvcBlockUnit.h"
#include "TypeDef.h"

class GvcFrameUnitSym
{

    private:
        unsigned int          m_frameWidthInCtus;
        unsigned int          m_frameHeightInCtus;

        unsigned int          m_uiMinCUWidth;
        unsigned int          m_uiMinCUHeight;

        char                  m_uhTotalDepth;       ///< max. depth
        unsigned int          m_numPartitionsInCtu;
        unsigned int          m_numPartInCtuWidth;
        unsigned int          m_numPartInCtuHeight;
        unsigned int          m_numCtusInFrame;

        GvcBlockUnit**  m_pictureCtuArray;        ///< array of CU data.

    public:
        void               create  ( ChromaFormat chromaFormatIDC, int iWidth, int iHeight, unsigned int uiMaxCuWidth, unsigned int uiMaxCuHeight, unsigned int uiMaxDepth);
        void               destroy ();

        GvcFrameUnitSym  ();
        ~GvcFrameUnitSym();

        unsigned int               getFrameWidthInCtus() const                           { return m_frameWidthInCtus;            }
        unsigned int               getFrameHeightInCtus() const                          { return m_frameHeightInCtus;           }
        unsigned int               getMinCUWidth() const                                 { return m_uiMinCUWidth;                }
        unsigned int               getMinCUHeight() const                                { return m_uiMinCUHeight;               }
        unsigned int               getNumberOfCtusInFrame() const                        { return m_numCtusInFrame;              }
        GvcBlockUnit*        getCtu( unsigned int ctuRsAddr )                              { return m_pictureCtuArray[ctuRsAddr];  }
        const GvcBlockUnit*  getCtu( unsigned int ctuRsAddr ) const                        { return m_pictureCtuArray[ctuRsAddr];  }

        unsigned int               getNumPartitionsInCtu() const                         { return m_numPartitionsInCtu;   }
        unsigned int               getNumPartInCtuWidth() const                          { return m_numPartInCtuWidth;    }
        unsigned int               getNumPartInCtuHeight() const                         { return m_numPartInCtuHeight;   }

//! \}
};

#endif //GVC_GVCFRAMEUNITSYM_H

