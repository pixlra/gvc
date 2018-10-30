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

#ifndef __GVCENCFRAME_H__
#define __GVCENCFRAME_H__

#include "TypeDef.h"
#include "GvcFrameUnitYuv.h"

/**
 * \class    GvcEncFrameUnit
 * \brief    Main GVC encoder class
 */
class GvcEncFrameUnit
{
private:
	GvcFrameUnitYuv m_picYuvPred;
	GvcFrameUnitYuv m_picYuvResi;
  public:
	GvcEncFrameUnit();
	virtual ~GvcEncFrameUnit();
	void      create              ( int iWidth, int iHeight, ChromaFormat chromaFormat, unsigned int iMaxBUWidth, unsigned int iMaxBUHeight, unsigned char uhTotalDepth );
};

#endif  // __GVCENCFRAME_H__
