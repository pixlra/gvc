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
 * \file     GvcEncoder.h
 * \brief    Main GVC encoder
 */

#ifndef __GVCENCODER_H__
#define __GVCENCODER_H__

#include "TypeDef.h"

/**
 * \class    GvcEncoder
 * \brief    Main GVC encoder class
 */
class GvcEncoder
{
	int m_iSourceWidth;
	int m_iSourceHeight;
	int m_framesToBeEncoded;
	int m_iQP;
	int m_aiPad[2];
	unsigned int m_maxCUWidth;
	unsigned int m_maxCUHeight;
	unsigned int m_maxTotalCUDepth;
	ChromaFormat m_chromaFormatIDC;
	int m_bitDepth[MAX_NUM_CHANNEL_TYPE];


  public:
	GvcEncoder();
	virtual ~GvcEncoder();
	void      setSourceWidth                  ( int   i )      { m_iSourceWidth = i; }
	int       getSourceWidth                  ()      { return  m_iSourceWidth; }
	void      setSourceHeight                 ( int   i )      { m_iSourceHeight = i; }
	int       getSourceHeight                 ()      { return  m_iSourceHeight; }
	void      setFramesToBeEncoded            ( int   i )      { m_framesToBeEncoded = i; }
	int       getFramesToBeEncoded            ()      { return  m_framesToBeEncoded; }
	void      setQP                           ( int   i )      { m_iQP = i; }
	int       getQP                           ()      { return  m_iQP; }
	void      setPad                          ( int*  iPad )      { for ( int i = 0; i < 2; i++ ) m_aiPad[i] = iPad[i]; }
	int       getPad                          ( int i )      { return  m_aiPad[i]; }
	void      setMaxCUWidth                   ( unsigned int  u )      { m_maxCUWidth  = u; }
	void      setMaxCUHeight                  ( unsigned int  u )      { m_maxCUHeight = u; }
	void      setMaxTotalCUDepth              ( unsigned int  u )      { m_maxTotalCUDepth = u; }
	void      setChromaFormatIdc              ( ChromaFormat cf ) { m_chromaFormatIDC = cf; }
	ChromaFormat  getChromaFormatIdc          ( )              { return m_chromaFormatIDC; }
	void      setBitDepth( const ChannelType chType, int internalBitDepthForChannel ) { m_bitDepth[chType] = internalBitDepthForChannel; }

};

#endif  // __GVCENCODER_H__
