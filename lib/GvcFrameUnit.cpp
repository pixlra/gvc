/** \file     TComPic.cpp
    \brief    picture class
*/

#include "GvcFrameUnit.h"
#include "GvcBlockUnit.h"

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Constructor / destructor / create / destroy
// ====================================================================================================================

GvcFrameUnit::GvcFrameUnit()
: m_pcFrameYuvPred                          (NULL)
, m_pcFrameYuvResi                          (NULL)
{
  for(int i=0; i<NUM_FRAME_YUV; i++)
  {
    m_apcFrameYuv[i] = NULL;
  }
}

GvcFrameUnit::~GvcFrameUnit()
{
  destroy();
}

void GvcFrameUnit::create( ChromaFormat chromaFormatIDC, int iWidth, int iHeight, unsigned int uiMaxCuWidth, unsigned int uiMaxCuHeight, unsigned int uiMaxDepth, bool bIsVirtual)
{
  destroy();

  if (!bIsVirtual)
  {
    m_apcFrameYuv[FRAME_YUV_ORG]   = new GvcFrameUnitYuv;
    m_apcFrameYuv[FRAME_YUV_ORG]->create( iWidth, iHeight, chromaFormatIDC, uiMaxCuWidth, uiMaxCuHeight, true );
  }
  m_apcFrameYuv[FRAME_YUV_REC]  = new GvcFrameUnitYuv;
  m_apcFrameYuv[FRAME_YUV_REC]->create( iWidth, iHeight, chromaFormatIDC, uiMaxCuWidth, uiMaxCuHeight, true );
}

void GvcFrameUnit::destroy()
{

  for(unsigned int i=0; i<NUM_FRAME_YUV; i++)
  {
    if (m_apcFrameYuv[i])
    {
      m_apcFrameYuv[i]->destroy();
      delete m_apcFrameYuv[i];
      m_apcFrameYuv[i]  = NULL;
    }
  }
}

//! \}
