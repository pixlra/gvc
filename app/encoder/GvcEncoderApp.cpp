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

#include <CalypStream.h>
#include <climits>
#include <cstring>
#include <iostream>

#include "GvcEncoder.h"
#include "GvcLogger.h"

GvpEncoderApp::GvpEncoderApp() {}

GvpEncoderApp::~GvpEncoderApp() {}

int GvpEncoderApp::Open( int argc, char* argv[] )
{
   m_cOptions.addOptions()                                               /**/
      ( "verbose,q", m_bVerbose, "increase logging" )                   /**/
      ( "input,i", m_strInput, "input file" )                           /**/
      ( "output,o", m_strOutput, "output file" )                        /**/
      ( "size,s", m_strResolution, "size (WxH)" )                       /**/
      ( "pel_fmt,p", m_strPelFmt, "pixel format" )                      /**/
      ( "bits_pel", m_uiBitsPerPixel, "bits per pixel" )                /**/
      ( "endianness", m_uiEndianness, "File endianness (big, little)" ) /**/
      ( "frames,f", m_iFrames, "number of frames to encode" )           /**/
      ;

  m_bVerbose = false;
  m_strResolution = "";
  m_strPelFmt = "yuv420p";
  m_uiBitsPerPixel = 8;
  m_uiEndianness = CLP_BIG_ENDIAN;

  if( !m_cOptions.parse( argc, argv ) )
  {
    return -1;
  }
  else if( m_cOptions.hasOpt( "help" ) )
  {
    printf( "Usage: %s --input=input_file --output=output_file\n", argv[0] );
    m_cOptions.doHelp( std::cout );
    return 1;
  }

  GvcLogger::SetReportLevel( GVC_LOG_ERROR );

  if( !m_cOptions.hasOpt( "input" ) || !m_cOptions.hasOpt( "ouput" ))
  {
    LOG( GVC_LOG_ERROR ) << "No enough input parameters!";
    LOG( GVC_LOG_ERROR ) << "Usage: " << argv[0] << " --input=input_file --output=output_file";
    m_cOptions.doHelp( std::cout );
    return 1;
    return -1;
  }

  ClpString resolutionString( "" );
  ClpString fmtString( "yuv420p" );
  unsigned int uiBitPerPixel = 8;
  unsigned int uiEndianness = 0;

  m_pcInputStream = new CalypStream;
  try
  {
    if( !m_pcInputStream->open( m_strInput, resolutionString, fmtString, uiBitPerPixel, uiEndianness, 1, true ) )
    {
      LOG( GVC_LOG_ERROR ) << "Cannot open input stream " << m_strInput.c_str();
      return -1;
    }
  }
  catch( const char* msg )
  {
    LOG( GVC_LOG_ERROR ) << "Cannot open input stream << m_strInput.c_str() << with the following error: \n" << msg;
    return -1;
  }

  return 0;
}

int GvpEncoderApp::Process()
{
  return 0;
}
int GvpEncoderApp::Close()
{
  return 0;
}
