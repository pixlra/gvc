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
 *    MERCHANTABILITY or FITNESS FOR A PARTIBULAR PURPOSE.  See the
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
class GvcFrameUnit;
class GvcEncoder
{
	int m_iSourceWidth;
	int m_iSourceHeight;
	int m_iFramesToBeEncoded;
	int m_iNumEncodedFrames;
	int m_iQP;
	int m_aiPad[2];
	unsigned int m_maxBUWidth;
	unsigned int m_maxBUHeight;
	unsigned int m_maxTotalBUDepth;
	ChromaFormat m_chromaFormat;
	int m_bitDepth[MAX_NUM_CHANNEL_TYPE];
    GvcFrameUnit* m_pcFrameOrg;
    GvcFrameUnit* m_pcFrameRec;

  public:
	GvcEncoder();
	virtual ~GvcEncoder();
	void      setSourceWidth                  ( int   i )      { m_iSourceWidth = i; }
	int       getSourceWidth                  ()      { return  m_iSourceWidth; }
	void      setSourceHeight                 ( int   i )      { m_iSourceHeight = i; }
	int       getSourceHeight                 ()      { return  m_iSourceHeight; }
	void      setFramesToBeEncoded            ( int   i )      { m_iFramesToBeEncoded = i; }
	int       getFramesToBeEncoded            ()      { return  m_iFramesToBeEncoded; }
    void      setNumEncodedFrames                ( int   i )      { m_iNumEncodedFrames = i; }
    int       getNumEncodedFrames                ()      { return  m_iNumEncodedFrames; }
	void      setQP                           ( int   i )      { m_iQP = i; }
	int       getQP                           ()      { return  m_iQP; }
	void      setPad                          ( int*  iPad )      { for ( int i = 0; i < 2; i++ ) m_aiPad[i] = iPad[i]; }
	int       getPad                          ( int i )      { return  m_aiPad[i]; }
	void      setMaxBUWidth                   ( unsigned int  u )      { m_maxBUWidth  = u; }
	void      setMaxBUHeight                  ( unsigned int  u )      { m_maxBUHeight = u; }
	void      setMaxTotalBUDepth              ( unsigned int  u )      { m_maxTotalBUDepth = u; }
	void      setChromaFormat                 ( ChromaFormat cf ) { m_chromaFormat = cf; }
	ChromaFormat  getChromaFormat             ( )              { return m_chromaFormat; }
	void      setBitDepth( const ChannelType chType, int internalBitDepthForChannel ) { m_bitDepth[chType] = internalBitDepthForChannel; }
    GvcFrameUnit* getFrameOrg() { return m_pcFrameOrg; }
    void setFrameOrg(GvcFrameUnit* frame) { m_pcFrameOrg = frame; }
    GvcFrameUnit* getFrameRec() { return m_pcFrameRec; }
    void setFrameRec(GvcFrameUnit* frame) { m_pcFrameRec = frame; }
	void      create();
	void      encode(GvcFrameUnit* pcFrameOrg, GvcFrameUnit* pcFrameRec);
	void      encodeFrameUnit();
	void      encodeBlockUnit();
};

#endif  // __GVCENCODER_H__
