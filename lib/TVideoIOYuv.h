/* The copyright in this software is being made available under the BSD
 * License, included below. This software may be subject to other third party
 * and contributor rights, including patent rights, and no such rights are
 * granted under this license.
 *
 * Copyright (c) 2010-2016, ITU/ISO/IEC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of the ITU/ISO/IEC nor the names of its contributors may
 *    be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * intERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TVideoIOYuv.h
    \brief    YUV file I/O class (header)
*/

#ifndef __TVIDEOIOYUV__
#define __TVIDEOIOYUV__

#include <stdio.h>
#include <fstream>
#include <iostream>
#include "TypeDef.h"

class GvcFrameUnit;

using namespace std;

// ====================================================================================================================
// Class definition
// ====================================================================================================================

/// YUV file I/O class
class TVideoIOYuv
{
private:
  fstream   m_cHandle;                                      ///< file handle
  int       m_fileBitdepth[MAX_NUM_CHANNEL_TYPE]; ///< bitdepth of input/output video file
  int       m_MSBExtendedBitDepth[MAX_NUM_CHANNEL_TYPE];  ///< bitdepth after addition of MSBs (with value 0)
  int       m_bitdepthShift[MAX_NUM_CHANNEL_TYPE];  ///< number of bits to increase or decrease image by before/after write/read

public:
  TVideoIOYuv()           {}
  virtual ~TVideoIOYuv()  {}

  void  open  ( const std::string &fileName, bool bWriteMode, const int fileBitDepth[MAX_NUM_CHANNEL_TYPE], const int MSBExtendedBitDepth[MAX_NUM_CHANNEL_TYPE], const int internalBitDepth[MAX_NUM_CHANNEL_TYPE] ); ///< open or create file
  void  close ();                                           ///< close file

  void skipFrames(unsigned int numFrames, unsigned int width, unsigned int height, ChromaFormat format);

  // If fileFormat=NUM_CHROMA_FORMAT, use the format defined by pPicYuvTrueOrg
  bool  read  ( GvcFrameUnit* pPicYuv, GvcFrameUnit* pPicYuvTrueOrg, const InputColourSpaceConversion ipcsc, int aiPad[2], ChromaFormat fileFormat=NUM_CHROMA_FORMAT, const bool bClipToRec709=false );     ///< read one frame with padding parameter

  // If fileFormat=NUM_CHROMA_FORMAT, use the format defined by pPicYuv
  bool  write ( GvcFrameUnit* pPicYuv, const InputColourSpaceConversion ipCSC, int confLeft=0, int confRight=0, int confTop=0, int confBottom=0, ChromaFormat fileFormat=NUM_CHROMA_FORMAT, const bool bClipToRec709=false );     ///< write one YUV frame with padding parameter

  // If fileFormat=NUM_CHROMA_FORMAT, use the format defined by pPicYuvTop and pPicYuvBottom
  bool  write ( GvcFrameUnit* pPicYuvTop, GvcFrameUnit* pPicYuvBottom, const InputColourSpaceConversion ipCSC, int confLeft=0, int confRight=0, int confTop=0, int confBottom=0, ChromaFormat fileFormat=NUM_CHROMA_FORMAT, const bool isTff=false, const bool bClipToRec709=false);
  static void ColourSpaceConvert(const GvcFrameUnit &src, GvcFrameUnit &dest, const InputColourSpaceConversion conversion, bool bIsForwards);

  bool  isEof ();                                           ///< check for end-of-file
  bool  isFail();                                           ///< check for failure


};

#endif // __TVIDEOIOYUV__

