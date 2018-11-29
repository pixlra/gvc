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

//! \ingroup TLibCommon
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// picture class (symbol + YUV buffers)
class GvcBlockUnit;
class GvcFrameUnit
{
private:
    GvcBlockUnit**  m_apBU;                               ///< array of CU data.
    short*  m_apsFrameBuf[MAX_NUM_COMPONENT];             ///< Buffer (including margin)
    short*  m_apsFrameOrg[MAX_NUM_COMPONENT];             ///< m_apiPicBufY + m_iMarginLuma*getStride() + m_iMarginLuma
    int   m_iFrameWidth;                                  ///< Width of picture in pixels
    int   m_iFrameHeight;                                 ///< Height of picture in pixels
    int   m_iFrameWidthInBUs;
    int   m_iFrameHeightInBUs;
    int   m_iMarginX;                                     ///< margin of Luma channel (chroma's may be smaller, depending on ratio)
    int   m_iMarginY;                                     ///< margin of Luma channel (chroma's may be smaller, depending on ratio)
    int   m_iMinBUWidth;
    int   m_iMinBUHeight;
    int   m_iMaxBUWidth;
    int   m_iMaxBUHeight;
    int   m_iMaxDepth;
    int   m_iNumBUsInFrame;
    ChromaFormat m_chromaFormatIDC;                       ///< Chroma Format

public:
    GvcFrameUnit();
    virtual ~GvcFrameUnit();
    virtual void  destroy();
    void          create            (const int picWidth, const int picHeight, const ChromaFormat chromaFormatIDC, const unsigned int maxCUWidth=0, const unsigned int maxCUHeight=0, const bool bUseMargin=false);   ///< if true, then a margin of uiMaxCUWidth+16 and uiMaxCUHeight+16 is created around the image.
    GvcBlockUnit*   getBU( unsigned int buRsAddr ) { return  m_apBU[buRsAddr]; }
    int           getWidth          (const ComponentID id) const { return  m_iFrameWidth >> getComponentScaleX(id);   }
    int           getHeight         (const ComponentID id) const { return  m_iFrameHeight >> getComponentScaleY(id);  }
    int           getTotalHeight    (const ComponentID id) const { return ((m_iFrameHeight    ) + (m_iMarginY  <<1)) >> getComponentScaleY(id); } /// height + margin Y * 2
    ChromaFormat  getChromaFormat   ()                     const { return m_chromaFormatIDC; }
    int           getStride         (const ComponentID id) const { return ((m_iFrameWidth) + (m_iMarginX  <<1)) >> getComponentScaleX(id); }
    int           getMarginX        (const ComponentID id) const { return m_iMarginX >> getComponentScaleX(id);  }
    int           getMarginY        (const ComponentID id) const { return m_iMarginY >> getComponentScaleY(id);  }
    unsigned int          getComponentScaleX(const ComponentID id) const { return ::getComponentScaleX(id, m_chromaFormatIDC); }
    unsigned int          getComponentScaleY(const ComponentID id) const { return ::getComponentScaleY(id, m_chromaFormatIDC); }
    unsigned int          getChannelTypeScaleX(const ChannelType id) const { return ::getChannelTypeScaleX(id, m_chromaFormatIDC); }
    unsigned int          getChannelTypeScaleY(const ChannelType id) const { return ::getChannelTypeScaleY(id, m_chromaFormatIDC); }
    //  Access starting position of picture buffer with margin
    short*          getBuf            (const ComponentID ch)       { return  m_apsFrameBuf[ch];   }
    const short*    getBuf            (const ComponentID ch) const { return  m_apsFrameBuf[ch];   }
    //  Access starting position of original picture
    short*          getAddr           (const ComponentID ch)       { return  m_apsFrameOrg[ch];   }
    const short*    getAddr           (const ComponentID ch) const { return  m_apsFrameOrg[ch];   }
};// END CLASS DEFINITION GvcFrameUnit

//! \}

#endif // __GVCFRAMEUNIT__
