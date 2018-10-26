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
 * \file     GvcEncoderCfgApp.cpp
 * \brief    Main definition of the GvpEncoderCfgApp
 */

#include "GvcEncoderCfgApp.h"
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <limits>
#include <string>
#include "app/common/program_options_lite.h"

using namespace std;
namespace po = df::program_options_lite;

GvcEncoderCfgApp::GvcEncoderCfgApp()
{  //
}

GvcEncoderCfgApp::~GvcEncoderCfgApp()
{  //
}

void GvcEncoderCfgApp::create()
{  //
}

void GvcEncoderCfgApp::destroy()
{  //
}

bool confirmPara( bool bflag, const char* message );

bool GvcEncoderCfgApp::parseCfg( int argc, char* argv[] )
{
	bool do_help = true;
	int warnUnknowParameter = 0;
	int tmpChromaFormat = 0;
	int tmpInternalBitDepth = 0;

	po::Options opts;
	opts.addOptions()
	( "help", do_help, false, "this help text" )
	( "c", po::parseConfigFile, "configuration file name" )
	( "WarnUnknowParameter,w", warnUnknowParameter, 0, "warn for unknown configuration parameters instead of failing" )
	( "InputFile,i", m_inputFileName, string( "" ), "Original YUV input file name" )
	( "BitstreamFile,b", m_bitstreamFileName, string( "" ), "Bitstream output file name" )
	( "ReconFile,o", m_reconFileName, string( "" ), "Reconstructed YUV output file name" )
	( "SourceWidth,-wdt", m_iSourceWidth, 0, "Source picture width" )
	( "SourceHeight,-hgt", m_iSourceHeight, 0, "Source picture height" )
	( "QP,q", m_iQP, 30, "Qp value" )
	("MaxCUWidth",                                      m_uiMaxCUWidth,                                     64u)
	("MaxCUHeight",                                     m_uiMaxCUHeight,                                    64u)
	("MaxPartitionDepth,h",                             m_uiMaxCUDepth,                                      4u, "CU depth")
	("ChromaFormat",                               tmpChromaFormat,                               420, "ChromaFormat")
	("FramesToBeEncoded,f",                             m_framesToBeEncoded,                                  0, "Number of frames to be encoded (default=all)")
	("BitDepth",                                tmpInternalBitDepth,                8, "Bit-depth the codec operates at. (default:MSBExtendedBitDepth). If different to MSBExtendedBitDepth, source data will be converted");

	po::setDefaults( opts );
	po::ErrorReporter err;
	const list<const char*>& argv_unhandled = po::scanArgv( opts, argc, (const char**)argv, err );

	for( list<const char*>::const_iterator it = argv_unhandled.begin(); it != argv_unhandled.end(); it++ )
	{
		fprintf( stderr, "Unhandled argument ignored: `%s'\n", *it );
	}

	if( argc == 1 || do_help )
	{
		/* argc == 1: no options have been specified */
		po::doHelp( cout, opts );
		return false;
	}

	if( err.is_errored )
	{
		if( !warnUnknowParameter )
		{
			/* error report has already been printed on stderr */
			return false;
		}
	}

	m_chromaFormat = numberToChromaFormat(tmpChromaFormat);
	m_bitDepth[CHANNEL_TYPE_LUMA] = 8;
	m_bitDepth[CHANNEL_TYPE_CHROMA] = 8;
	m_aiPad[1] = m_aiPad[0] = 0;

	// check validity of input parameters
	xCheckParameter();

	// print-out parameters
	xPrintParameter();

	return true;
}

void GvcEncoderCfgApp::xCheckParameter()
{
	bool check_failed = false; /* abort if there is a fatal configuration problem */
#define xConfirmPara( a, b ) check_failed |= confirmPara( a, b )

	xConfirmPara( m_bitstreamFileName.empty(), "A bitstream file name must be specified (BitstreamFile)" );
	xConfirmPara( m_iQP < 0 || m_iQP > 51, "QP exceeds supported range (0 to 51)" );
	xConfirmPara( ( m_iSourceWidth % 4 ) != 0, "Resulting coded frame width must be a multiple of the minimum CU size (4)" );
	xConfirmPara( ( m_iSourceHeight % 4 ) != 0, "Resulting coded frame height must be a multiple of the minimum CU size (4)" );
	xConfirmPara( (m_uiMaxCUWidth & (m_uiMaxCUWidth - 1)) != 0, "Max CU size must be a power of 2" );
	xConfirmPara( (m_uiMaxCUHeight & (m_uiMaxCUHeight - 1)) != 0, "Max CU size must be a power of 2" );
	xConfirmPara( m_chromaFormat == NUM_CHROMA_FORMAT, "Chroma format must be 400, 420, 422 or 444" );
	xConfirmPara( m_framesToBeEncoded < 0, "Frame number must larger or equal to zero" );
	xConfirmPara( m_bitDepth[CHANNEL_TYPE_LUMA] <= 0 && m_bitDepth[CHANNEL_TYPE_LUMA] > 16, "bit depth must be between 1 and 16" );

#undef xConfirmPara
	if( check_failed )
	{
		exit( EXIT_FAILURE );
	}
}

void GvcEncoderCfgApp::xPrintParameter()
{
	printf( "\n" );
	printf( "Input          File                    : %s\n", m_inputFileName.c_str() );
	printf( "Bitstream      File                    : %s\n", m_bitstreamFileName.c_str() );
	printf( "Reconstruction File                    : %s\n", m_reconFileName.c_str() );
	printf( "Real     Format                        : %dx%d\n", m_iSourceWidth, m_iSourceHeight );
	printf( "Internal Format                        : %dx%d\n", m_iSourceWidth, m_iSourceHeight );
	printf( "QP                                     : %d\n", m_iQP );
	printf( "\n\n" );

	fflush( stdout );
}

void GvcEncoderCfgApp::xPrintUsage()
{
}

bool confirmPara( bool bflag, const char* message )
{
	if( !bflag )
	{
		return false;
	}

	printf( "Error: %s\n", message );
	return true;
}

ChromaFormat GvcEncoderCfgApp::numberToChromaFormat(int val)
{
	switch (val)
	{
		case 400: return CHROMA_400; break;
		case 420: return CHROMA_420; break;
		case 422: return CHROMA_422; break;
		case 444: return CHROMA_444; break;
		default:  return NUM_CHROMA_FORMAT;
	}
}