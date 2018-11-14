//
// Created by rmonteiro on 24-10-2018.
//

#include "GvcFrameUnitSym.h"

GvcFrameUnitSym::GvcFrameUnitSym()
        :m_frameWidthInCtus(0)
        ,m_frameHeightInCtus(0)
        ,m_uiMinCUWidth(0)
        ,m_uiMinCUHeight(0)
        ,m_uhTotalDepth(0)
        ,m_numPartitionsInCtu(0)
        ,m_numPartInCtuWidth(0)
        ,m_numPartInCtuHeight(0)
        ,m_numCtusInFrame(0)
        ,m_frameBUArray(NULL)
        {}

GvcFrameUnitSym::~GvcFrameUnitSym()
{
    destroy();
}

void GvcFrameUnitSym::create  ( ChromaFormat chromaFormatIDC, int iWidth, int iHeight, unsigned int uiMaxCuWidth, unsigned int uiMaxCuHeight, unsigned int uiMaxDepth)
{
    destroy();

    m_uhTotalDepth       = uiMaxDepth;
    m_numPartitionsInCtu = 1<<(m_uhTotalDepth<<1);

    m_uiMinCUWidth       = uiMaxCuWidth  >> m_uhTotalDepth;
    m_uiMinCUHeight      = uiMaxCuHeight >> m_uhTotalDepth;

    m_numPartInCtuWidth  = uiMaxCuWidth  / m_uiMinCUWidth;  // equivalent to 1<<m_uhTotalDepth
    m_numPartInCtuHeight = uiMaxCuHeight / m_uiMinCUHeight; // equivalent to 1<<m_uhTotalDepth

    m_frameWidthInCtus   = ( iWidth %uiMaxCuWidth  ) ? iWidth /uiMaxCuWidth  + 1 : iWidth /uiMaxCuWidth;
    m_frameHeightInCtus  = ( iHeight%uiMaxCuHeight ) ? iHeight/uiMaxCuHeight + 1 : iHeight/uiMaxCuHeight;

    m_numCtusInFrame     = m_frameWidthInCtus * m_frameHeightInCtus;
    m_frameBUArray    = new GvcBlockUnit*[m_numCtusInFrame];

    for (unsigned int i=0; i<m_numCtusInFrame ; i++ )
    {
        m_frameBUArray[i] = new GvcBlockUnit;
        //m_frameBUArray[i]->create( chromaFormatIDC, m_numPartitionsInCtu, uiMaxCuWidth, uiMaxCuHeight, false, uiMaxCuWidth >> m_uhTotalDepth);
    }

}

void GvcFrameUnitSym::destroy()
{
    if (m_frameBUArray)
    {
        for (unsigned int i = 0; i < m_numCtusInFrame; i++)
        {
            if (m_frameBUArray[i])
            {
                //m_frameBUArray[i]->destroy();
                delete m_frameBUArray[i];
                m_frameBUArray[i] = NULL;
            }
        }
        delete [] m_frameBUArray;
        m_frameBUArray = NULL;
    }
}


