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

void GvcFrameUnit::create ( const int picWidth,                 ///< picture width
                                            const int picHeight,                ///< picture height
                                            const ChromaFormat chromaFormatIDC, ///< chroma format
                                            const unsigned int maxCUWidth,              ///< used for generating offsets to CUs.
                                            const unsigned int maxCUHeight,             ///< used for generating offsets to CUs.
                                            const bool bUseMargin              ///< if true, then a margin of uiMaxCUWidth+16 and uiMaxCUHeight+16 is created around the image.
)

{
  m_picWidth          = picWidth;
  m_picHeight         = picHeight;
  m_chromaFormatIDC   = chromaFormatIDC;
  m_marginX          = (bUseMargin?maxCUWidth:0) + 16;   // for 16-byte alignment
  m_marginY          = (bUseMargin?maxCUHeight:0) + 16;  // margin for 8-tap filter and infinite padding

  // assign the picture arrays and set up the ptr to the top left of the original picture
  for(unsigned int comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    const ComponentID ch=ComponentID(comp);
    m_apiFrameBuf[comp] = (short*)xMalloc( short, getStride(ch) * getTotalHeight(ch));
    m_piFrameOrg[comp]  = m_apiFrameBuf[comp] + (m_marginY >> getComponentScaleY(ch)) * getStride(ch) + (m_marginX >> getComponentScaleX(ch));
  }
}

void GvcFrameUnit::destroy()
{}

//! \}
