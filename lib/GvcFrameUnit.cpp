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

void GvcFrameUnit::createWithoutCUInfo ( const int picWidth,                 ///< picture width
                                            const int picHeight,                ///< picture height
                                            const ChromaFormat chromaFormatIDC, ///< chroma format
                                            const unsigned int maxCUWidth,              ///< used for generating offsets to CUs.
                                            const unsigned int maxCUHeight,             ///< used for generating offsets to CUs.
                                            const bool bUseMargin              ///< if true, then a margin of uiMaxCUWidth+16 and uiMaxCUHeight+16 is created around the image.
)

{
  destroy();

  m_picWidth          = picWidth;
  m_picHeight         = picHeight;
  m_chromaFormatIDC   = chromaFormatIDC;
  m_marginX          = (bUseMargin?maxCUWidth:0) + 16;   // for 16-byte alignment
  m_marginY          = (bUseMargin?maxCUHeight:0) + 16;  // margin for 8-tap filter and infinite padding

  // assign the picture arrays and set up the ptr to the top left of the original picture
  for(unsigned int comp=0; comp<getNumberValidComponents(); comp++)
  {
    const ComponentID ch=ComponentID(comp);
    m_apiFrameBuf[comp] = (short*)xMalloc( short, getStride(ch) * getTotalHeight(ch));
    m_piFrameOrg[comp]  = m_apiFrameBuf[comp] + (m_marginY >> getComponentScaleY(ch)) * getStride(ch) + (m_marginX >> getComponentScaleX(ch));
  }
  // initialize pointers for unused components to NULL
  for(int comp=getNumberValidComponents();comp<MAX_NUM_COMPONENT; comp++)
  {
    m_apiFrameBuf[comp] = NULL;
    m_piFrameOrg[comp]  = NULL;
  }
}


void GvcFrameUnit::create ( const int picWidth,                 ///< picture width
                               const int picHeight,                ///< picture height
                               const ChromaFormat chromaFormatIDC, ///< chroma format
                               const unsigned int maxCUWidth,              ///< used for generating offsets to CUs.
                               const unsigned int maxCUHeight,             ///< used for generating offsets to CUs.
                               const bool bUseMargin)              ///< if true, then a margin of uiMaxCUWidth+16 and uiMaxCUHeight+16 is created around the image.

{
  createWithoutCUInfo(picWidth, picHeight, chromaFormatIDC, maxCUWidth, maxCUHeight, bUseMargin);

  const int numCuInWidth  = m_picWidth  / maxCUWidth  + (m_picWidth  % maxCUWidth  != 0);
  const int numCuInHeight = m_picHeight / maxCUHeight + (m_picHeight % maxCUHeight != 0);
  for(int chan=0; chan<MAX_NUM_CHANNEL_TYPE; chan++)
  {
    const ChannelType ch= ChannelType(chan);
    const int ctuHeight = maxCUHeight>>getChannelTypeScaleY(ch);
    const int ctuWidth  = maxCUWidth>>getChannelTypeScaleX(ch);
    const int stride    = getStride(ch);
  }
}

void GvcFrameUnit::destroy()
{

}

//! \}
