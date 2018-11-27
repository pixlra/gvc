/*    This file is a part of GVC project
 *    Copyright (C) 2018  by Ricardo Monteiro
 *                           Joao Carreira
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 * \file     GvcFrameUnit.h
 * \brief    Main GVC encoder
 */

#ifndef __GVCFRAMEUNIT__
#define __GVCFRAMEUNIT__

#include "TypeDef.h"
#include "TComChromaFormat.h"

class GvcBlockUnit;

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// picture class (symbol + YUV buffers)

class GvcFrameUnit
{
public:
  typedef enum { FRAME_YUV_ORG, FRAME_YUV_REC, NUM_FRAME_YUV} FRAME_YUV_T;
private:
    GvcBlockUnit**  m_frameBUArray;        ///< array of CU data.
    short*  m_apiFrameBuf[MAX_NUM_COMPONENT];             ///< Buffer (including margin)
    short*  m_piFrameOrg[MAX_NUM_COMPONENT];              ///< m_apiPicBufY + m_iMarginLuma*getStride() + m_iMarginLuma
    int   m_picWidth;                                 ///< Width of picture in pixels
    int   m_picHeight;                                ///< Height of picture in pixels
    ChromaFormat m_chromaFormatIDC;                   ///< Chroma Format
    int   m_marginX;                                  ///< margin of Luma channel (chroma's may be smaller, depending on ratio)
    int   m_marginY;                                  ///< margin of Luma channel (chroma's may be smaller, depending on ratio)
public:
  GvcFrameUnit();
  virtual ~GvcFrameUnit();
    void          create            (const int picWidth,
                                     const int picHeight,
                                     const ChromaFormat chromaFormatIDC,
                                     const unsigned int maxCUWidth=0,  ///< used for generating offsets to CUs.
                                     const unsigned int maxCUHeight=0, ///< used for generating offsets to CUs.
                                     const bool bUseMargin=false);   ///< if true, then a margin of uiMaxCUWidth+16 and uiMaxCUHeight+16 is created around the image.
  virtual void  destroy();

  GvcBlockUnit*   getBU( unsigned int ctuRsAddr )           { return  m_frameBUArray[ctuRsAddr]; }

    int           getWidth          (const ComponentID id) const { return  m_picWidth >> getComponentScaleX(id);   }
    int           getHeight         (const ComponentID id) const { return  m_picHeight >> getComponentScaleY(id);  }
    ChromaFormat  getChromaFormat   ()                     const { return m_chromaFormatIDC; }
    int           getStride         (const ComponentID id) const { return ((m_picWidth     ) + (m_marginX  <<1)) >> getComponentScaleX(id); }
    int           getStride         (const ChannelType id) const { return ((m_picWidth     ) + (m_marginX  <<1)) >> getChannelTypeScaleX(id); }
    int           getTotalHeight    (const ComponentID id) const { return ((m_picHeight    ) + (m_marginY  <<1)) >> getComponentScaleY(id); }
    int           getMarginX        (const ComponentID id) const { return m_marginX >> getComponentScaleX(id);  }
    int           getMarginY        (const ComponentID id) const { return m_marginY >> getComponentScaleY(id);  }
    unsigned int          getComponentScaleX(const ComponentID id) const { return ::getComponentScaleX(id, m_chromaFormatIDC); }
    unsigned int          getComponentScaleY(const ComponentID id) const { return ::getComponentScaleY(id, m_chromaFormatIDC); }
    unsigned int          getChannelTypeScaleX(const ChannelType id) const { return ::getChannelTypeScaleX(id, m_chromaFormatIDC); }
    unsigned int          getChannelTypeScaleY(const ChannelType id) const { return ::getChannelTypeScaleY(id, m_chromaFormatIDC); }

    //  Access starting position of picture buffer with margin
    short*          getBuf            (const ComponentID ch)       { return  m_apiFrameBuf[ch];   }
    const short*    getBuf            (const ComponentID ch) const { return  m_apiFrameBuf[ch];   }

    //  Access starting position of original picture
    short*          getAddr           (const ComponentID ch)       { return  m_piFrameOrg[ch];   }
    const short*    getAddr           (const ComponentID ch) const { return  m_piFrameOrg[ch];   }
};// END CLASS DEFINITION GvcFrameUnit

//! \}

#endif // __GVCFRAMEUNIT__
