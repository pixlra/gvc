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
 * \file     GvcEncoderApp.h
 * \brief    Main definition of the GvpEncoderApp
 */

#ifndef __GVCENCODERAPP_H__
#define __GVCENCODERAPP_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "GvcEncoder.h"
#include "TVideoIOYuv.h"
#include "GvcFrameUnitYuv.h"
#include "program_options_lite.h"
#include "TypeDef.h"

/// encoder application class
class GvcEncoderApp
{
  private:
	// class interface
	GvcEncoder m_cGvcEnc;  ///< encoder class
	TVideoIOYuv                 m_cTVideoIOYuvInputFile;       ///< input YUV file
	TVideoIOYuv                 m_cTVideoIOYuvReconFile;       ///< output reconstruction file

	int m_iFrameRcvd;  ///< number of received frames
	unsigned int m_totalBytes;

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
	ChromaFormat m_chromaFormat;
	int       m_bitDepth   [MAX_NUM_CHANNEL_TYPE];         ///< bit-depth of input file
	// coding quality
	int m_iQP;  ///< QP value of key-picture
	// coding unit (CU) definition
	unsigned int      m_uiMaxCUWidth;                                   ///< max. CU width in pixel
	unsigned int      m_uiMaxCUHeight;                                  ///< max. CU height in pixel
	unsigned int      m_uiMaxCUDepth;                                   ///< max. CU depth (as specified by command line)
	// internal member functions
	void xCheckParameter();  ///< check validity of configuration values
	void xPrintParameter();  ///< print configuration values
	void xPrintUsage();      ///< print usage
	bool confirmPara( bool bflag, const char* message );
	ChromaFormat numberToChromaFormat(int val);
	// initialization
	void  xCreateLib        ();                               ///< create files & encoder class
	void  xInitLibCfg       ();                               ///< initialize internal variables
	void  xInitLib          ();					              ///< initialize encoder class
	void  xDestroyLib       ();                               ///< destroy encoder class
	/// obtain required buffers
	void xGetBuffer(GvcFrameUnitYuv*& rpcFrameYuvRec);
	/// delete allocated buffers
	void  xDeleteBuffer     ();
	// file I/O
	void xWriteOutput(std::ostream& bitstreamFile, int iNumEncoded); ///< write bitstream to file
	void printRateSummary();
  public:
	GvcEncoderApp();
	virtual ~GvcEncoderApp();
	void create();                            ///< create option handling class
	void destroy();                           ///< destroy option handling class
	void encode();                                 ///< main encoding function
	GvcEncoder& getGvcEnc() { return m_cGvcEnc; }  ///< return encoder class pointer reference
	bool parseCfg( int argc, char* argv[] );  ///< parse configuration file to fill member variables
};

#endif  // __GVCENCODERAPP_H__
