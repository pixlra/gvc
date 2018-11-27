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
#include "GvcFrameUnitYuv.h"

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
  GvcFrameUnitYuv*           m_apcFrameYuv[NUM_FRAME_YUV]; // Original and Recon
  GvcFrameUnitYuv*           m_pcFrameYuvPred;           // Prediction
  GvcFrameUnitYuv*           m_pcFrameYuvResi;           // Residual
    unsigned int          m_frameWidthInCtus;
    unsigned int          m_frameHeightInCtus;

    unsigned int          m_uiMinCUWidth;
    unsigned int          m_uiMinCUHeight;

    char                  m_uhTotalDepth;       ///< max. depth
    unsigned int          m_numPartitionsInCtu;
    unsigned int          m_numPartInCtuWidth;
    unsigned int          m_numPartInCtuHeight;
    unsigned int          m_numCtusInFrame;

    GvcBlockUnit**  m_frameBUArray;        ///< array of CU data.
public:
  GvcFrameUnit();
  virtual ~GvcFrameUnit();
  void          create(ChromaFormat chromaFormatIDC, int iWidth, int iHeight, unsigned int uiMaxCuWidth, unsigned int uiMaxCuHeight, unsigned int uiMaxDepth, bool bIsVirtual);
  virtual void  destroy();

  GvcBlockUnit*   getBU( unsigned int ctuRsAddr )           { return  m_frameBUArray[ctuRsAddr]; }

  GvcFrameUnitYuv*   getFrameYuvOrg()        { return  m_apcFrameYuv[FRAME_YUV_ORG]; }
  GvcFrameUnitYuv*   getFrameYuvRec()        { return  m_apcFrameYuv[FRAME_YUV_REC]; }
  GvcFrameUnitYuv*   getFrameYuvPred()       { return  m_pcFrameYuvPred; }
  GvcFrameUnitYuv*   getFrameYuvResi()       { return  m_pcFrameYuvResi; }
  void          setFrameYuvPred( GvcFrameUnitYuv* pcFrameYuv )       { m_pcFrameYuvPred = pcFrameYuv; }
  void          setFrameYuvResi( GvcFrameUnitYuv* pcFrameYuv )       { m_pcFrameYuvResi = pcFrameYuv; }

  unsigned int          getNumberOfCtusInFrame()      { return getNumberOfCtusInFrame(); }
  unsigned int          getNumPartInCtuWidth()        { return getNumPartInCtuWidth();   }
  unsigned int          getNumPartInCtuHeight()       { return getNumPartInCtuHeight();  }
  unsigned int          getNumPartitionsInCtu()       { return getNumPartitionsInCtu();  }
  unsigned int          getFrameWidthInCtus()         { return getFrameWidthInCtus();    }
  unsigned int          getFrameHeightInCtus()        { return getFrameHeightInCtus();   }
  unsigned int          getMinCUWidth()               { return getMinCUWidth();          }
  unsigned int          getMinCUHeight()              { return getMinCUHeight();         }

  int           getStride( ComponentID id)           { return m_apcFrameYuv[FRAME_YUV_REC]->getStride(id); }
  int           getComponentScaleX( ComponentID id)     { return m_apcFrameYuv[FRAME_YUV_REC]->getComponentScaleX(id); }
  int           getComponentScaleY( ComponentID id)     { return m_apcFrameYuv[FRAME_YUV_REC]->getComponentScaleY(id); }
  ChromaFormat  getChromaFormat()                            { return m_apcFrameYuv[FRAME_YUV_REC]->getChromaFormat(); }
  int           getNumberValidComponents()                   { return m_apcFrameYuv[FRAME_YUV_REC]->getNumberValidComponents(); }

};// END CLASS DEFINITION GvcFrameUnit

//! \}

#endif // __GVCFRAMEUNIT__
