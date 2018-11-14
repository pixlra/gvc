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
 * \file     GvcEncBlockUnit.cpp
 * \brief    Main GVC encoder
 */

#include "GvcEncBlockUnit.h"
#include "GvcBlockUnit.h"
#include "GvcFrameUnit.h"

GvcEncBlockUnit::GvcEncBlockUnit() {}
GvcEncBlockUnit::~GvcEncBlockUnit() {}

void GvcEncBlockUnit::create (unsigned char uhTotalDepth, unsigned int uiMaxWidth, unsigned int uiMaxHeight, ChromaFormat chromaFormat )
{
    int i;

    m_uhTotalDepth   = uhTotalDepth + 1;
    m_ppcBestBU      = new GvcBlockUnit*[m_uhTotalDepth-1];
    m_ppcTempBU      = new GvcBlockUnit*[m_uhTotalDepth-1];

    m_ppcPredYuvBest = new GvcFrameUnitYuv*[m_uhTotalDepth-1];
    m_ppcResiYuvBest = new GvcFrameUnitYuv*[m_uhTotalDepth-1];
    m_ppcRecoYuvBest = new GvcFrameUnitYuv*[m_uhTotalDepth-1];
    m_ppcPredYuvTemp = new GvcFrameUnitYuv*[m_uhTotalDepth-1];
    m_ppcResiYuvTemp = new GvcFrameUnitYuv*[m_uhTotalDepth-1];
    m_ppcRecoYuvTemp = new GvcFrameUnitYuv*[m_uhTotalDepth-1];
    m_ppcOrigYuv     = new GvcFrameUnitYuv*[m_uhTotalDepth-1];

    unsigned int uiNumPartitions;
    for( i=0 ; i<m_uhTotalDepth-1 ; i++)
    {
        uiNumPartitions = 1<<( ( m_uhTotalDepth - i - 1 )<<1 );
        unsigned int uiWidth  = uiMaxWidth  >> i;
        unsigned int uiHeight = uiMaxHeight >> i;

       // m_ppcBestBU[i] = new GvcBlockUnit; m_ppcBestBU[i]->create( chromaFormat, uiNumPartitions, uiWidth, uiHeight, false, uiMaxWidth >> (m_uhTotalDepth - 1) ); // ADD CREATE FUNC IN GVCBLOCKUNIT
       // m_ppcTempBU[i] = new GvcBlockUnit; m_ppcTempBU[i]->create( chromaFormat, uiNumPartitions, uiWidth, uiHeight, false, uiMaxWidth >> (m_uhTotalDepth - 1) ); // ADD CREATE FUNC IN GVCBLOCKUNIT

        /*m_ppcPredYuvBest[i] = new GvcFrameUnitYuv; m_ppcPredYuvBest[i]->create(uiWidth, uiHeight, chromaFormat); // ADD CREATE TCOMYUV CLASS
        m_ppcResiYuvBest[i] = new GvcFrameUnitYuv; m_ppcResiYuvBest[i]->create(uiWidth, uiHeight, chromaFormat);
        m_ppcRecoYuvBest[i] = new GvcFrameUnitYuv; m_ppcRecoYuvBest[i]->create(uiWidth, uiHeight, chromaFormat);

        m_ppcPredYuvTemp[i] = new GvcFrameUnitYuv; m_ppcPredYuvTemp[i]->create(uiWidth, uiHeight, chromaFormat);
        m_ppcResiYuvTemp[i] = new GvcFrameUnitYuv; m_ppcResiYuvTemp[i]->create(uiWidth, uiHeight, chromaFormat);
        m_ppcRecoYuvTemp[i] = new GvcFrameUnitYuv; m_ppcRecoYuvTemp[i]->create(uiWidth, uiHeight, chromaFormat);

        m_ppcOrigYuv    [i] = new GvcFrameUnitYuv; m_ppcOrigYuv    [i]->create(uiWidth, uiHeight, chromaFormat);*/
    }
 }
