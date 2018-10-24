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

/// encoder application class
class GvcEncoderApp : public GvcEncoderCfgApp
{
  private:
	// class interface
	GvcEncoder m_cGvcEnc;  ///< encoder class
	//TVideoIOYuv                 m_cTVideoIOYuvInputFile;       ///< input YUV file
	//TVideoIOYuv                 m_cTVideoIOYuvReconFile;       ///< output reconstruction file
	//TComList<TComPicYuv*>       m_cListPicYuvRec;              ///< list of reconstruction YUV files
	int m_iFrameRcvd;  ///< number of received frames
	unsigned int m_totalBytes;

  protected:
	// initialization
	//Void  xCreateLib        ();                               ///< create files & encoder class
	//Void  xInitLibCfg       ();                               ///< initialize internal variables
	//Void  xInitLib          (Bool isFieldCoding);             ///< initialize encoder class
	//Void  xDestroyLib       ();                               ///< destroy encoder class

	/// obtain required buffers
	//Void xGetBuffer(TComPicYuv*& rpcPicYuvRec);

	/// delete allocated buffers
	//Void  xDeleteBuffer     ();

	// file I/O
	//Void xWriteOutput(std::ostream& bitstreamFile, Int iNumEncoded, const std::list<AccessUnit>& accessUnits); ///< write bitstream to file
	//Void rateStatsAccum(const AccessUnit& au, const std::vector<UInt>& stats);
	//Void printRateSummary();
	//Void printChromaFormat();

  public:
	GvcEncoderApp();
	virtual ~GvcEncoderApp();

	void encode();                                 ///< main encoding function
	GvcEncoder& getGvcEnc() { return m_cGvcEnc; }  ///< return encoder class pointer reference
};

#endif  // __GVCENCODERAPP_H__
