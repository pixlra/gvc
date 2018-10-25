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

#include "GvcEncoder.h"
#include "GvcEncoderCfgApp.h"
#include "TVideoIOYuv.h"
#include "GvcFrameUnitYuv.h"

/// encoder application class
class GvcEncoderApp : public GvcEncoderCfgApp
{
  private:
	// class interface
	GvcEncoder m_cGvcEnc;  ///< encoder class
	TVideoIOYuv                 m_cTVideoIOYuvInputFile;       ///< input YUV file
	TVideoIOYuv                 m_cTVideoIOYuvReconFile;       ///< output reconstruction file

	int m_iFrameRcvd;  ///< number of received frames
	unsigned int m_totalBytes;

  protected:
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

	void encode();                                 ///< main encoding function
	GvcEncoder& getGvcEnc() { return m_cGvcEnc; }  ///< return encoder class pointer reference
};

#endif  // __GVCENCODERAPP_H__
