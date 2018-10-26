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
 * \file     GvcEncoderApp.cpp
 * \brief    Main definition of the GvpEncoderApp
 */

#include "GvcEncoderApp.h"

GvcEncoderApp::GvcEncoderApp()
{
	m_iFrameRcvd = 0;
	m_totalBytes = 0;
}

GvcEncoderApp::~GvcEncoderApp()
{
}

void GvcEncoderApp::encode()
{
	// create bitstream
	std::fstream bitstreamFile(m_bitstreamFileName.c_str(), std::fstream::binary | std::fstream::out);
	if (!bitstreamFile)
	{
		fprintf(stderr, "\nfailed to open bitstream file `%s' for writing\n", m_bitstreamFileName.c_str());
		exit(EXIT_FAILURE);
	}
	// Original and Recon frame
	GvcFrameUnitYuv*       pcFrameYuvOrg = new GvcFrameUnitYuv;
	GvcFrameUnitYuv*       pcFrameYuvRec = new GvcFrameUnitYuv;
	// initialize internal class & member variables
	xInitLibCfg();
	xCreateLib();
	// main encoder loop
	int   iNumEncoded = 0;
	// allocate original YUV buffer
	pcFrameYuvOrg->create( m_iSourceWidth, m_iSourceHeight, m_chromaFormat, m_uiMaxCUWidth, m_uiMaxCUHeight, m_uiMaxCUDepth, true );
	pcFrameYuvRec->create( m_iSourceWidth, m_iSourceHeight, m_chromaFormat, m_uiMaxCUWidth, m_uiMaxCUHeight, m_uiMaxCUDepth, true );

	while ( m_iFrameRcvd != m_framesToBeEncoded )
	{
		// read input YUV file
		m_cTVideoIOYuvInputFile.read( pcFrameYuvOrg, pcFrameYuvOrg, IPCOLOURSPACE_UNCHANGED, m_aiPad, m_chromaFormat, false );

		// increase number of received frames
		m_iFrameRcvd++;

		// if end of file (which is only detected on a read failure) flush the encoder of any queued pictures

		// call encoding function for one frame
		//m_cGvcEnc.encode( bEos, flush ? 0 : pcFrameYuvOrg, &cFrameYuvTrueOrg, iNumEncoded); // TODO: Add to GvcEncoder

		// write bistream to file if necessary
		m_cTVideoIOYuvReconFile.write( pcFrameYuvRec, IPCOLOURSPACE_UNCHANGED, 0, 0, 0, 0, NUM_CHROMA_FORMAT, false  );

	}
	//m_cGvcEnc.printSummary(false); // TODO: Add to GvcEncoder
	// delete original YUV buffer
	pcFrameYuvOrg->destroy();
	delete pcFrameYuvOrg;
	pcFrameYuvOrg = NULL;
	// delete used buffers in encoder class
	//m_cGvcEnc.deletePicBuffer(); // TODO: Add to GvcEncoder
	//cFrameYuvTrueOrg.destroy();
	pcFrameYuvRec->destroy();
	delete pcFrameYuvRec;
	pcFrameYuvRec = NULL;
	// delete buffers & classes
	xDestroyLib();
	printf("Bytes written to file: %u\n", m_totalBytes);
	return;
}

void GvcEncoderApp::xInitLibCfg()
{
	m_cGvcEnc.setSourceWidth                                       ( m_iSourceWidth );
	m_cGvcEnc.setSourceHeight                                      ( m_iSourceHeight );
	m_cGvcEnc.setFramesToBeEncoded                                 ( m_framesToBeEncoded );
	m_cGvcEnc.setQP                                                ( m_iQP );
	m_cGvcEnc.setPad                                               ( m_aiPad );
	m_cGvcEnc.setChromaFormatIdc                                   ( m_chromaFormat  );
	m_cGvcEnc.setMaxCUWidth                                        ( m_uiMaxCUWidth );
	m_cGvcEnc.setMaxCUHeight                                       ( m_uiMaxCUHeight );
	m_cGvcEnc.setMaxTotalCUDepth                                   ( m_uiMaxCUDepth );

	// set internal bit-depth and constants
	for (int channelType = 0; channelType < MAX_NUM_CHANNEL_TYPE; channelType++)
	{
		m_cGvcEnc.setBitDepth((ChannelType)channelType, m_bitDepth[channelType]);
	}
}

void GvcEncoderApp::xCreateLib()
{
	// Video I/O
	int noBitDepthShift[2];
	noBitDepthShift[0] = noBitDepthShift[1] = 0;
	m_cTVideoIOYuvInputFile.open( m_inputFileName,     false, m_bitDepth, noBitDepthShift, m_bitDepth );  // read  mode
	//m_cTVideoIOYuvInputFile.skipFrames(m_FrameSkip, m_iSourceWidth - m_aiPad[0], m_iSourceHeight - m_aiPad[1], m_InputChromaFormatIDC);

	if (!m_reconFileName.empty())
	{
		m_cTVideoIOYuvReconFile.open(m_reconFileName, true, m_bitDepth, m_bitDepth, m_bitDepth);  // write mode
	}

	// Neo Decoder
	//m_cTEncTop.create();
}

void GvcEncoderApp::xDestroyLib()
{
	// Video I/O
	m_cTVideoIOYuvInputFile.close();
	m_cTVideoIOYuvReconFile.close();

	// Neo Decoder
	//m_cGvcEnc.destroy(); TODO: Add to GvcEncoder
}




