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
 * \file     GvcEncFrame.h
 * \brief    Main GVC encoder
 */

#ifndef __GVCENCBLOCK_H__
#define __GVCENCBLOCK_H__

#include "TypeDef.h"

class GvcBlockUnit;
class GvcFrameUnitYuv;

/**
 * \class    GvcEncFrameUnit
 * \brief    Main GVC encoder class
 */
class GvcEncBlockUnit
{
private:
	GvcBlockUnit**          		m_ppcBestBU;      ///< Best CUs in each depth
	GvcBlockUnit**           		m_ppcTempBU;      ///< Temporary CUs in each depth
	unsigned char                   m_uhTotalDepth;

	GvcFrameUnitYuv**               m_ppcPredYuvBest; ///< Best Prediction Yuv for each depth
	GvcFrameUnitYuv**               m_ppcResiYuvBest; ///< Best Residual Yuv for each depth
	GvcFrameUnitYuv**               m_ppcRecoYuvBest; ///< Best Reconstruction Yuv for each depth
	GvcFrameUnitYuv**               m_ppcPredYuvTemp; ///< Temporary Prediction Yuv for each depth
	GvcFrameUnitYuv**               m_ppcResiYuvTemp; ///< Temporary Residual Yuv for each depth
	GvcFrameUnitYuv**               m_ppcRecoYuvTemp; ///< Temporary Reconstruction Yuv for each depth
	GvcFrameUnitYuv**               m_ppcOrigYuv;     ///< Original Yuv for each depth

  public:
	GvcEncBlockUnit();
	~GvcEncBlockUnit();
	void  create (unsigned char uhTotalDepth, unsigned int iMaxWidth, unsigned int iMaxHeight, ChromaFormat chromaFormat );
};

#endif  // __GVCENCBLOCK_H__
