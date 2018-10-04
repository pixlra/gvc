/*    This file is a part of GVP project
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

#ifndef __GVCENCODERAPP_H__
#define __GVCENCODERAPP_H__

#include "CalypDefs.h"
#include "CalypOptions.h"

class CalypOptions;
class CalypFrame;
class CalypStream;

class GvpEncoderApp
{
public:
  GvpEncoderApp();
  ~GvpEncoderApp();

  int Open( int argc, char* argv[] );
  int Process();
  int Close();

private:
  bool m_bVerbose;

  CalypOptions m_cOptions;

  ClpString m_strInput;
  ClpString m_strOutput;
  ClpString m_strResolution;
  ClpString m_strPelFmt;
  unsigned int m_uiBitsPerPixel;
  unsigned int m_uiEndianness;
  long m_iFrames;

  CalypStream* m_pcInputStream;
};

#endif  // __GVCENCODERAPP_H__
