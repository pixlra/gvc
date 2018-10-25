/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2016, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * intERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     GvcFrameUnitYuv.cpp
    \brief    picture YUV buffer class
*/

#include <cstdlib>
#include <assert.h>
#include <memory.h>
#include <malloc.h>

#include "GvcFrameUnitYuv.h"
#include "TVideoIOYuv.h"

//! \ingroup TLibCommon
//! \{

GvcFrameUnitYuv::GvcFrameUnitYuv()
{
  for(int i=0; i<MAX_NUM_COMPONENT; i++)
  {
    m_apiFrameBuf[i]    = NULL;   // Buffer (including margin)
    m_piFrameOrg[i]     = NULL;    // m_apiPicBufY + m_iMarginLuma*getStride() + m_iMarginLuma
  }

  for(int i=0; i<MAX_NUM_CHANNEL_TYPE; i++)
  {
    m_ctuOffsetInBuffer[i]=0;
    m_subCuOffsetInBuffer[i]=0;
  }
}

GvcFrameUnitYuv::~GvcFrameUnitYuv()
{
  destroy();
}

void GvcFrameUnitYuv::createWithoutCUInfo ( const int picWidth,                 ///< picture width
                                       const int picHeight,                ///< picture height
                                       const ChromaFormat chromaFormatIDC, ///< chroma format
                                       const bool bUseMargin,              ///< if true, then a margin of uiMaxCUWidth+16 and uiMaxCUHeight+16 is created around the image.
                                       const unsigned int maxCUWidth,              ///< used for margin only
                                       const unsigned int maxCUHeight)             ///< used for margin only

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

  for(int chan=0; chan<MAX_NUM_CHANNEL_TYPE; chan++)
  {
    m_ctuOffsetInBuffer[chan]   = NULL;
    m_subCuOffsetInBuffer[chan] = NULL;
  }
}



void GvcFrameUnitYuv::create ( const int picWidth,                 ///< picture width
                          const int picHeight,                ///< picture height
                          const ChromaFormat chromaFormatIDC, ///< chroma format
                          const unsigned int maxCUWidth,              ///< used for generating offsets to CUs.
                          const unsigned int maxCUHeight,             ///< used for generating offsets to CUs.
                          const unsigned int maxCUDepth,              ///< used for generating offsets to CUs.
                          const bool bUseMargin)              ///< if true, then a margin of uiMaxCUWidth+16 and uiMaxCUHeight+16 is created around the image.

{
  createWithoutCUInfo(picWidth, picHeight, chromaFormatIDC, bUseMargin, maxCUWidth, maxCUHeight);

  const int numCuInWidth  = m_picWidth  / maxCUWidth  + (m_picWidth  % maxCUWidth  != 0);
  const int numCuInHeight = m_picHeight / maxCUHeight + (m_picHeight % maxCUHeight != 0);
  for(int chan=0; chan<MAX_NUM_CHANNEL_TYPE; chan++)
  {
    const ChannelType ch= ChannelType(chan);
    const int ctuHeight = maxCUHeight>>getChannelTypeScaleY(ch);
    const int ctuWidth  = maxCUWidth>>getChannelTypeScaleX(ch);
    const int stride    = getStride(ch);

    m_ctuOffsetInBuffer[chan] = new int[numCuInWidth * numCuInHeight];

    for (int cuRow = 0; cuRow < numCuInHeight; cuRow++)
    {
      for (int cuCol = 0; cuCol < numCuInWidth; cuCol++)
      {
        m_ctuOffsetInBuffer[chan][cuRow * numCuInWidth + cuCol] = stride * cuRow * ctuHeight + cuCol * ctuWidth;
      }
    }

    m_subCuOffsetInBuffer[chan] = new int[(size_t)1 << (2 * maxCUDepth)];

    const int numSubBlockPartitions=(1<<maxCUDepth);
    const int minSubBlockHeight    =(ctuHeight >> maxCUDepth);
    const int minSubBlockWidth     =(ctuWidth  >> maxCUDepth);

    for (int buRow = 0; buRow < numSubBlockPartitions; buRow++)
    {
      for (int buCol = 0; buCol < numSubBlockPartitions; buCol++)
      {
        m_subCuOffsetInBuffer[chan][(buRow << maxCUDepth) + buCol] = stride  * buRow * minSubBlockHeight + buCol * minSubBlockWidth;
      }
    }
  }
}

void GvcFrameUnitYuv::destroy()
{
  for(int comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    m_piFrameOrg[comp] = NULL;

    if( m_apiFrameBuf[comp] )
    {
      xFree( m_apiFrameBuf[comp] );
      m_apiFrameBuf[comp] = NULL;
    }
  }

  for(int chan=0; chan<MAX_NUM_CHANNEL_TYPE; chan++)
  {
    if (m_ctuOffsetInBuffer[chan])
    {
      delete[] m_ctuOffsetInBuffer[chan];
      m_ctuOffsetInBuffer[chan] = NULL;
    }
    if (m_subCuOffsetInBuffer[chan])
    {
      delete[] m_subCuOffsetInBuffer[chan];
      m_subCuOffsetInBuffer[chan] = NULL;
    }
  }
}

void  GvcFrameUnitYuv::copyToPic (GvcFrameUnitYuv*  pcPicYuvDst) const
{
  assert( m_chromaFormatIDC == pcPicYuvDst->getChromaFormat() );

  for(unsigned int comp=0; comp<getNumberValidComponents(); comp++)
  {
    const ComponentID compId=ComponentID(comp);
    const int width     = getWidth(compId);
    const int height    = getHeight(compId);
    const int strideSrc = getStride(compId);
    assert(pcPicYuvDst->getWidth(compId) == width);
    assert(pcPicYuvDst->getHeight(compId) == height);
    if (strideSrc==pcPicYuvDst->getStride(compId))
    {
      ::memcpy ( pcPicYuvDst->getBuf(compId), getBuf(compId), sizeof(short)*strideSrc*getTotalHeight(compId));
    }
    else
    {
      const short *pSrc       = getAddr(compId);
            short *pDest      = pcPicYuvDst->getAddr(compId);
      const unsigned int strideDest = pcPicYuvDst->getStride(compId);

      for(int y=0; y<height; y++, pSrc+=strideSrc, pDest+=strideDest)
      {
        ::memcpy(pDest, pSrc, width*sizeof(short));
      }
    }
  }
}

//! \}
