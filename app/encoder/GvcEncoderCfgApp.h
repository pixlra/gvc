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

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

//! \ingroup GvcEncoderApp
//! \{
class GvcEncoderCfgApp
{
protected:
    // file I/O
    std::string m_inputFileName;                                ///< source file name
    std::string m_bitstreamFileName;                            ///< output bitstream file
    std::string m_reconFileName;                                ///< output reconstruction file

    // source specification
    int       m_iSourceWidth;                                   ///< source width in pixel
    int       m_iSourceHeight;                                  ///< source height in pixel

    // coding quality
    int       m_iQP;                                            ///< QP value of key-picture

    // internal member functions
    void  xCheckParameter ();                                   ///< check validity of configuration values
    void  xPrintParameter ();                                   ///< print configuration values
    void  xPrintUsage     ();                                   ///< print usage
public:
    GvcEncoderCfgApp();
    virtual ~GvcEncoderCfgApp();
    void  create    ();                                         ///< create option handling class
    void  destroy   ();                                         ///< destroy option handling class
    bool  parseCfg  ( int argc, char* argv[] );                 ///< parse configuration file to fill member variables

};// END CLASS DEFINITION GvcEncoderCfgApp
//! \}
#endif // __GVCENCODERCFGAPP__