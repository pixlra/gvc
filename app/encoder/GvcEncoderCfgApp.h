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
 * \file     GvcEncoderCfgApp.h
 * \brief    Main definition of the GvpEncoderCfgApp
 */

#ifndef __GVCENCODERCFGAPP__
#define __GVCENCODERCFGAPP__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "TypeDef.h"

//! \ingroup GvcEncoderApp
//! \{
class GvcEncoderCfgApp
{
  protected:
	// file I/O
	std::string m_inputFileName;      ///< source file name
	std::string m_bitstreamFileName;  ///< output bitstream file
	std::string m_reconFileName;      ///< output reconstruction file

	// source specification
	int m_iSourceWidth;   ///< source width in pixel
	int m_iSourceHeight;  ///< source height in pixel
	int m_aiPad[2];                                       ///< number of padded pixels for width and height
	int m_framesToBeEncoded;
	ChromaFormat m_InputChromaFormatIDC;
	ChromaFormat m_chromaFormatIDC;
	bool      m_bClipInputVideoToRec709Range=false; // ?
	bool      m_bClipOutputVideoToRec709Range=false; // ?
	int       m_inputBitDepth   [MAX_NUM_CHANNEL_TYPE];         ///< bit-depth of input file
	int       m_outputBitDepth  [MAX_NUM_CHANNEL_TYPE];         ///< bit-depth of output file
	int       m_MSBExtendedBitDepth[MAX_NUM_CHANNEL_TYPE];      ///< bit-depth of input samples after MSB extension
	int       m_internalBitDepth[MAX_NUM_CHANNEL_TYPE];         ///< bit-depth codec operates at (input/output files will be converted)

	// coding quality
	int m_iQP;  ///< QP value of key-picture

	// coding unit (CU) definition
	unsigned int      m_uiMaxCUWidth;                                   ///< max. CU width in pixel
	unsigned int      m_uiMaxCUHeight;                                  ///< max. CU height in pixel
	unsigned int      m_uiMaxCUDepth;                                   ///< max. CU depth (as specified by command line)
	unsigned int      m_uiMaxTotalCUDepth;                              ///< max. total CU depth - includes depth of transform-block structure

	// internal member functions
	void xCheckParameter();  ///< check validity of configuration values
	void xPrintParameter();  ///< print configuration values
	void xPrintUsage();      ///< print usage
  public:
	GvcEncoderCfgApp();
	virtual ~GvcEncoderCfgApp();
	void create();                            ///< create option handling class
	void destroy();                           ///< destroy option handling class
	bool parseCfg( int argc, char* argv[] );  ///< parse configuration file to fill member variables

};  // END CLASS DEFINITION GvcEncoderCfgApp
//! \}
#endif  // __GVCENCODERCFGAPP__