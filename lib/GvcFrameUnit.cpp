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
{
}

GvcFrameUnit::~GvcFrameUnit()
{
}

void GvcFrameUnit:: create(const int picWidth, const int picHeight, const ChromaFormat chromaFormatIDC, const unsigned int maxCUWidth, const unsigned int maxCUHeight, const bool bUseMargin)
{
  m_iFrameWidth       = picWidth;
  m_iFrameHeight      = picHeight;
  m_chromaFormatIDC   = chromaFormatIDC;
  m_iMarginX          = (bUseMargin?maxCUWidth:0) + 16;   // for 16-byte alignment
  m_iMarginY          = (bUseMargin?maxCUHeight:0) + 16;  // margin for 8-tap filter and infinite padding
  // assign the picture arrays and set up the ptr to the top left of the original picture
  for(unsigned int comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    const ComponentID ch=ComponentID(comp);
    m_apsFrameBuf[comp] = (short*)xMalloc( short, getStride(ch) * getTotalHeight(ch));
    m_apsFrameOrg[comp]  = m_apsFrameBuf[comp] + (m_iMarginY >> getComponentScaleY(ch)) * getStride(ch) + (m_iMarginX >> getComponentScaleX(ch));
  }
}

void GvcFrameUnit::destroy()
{}

//! \}
