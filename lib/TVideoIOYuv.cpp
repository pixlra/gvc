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
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file     TVideoIOYuv.cpp
    \brief    YUV file I/O class
*/

#include <cstdlib>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <memory.h>

#include "GvcFrameUnit.h"
#include "TVideoIOYuv.h"
#include "TypeDef.h"

using namespace std;

// ====================================================================================================================
// Local Functions
// ====================================================================================================================

/**
 * Scale all pixels in img depending upon sign of shiftbits by a factor of
 * 2<sup>shiftbits</sup>.
 *
 * @param img        pointer to image to be transformed
 * @param stride  distance between vertically adjacent pixels of img.
 * @param width   width of active area in img.
 * @param height  height of active area in img.
 * @param shiftbits if zero, no operation performed
 *                  if > 0, multiply by 2<sup>shiftbits</sup>, see scalePlane()
 *                  if < 0, divide and round by 2<sup>shiftbits</sup> and clip,
 *                          see invScalePlane().
 * @param minval  minimum clipping value when dividing.
 * @param maxval  maximum clipping value when dividing.
 */
static void scalePlane(short* img, const unsigned int stride, const unsigned int width, const unsigned int height, int shiftbits, short minval, short maxval)
{
  if (shiftbits > 0)
  {
    for (unsigned int y = 0; y < height; y++, img+=stride)
    {
      for (unsigned int x = 0; x < width; x++)
      {
        img[x] <<= shiftbits;
      }
    }
  }
  else if (shiftbits < 0)
  {
    shiftbits=-shiftbits;

    short rounding = 1 << (shiftbits-1);
    for (unsigned int y = 0; y < height; y++, img+=stride)
    {
      for (unsigned int x = 0; x < width; x++)
      {
        img[x] = Clip3(minval, maxval, short((img[x] + rounding) >> shiftbits));
      }
    }
  }
}

static void
copyPlane(const GvcFrameUnit &src, const ComponentID srcPlane, GvcFrameUnit &dest, const ComponentID destPlane);

// ====================================================================================================================
// Public member functions
// ====================================================================================================================

/**
 * Open file for reading/writing Y'CbCr frames.
 *
 * Frames read/written have bitdepth fileBitDepth, and are automatically
 * formatted as 8 or 16 bit word values (see TVideoIOYuv::write()).
 *
 * Image data read or written is converted to/from internalBitDepth
 * (See scalePlane(), TVideoIOYuv::read() and TVideoIOYuv::write() for
 * further details).
 *
 * \param pchFile          file name string
 * \param bWriteMode       file open mode: true=write, false=read
 * \param fileBitDepth     bit-depth array of input/output file data.
 * \param MSBExtendedBitDepth
 * \param internalBitDepth bit-depth array to scale image data to/from when reading/writing.
 */
void TVideoIOYuv::open( const std::string &fileName, bool bWriteMode, const int fileBitDepth[MAX_NUM_CHANNEL_TYPE], const int MSBExtendedBitDepth[MAX_NUM_CHANNEL_TYPE], const int internalBitDepth[MAX_NUM_CHANNEL_TYPE] )
{
  //NOTE: files cannot have bit depth greater than 16
  for(unsigned int ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
  {
    m_fileBitdepth       [ch] = std::min<unsigned int>(fileBitDepth[ch], 16);
    m_MSBExtendedBitDepth[ch] = MSBExtendedBitDepth[ch];
    m_bitdepthShift      [ch] = internalBitDepth[ch] - m_MSBExtendedBitDepth[ch];

    if (m_fileBitdepth[ch] > 16)
    {
      if (bWriteMode)
      {
        std::cerr << "\nWARNING: Cannot write a yuv file of bit depth greater than 16 - output will be right-shifted down to 16-bit precision\n" << std::endl;
      }
      else
      {
        std::cerr << "\nERROR: Cannot read a yuv file of bit depth greater than 16\n" << std::endl;
        exit(0);
      }
    }
  }

  if ( bWriteMode )
  {
    m_cHandle.open( fileName.c_str(), ios::binary | ios::out );

    if( m_cHandle.fail() )
    {
      printf("\nfailed to write reconstructed YUV file\n");
      exit(0);
    }
  }
  else
  {
    m_cHandle.open( fileName.c_str(), ios::binary | ios::in );

    if( m_cHandle.fail() )
    {
      printf("\nfailed to open Input YUV file\n");
      exit(0);
    }
  }

  return;
}

void TVideoIOYuv::close()
{
  m_cHandle.close();
}

bool TVideoIOYuv::isEof()
{
  return m_cHandle.eof();
}

bool TVideoIOYuv::isFail()
{
  return m_cHandle.fail();
}

/**
 * Skip numFrames in input.
 *
 * This function correctly handles cases where the input file is not
 * seekable, by consuming bytes.
 */
void TVideoIOYuv::skipFrames(unsigned int numFrames, unsigned int width, unsigned int height, ChromaFormat format)
{
  if (!numFrames)
  {
    return;
  }

  //------------------
  //set the frame size according to the chroma format
  streamoff frameSize = 0;
  unsigned int wordsize=1; // default to 8-bit, unless a channel with more than 8-bits is detected.
  for (unsigned int component = 0; component < getNumberValidComponents(format); component++)
  {
    ComponentID compID=ComponentID(component);
    frameSize += (width >> getComponentScaleX(compID, format)) * (height >> getComponentScaleY(compID, format));
    if (m_fileBitdepth[toChannelType(compID)] > 8)
    {
      wordsize=2;
    }
  }
  frameSize *= wordsize;
  //------------------

  const streamoff offset = frameSize * numFrames;

  /* attempt to seek */
  if (!!m_cHandle.seekg(offset, ios::cur))
  {
    return; /* success */
  }
  m_cHandle.clear();

  /* fall back to consuming the input */
  char buf[512];
  const streamoff offset_mod_bufsize = offset % sizeof(buf);
  for (streamoff i = 0; i < offset - offset_mod_bufsize; i += sizeof(buf))
  {
    m_cHandle.read(buf, sizeof(buf));
  }
  m_cHandle.read(buf, offset_mod_bufsize);
}

/**
 * Read width*height pixels from fd into dst, optionally
 * padding the left and right edges by edge-extension.  Input may be
 * either 8bit or 16bit little-endian lsb-aligned words.
 *
 * @param dst          destination image plane
 * @param fd           input file stream
 * @param is16bit      true if input file carries > 8bit data, false otherwise.
 * @param stride444    distance between vertically adjacent pixels of dst.
 * @param width444     width of active area in dst.
 * @param height444    height of active area in dst.
 * @param pad_x444     length of horizontal padding.
 * @param pad_y444     length of vertical padding.
 * @param compID       chroma component
 * @param destFormat   chroma format of image
 * @param fileFormat   chroma format of file
 * @param fileBitDepth component bit depth in file
 * @return true for success, false in case of error
 */
static bool readPlane(short* dst,
                      istream& fd,
                      bool is16bit,
                      unsigned int stride444,
                      unsigned int width444,
                      unsigned int height444,
                      unsigned int pad_x444,
                      unsigned int pad_y444,
                      const ComponentID compID,
                      const ChromaFormat destFormat,
                      const ChromaFormat fileFormat,
                      const unsigned int fileBitDepth)
{
  const unsigned int csx_file =getComponentScaleX(compID, fileFormat);
  const unsigned int csy_file =getComponentScaleY(compID, fileFormat);
  const unsigned int csx_dest =getComponentScaleX(compID, destFormat);
  const unsigned int csy_dest =getComponentScaleY(compID, destFormat);

  const unsigned int width_dest       = width444 >>csx_dest;
  const unsigned int height_dest      = height444>>csy_dest;
  const unsigned int pad_x_dest       = pad_x444>>csx_dest;
  const unsigned int pad_y_dest       = pad_y444>>csy_dest;
  const unsigned int stride_dest      = stride444>>csx_dest;

  const unsigned int full_width_dest  = width_dest+pad_x_dest;
  const unsigned int full_height_dest = height_dest+pad_y_dest;

  const unsigned int stride_file      = (width444 * (is16bit ? 2 : 1)) >> csx_file;
  std::vector<unsigned char> bufVec(stride_file);
  unsigned char *buf=&(bufVec[0]);

  if (compID!=COMPONENT_Y && (fileFormat==CHROMA_400 || destFormat==CHROMA_400))
  {
    if (destFormat!=CHROMA_400)
    {
      // set chrominance data to mid-range: (1<<(fileBitDepth-1))
      const short value=short(1<<(fileBitDepth-1));
      for (unsigned int y = 0; y < full_height_dest; y++, dst+=stride_dest)
      {
        for (unsigned int x = 0; x < full_width_dest; x++)
        {
          dst[x] = value;
        }
      }
    }

    if (fileFormat!=CHROMA_400)
    {
      const unsigned int height_file      = height444>>csy_file;
      fd.seekg(height_file*stride_file, ios::cur);
      if (fd.eof() || fd.fail() )
      {
        return false;
      }
    }
  }
  else
  {
    const unsigned int mask_y_file=(1<<csy_file)-1;
    const unsigned int mask_y_dest=(1<<csy_dest)-1;
    for(unsigned int y444=0; y444<height444; y444++)
    {
      if ((y444&mask_y_file)==0)
      {
        // read a new line
        fd.read(reinterpret_cast<char*>(buf), stride_file);
        if (fd.eof() || fd.fail() )
        {
          return false;
        }
      }

      if ((y444&mask_y_dest)==0)
      {
        // process current destination line
        if (csx_file < csx_dest)
        {
          // eg file is 444, dest is 422.
          const unsigned int sx=csx_dest-csx_file;
          if (!is16bit)
          {
            for (unsigned int x = 0; x < width_dest; x++)
            {
              dst[x] = buf[x<<sx];
            }
          }
          else
          {
            for (unsigned int x = 0; x < width_dest; x++)
            {
              dst[x] = short(buf[(x<<sx)*2+0]) | (short(buf[(x<<sx)*2+1])<<8);
            }
          }
        }
        else
        {
          // eg file is 422, dest is 444.
          const unsigned int sx=csx_file-csx_dest;
          if (!is16bit)
          {
            for (unsigned int x = 0; x < width_dest; x++)
            {
              dst[x] = buf[x>>sx];
            }
          }
          else
          {
            for (unsigned int x = 0; x < width_dest; x++)
            {
              dst[x] = short(buf[(x>>sx)*2+0]) | (short(buf[(x>>sx)*2+1])<<8);
            }
          }
        }

        // process right hand side padding
        const short val=dst[width_dest-1];
        for (unsigned int x = width_dest; x < full_width_dest; x++)
        {
          dst[x] = val;
        }

        dst += stride_dest;
      }
    }

    // process lower padding
    for (unsigned int y = height_dest; y < full_height_dest; y++, dst+=stride_dest)
    {
      for (unsigned int x = 0; x < full_width_dest; x++)
      {
        dst[x] = (dst - stride_dest)[x];
      }
    }
  }
  return true;
}

/**
 * Write an image plane (width444*height444 pixels) from src into output stream fd.
 *
 * @param fd         output file stream
 * @param src        source image
 * @param is16bit    true if input file carries > 8bit data, false otherwise.
 * @param stride444  distance between vertically adjacent pixels of src.
 * @param width444   width of active area in src.
 * @param height444  height of active area in src.
 * @param compID       chroma component
 * @param srcFormat    chroma format of image
 * @param fileFormat   chroma format of file
 * @param fileBitDepth component bit depth in file
 * @return true for success, false in case of error
 */
static bool writePlane(ostream& fd, short* src, bool is16bit,
                       unsigned int stride444,
                       unsigned int width444, unsigned int height444,
                       const ComponentID compID,
                       const ChromaFormat srcFormat,
                       const ChromaFormat fileFormat,
                       const unsigned int fileBitDepth)
{
  const unsigned int csx_file =getComponentScaleX(compID, fileFormat);
  const unsigned int csy_file =getComponentScaleY(compID, fileFormat);
  const unsigned int csx_src  =getComponentScaleX(compID, srcFormat);
  const unsigned int csy_src  =getComponentScaleY(compID, srcFormat);

  const unsigned int stride_src      = stride444>>csx_src;

  const unsigned int stride_file      = (width444 * (is16bit ? 2 : 1)) >> csx_file;
  const unsigned int width_file       = width444 >>csx_file;
  const unsigned int height_file      = height444>>csy_file;

  std::vector<unsigned char> bufVec(stride_file);
  unsigned char *buf=&(bufVec[0]);

  if (compID!=COMPONENT_Y && (fileFormat==CHROMA_400 || srcFormat==CHROMA_400))
  {
    if (fileFormat!=CHROMA_400)
    {
      const unsigned int value=1<<(fileBitDepth-1);

      for(unsigned int y=0; y< height_file; y++)
      {
        if (!is16bit)
        {
          unsigned char val(value);
          for (unsigned int x = 0; x < width_file; x++)
          {
            buf[x]=val;
          }
        }
        else
        {
          unsigned short val(value);
          for (unsigned int x = 0; x < width_file; x++)
          {
            buf[2*x+0]= (val>>0) & 0xff;
            buf[2*x+1]= (val>>8) & 0xff;
          }
        }

        fd.write(reinterpret_cast<const char*>(buf), stride_file);
        if (fd.eof() || fd.fail() )
        {
          return false;
        }
      }
    }
  }
  else
  {
    const unsigned int mask_y_file=(1<<csy_file)-1;
    const unsigned int mask_y_src =(1<<csy_src )-1;
    for(unsigned int y444=0; y444<height444; y444++)
    {
      if ((y444&mask_y_file)==0)
      {
        // write a new line
        if (csx_file < csx_src)
        {
          // eg file is 444, source is 422.
          const unsigned int sx=csx_src-csx_file;
          if (!is16bit)
          {
            for (unsigned int x = 0; x < width_file; x++)
            {
              buf[x] = (unsigned char)(src[x>>sx]);
            }
          }
          else
          {
            for (unsigned int x = 0; x < width_file; x++)
            {
              buf[2*x  ] = (src[x>>sx]>>0) & 0xff;
              buf[2*x+1] = (src[x>>sx]>>8) & 0xff;
            }
          }
        }
        else
        {
          // eg file is 422, src is 444.
          const unsigned int sx=csx_file-csx_src;
          if (!is16bit)
          {
            for (unsigned int x = 0; x < width_file; x++)
            {
              buf[x] = (unsigned char)(src[x<<sx]);
            }
          }
          else
          {
            for (unsigned int x = 0; x < width_file; x++)
            {
              buf[2*x  ] = (src[x<<sx]>>0) & 0xff;
              buf[2*x+1] = (src[x<<sx]>>8) & 0xff;
            }
          }
        }

        fd.write(reinterpret_cast<const char*>(buf), stride_file);
        if (fd.eof() || fd.fail() )
        {
          return false;
        }
      }

      if ((y444&mask_y_src)==0)
      {
        src += stride_src;
      }

    }
  }
  return true;
}

static bool writeField(ostream& fd, short* top, short* bottom, bool is16bit,
                       unsigned int stride444,
                       unsigned int width444, unsigned int height444,
                       const ComponentID compID,
                       const ChromaFormat srcFormat,
                       const ChromaFormat fileFormat,
                       const unsigned int fileBitDepth, const bool isTff)
{
  const unsigned int csx_file =getComponentScaleX(compID, fileFormat);
  const unsigned int csy_file =getComponentScaleY(compID, fileFormat);
  const unsigned int csx_src  =getComponentScaleX(compID, srcFormat);
  const unsigned int csy_src  =getComponentScaleY(compID, srcFormat);

  const unsigned int stride_src      = stride444>>csx_src;

  const unsigned int stride_file      = (width444 * (is16bit ? 2 : 1)) >> csx_file;
  const unsigned int width_file       = width444 >>csx_file;
  const unsigned int height_file      = height444>>csy_file;

  std::vector<unsigned char> bufVec(stride_file * 2);
  unsigned char *buf=&(bufVec[0]);

  if (compID!=COMPONENT_Y && (fileFormat==CHROMA_400 || srcFormat==CHROMA_400))
  {
    if (fileFormat!=CHROMA_400)
    {
      const unsigned int value=1<<(fileBitDepth-1);

      for(unsigned int y=0; y< height_file; y++)
      {
        for (unsigned int field = 0; field < 2; field++)
        {
          unsigned char *fieldBuffer = buf + (field * stride_file);

          if (!is16bit)
          {
            unsigned char val(value);
            for (unsigned int x = 0; x < width_file; x++)
            {
              fieldBuffer[x]=val;
            }
          }
          else
          {
            unsigned short val(value);
            for (unsigned int x = 0; x < width_file; x++)
            {
              fieldBuffer[2*x+0]= (val>>0) & 0xff;
              fieldBuffer[2*x+1]= (val>>8) & 0xff;
            }
          }
        }

        fd.write(reinterpret_cast<const char*>(buf), (stride_file * 2));
        if (fd.eof() || fd.fail() )
        {
          return false;
        }
      }
    }
  }
  else
  {
    const unsigned int mask_y_file=(1<<csy_file)-1;
    const unsigned int mask_y_src =(1<<csy_src )-1;
    for(unsigned int y444=0; y444<height444; y444++)
    {
      if ((y444&mask_y_file)==0)
      {
        for (unsigned int field = 0; field < 2; field++)
        {
          unsigned char *fieldBuffer = buf + (field * stride_file);
          short   *src         = (((field == 0) && isTff) || ((field == 1) && (!isTff))) ? top : bottom;

          // write a new line
          if (csx_file < csx_src)
          {
            // eg file is 444, source is 422.
            const unsigned int sx=csx_src-csx_file;
            if (!is16bit)
            {
              for (unsigned int x = 0; x < width_file; x++)
              {
                fieldBuffer[x] = (unsigned char)(src[x>>sx]);
              }
            }
            else
            {
              for (unsigned int x = 0; x < width_file; x++)
              {
                fieldBuffer[2*x  ] = (src[x>>sx]>>0) & 0xff;
                fieldBuffer[2*x+1] = (src[x>>sx]>>8) & 0xff;
              }
            }
          }
          else
          {
            // eg file is 422, src is 444.
            const unsigned int sx=csx_file-csx_src;
            if (!is16bit)
            {
              for (unsigned int x = 0; x < width_file; x++)
              {
                fieldBuffer[x] = (unsigned char)(src[x<<sx]);
              }
            }
            else
            {
              for (unsigned int x = 0; x < width_file; x++)
              {
                fieldBuffer[2*x  ] = (src[x<<sx]>>0) & 0xff;
                fieldBuffer[2*x+1] = (src[x<<sx]>>8) & 0xff;
              }
            }
          }
        }

        fd.write(reinterpret_cast<const char*>(buf), (stride_file * 2));
        if (fd.eof() || fd.fail() )
        {
          return false;
        }
      }

      if ((y444&mask_y_src)==0)
      {
        top    += stride_src;
        bottom += stride_src;
      }

    }
  }
  return true;
}

/**
 * Read one Y'CbCr frame, performing any required input scaling to change
 * from the bitdepth of the input file to the internal bit-depth.
 *
 * If a bit-depth reduction is required, and internalBitdepth >= 8, then
 * the input file is assumed to be ITU-R BT.601/709 compliant, and the
 * resulting data is clipped to the appropriate legal range, as if the
 * file had been provided at the lower-bitdepth compliant to Rec601/709.
 *
 * @param pPicYuvUser      input picture YUV buffer class pointer
 * @param pPicYuvTrueOrg
 * @param ipcsc
 * @param aiPad            source padding size, aiPad[0] = horizontal, aiPad[1] = vertical
 * @param format           chroma format
 * @return true for success, false in case of error
 */
bool TVideoIOYuv::read ( GvcFrameUnit*  pPicYuvUser, GvcFrameUnit* pPicYuvTrueOrg, const InputColourSpaceConversion ipcsc, int aiPad[2], ChromaFormat format, const bool bClipToRec709 )
{
  // check end-of-file
  if ( isEof() )
  {
    return false;
  }
  GvcFrameUnit *pPicYuv=pPicYuvTrueOrg;
  if (format>=NUM_CHROMA_FORMAT)
  {
    format=pPicYuv->getChromaFormat();
  }

  bool is16bit = false;

  for(unsigned int ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
  {
    if (m_fileBitdepth[ch] > 8)
    {
      is16bit=true;
    }
  }

  const unsigned int stride444      = pPicYuv->getStride(COMPONENT_Y);

  // compute actual YUV width & height excluding padding size
  const unsigned int pad_h444       = aiPad[0];
  const unsigned int pad_v444       = aiPad[1];

  const unsigned int width_full444  = pPicYuv->getWidth(COMPONENT_Y);
  const unsigned int height_full444 = pPicYuv->getHeight(COMPONENT_Y);

  const unsigned int width444       = width_full444 - pad_h444;
  const unsigned int height444      = height_full444 - pad_v444;

  for(unsigned int comp=0; comp<MAX_NUM_COMPONENT; comp++)
  {
    const ComponentID compID = ComponentID(comp);
    const ChannelType chType=toChannelType(compID);

    const int desired_bitdepth = m_MSBExtendedBitDepth[chType] + m_bitdepthShift[chType];

    const bool b709Compliance=(bClipToRec709) && (m_bitdepthShift[chType] < 0 && desired_bitdepth >= 8);     /* ITU-R BT.709 compliant clipping for converting say 10b to 8b */
    const short minval = b709Compliance? ((   1 << (desired_bitdepth - 8))   ) : 0;
    const short maxval = b709Compliance? ((0xff << (desired_bitdepth - 8)) -1) : (1 << desired_bitdepth) - 1;

    if (! readPlane(pPicYuv->getAddr(compID), m_cHandle, is16bit, stride444, width444, height444, pad_h444, pad_v444, compID, pPicYuv->getChromaFormat(), format, m_fileBitdepth[chType]))
    {
      return false;
    }

    if (compID < MAX_NUM_COMPONENT )
    {
      const unsigned int csx=getComponentScaleX(compID, pPicYuv->getChromaFormat());
      const unsigned int csy=getComponentScaleY(compID, pPicYuv->getChromaFormat());
      scalePlane(pPicYuv->getAddr(compID), stride444>>csx, width_full444>>csx, height_full444>>csy, m_bitdepthShift[chType], minval, maxval);
    }
  }

  ColourSpaceConvert(*pPicYuvTrueOrg, *pPicYuvUser, ipcsc, true);

  return true;
}

/**
 * Write one Y'CbCr frame. No bit-depth conversion is performed, pcPicYuv is
 * assumed to be at TVideoIO::m_fileBitdepth depth.
 *
 * @param pPicYuvUser      input picture YUV buffer class pointer
 * @param ipCSC
 * @param confLeft         conformance window left border
 * @param confRight        conformance window right border
 * @param confTop          conformance window top border
 * @param confBottom       conformance window bottom border
 * @param format           chroma format
 * @return true for success, false in case of error
 */
bool TVideoIOYuv::write( GvcFrameUnit* pPicYuvUser, const InputColourSpaceConversion ipCSC, int confLeft, int confRight, int confTop, int confBottom, ChromaFormat format, const bool bClipToRec709 )
{
  GvcFrameUnit cPicYuvCSCd;
  if (ipCSC!=IPCOLOURSPACE_UNCHANGED)
  {
    //cPicYuvCSCd.createWithoutCUInfo(pPicYuvUser->getWidth(COMPONENT_Y), pPicYuvUser->getHeight(COMPONENT_Y), pPicYuvUser->getChromaFormat() );
    //ColourSpaceConvert(*pPicYuvUser, cPicYuvCSCd, ipCSC, false);
  }
  GvcFrameUnit *pPicYuv=(ipCSC==IPCOLOURSPACE_UNCHANGED) ? pPicYuvUser : &cPicYuvCSCd;

  // compute actual YUV frame size excluding padding size
  bool is16bit = false;
  bool nonZeroBitDepthShift=false;

  for(unsigned int ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
  {
    if (m_fileBitdepth[ch] > 8)
    {
      is16bit=true;
    }
    if (m_bitdepthShift[ch] != 0)
    {
      nonZeroBitDepthShift=true;
    }
  }

  GvcFrameUnit *dstPicYuv = NULL;
  bool retval = true;
  if (format>=NUM_CHROMA_FORMAT)
  {
    format=pPicYuv->getChromaFormat();
  }

  if (nonZeroBitDepthShift)
  {
    /*dstPicYuv = new GvcFrameUnit;
    dstPicYuv->createWithoutCUInfo( pPicYuv->getWidth(COMPONENT_Y), pPicYuv->getHeight(COMPONENT_Y), pPicYuv->getChromaFormat() );

    for(unsigned int comp=0; comp<dstPicYuv->getNumberValidComponents(); comp++)
    {
      const ComponentID compID=ComponentID(comp);
      const ChannelType ch=toChannelType(compID);
      const bool b709Compliance = bClipToRec709 && (-m_bitdepthShift[ch] < 0 && m_MSBExtendedBitDepth[ch] >= 8);     // ITU-R BT.709 compliant clipping for converting say 10b to 8b
      const short minval = b709Compliance? ((   1 << (m_MSBExtendedBitDepth[ch] - 8))   ) : 0;
      const short maxval = b709Compliance? ((0xff << (m_MSBExtendedBitDepth[ch] - 8)) -1) : (1 << m_MSBExtendedBitDepth[ch]) - 1;

      copyPlane(*pPicYuv, compID, *dstPicYuv, compID);
      scalePlane(dstPicYuv->getAddr(compID), dstPicYuv->getStride(compID), dstPicYuv->getWidth(compID), dstPicYuv->getHeight(compID), -m_bitdepthShift[ch], minval, maxval);
    }*/
  }
  else
  {
    dstPicYuv = pPicYuv;
  }

  const int  stride444 = dstPicYuv->getStride(COMPONENT_Y);
  const unsigned int width444  = dstPicYuv->getWidth(COMPONENT_Y) - confLeft - confRight;
  const unsigned int height444 = dstPicYuv->getHeight(COMPONENT_Y) -  confTop  - confBottom;

  if ((width444 == 0) || (height444 == 0))
  {
    printf ("\nWarning: writing %d x %d luma sample output picture!", width444, height444);
  }

  for(unsigned int comp=0; retval && comp<MAX_NUM_COMPONENT; comp++)
  {
    const ComponentID compID = ComponentID(comp);
    const ChannelType ch=toChannelType(compID);
    const unsigned int csx = dstPicYuv->getComponentScaleX(compID);
    const unsigned int csy = dstPicYuv->getComponentScaleY(compID);
    const int planeOffset =  (confLeft>>csx) + (confTop>>csy) * dstPicYuv->getStride(compID);
    if (! writePlane(m_cHandle, dstPicYuv->getAddr(compID) + planeOffset, is16bit, stride444, width444, height444, compID, dstPicYuv->getChromaFormat(), format, m_fileBitdepth[ch]))
    {
      retval=false;
    }
  }

  if (nonZeroBitDepthShift)
  {
    dstPicYuv->destroy();
    delete dstPicYuv;
  }

  cPicYuvCSCd.destroy();

  return retval;
}

bool TVideoIOYuv::write( GvcFrameUnit* pPicYuvUserTop, GvcFrameUnit* pPicYuvUserBottom, const InputColourSpaceConversion ipCSC, int confLeft, int confRight, int confTop, int confBottom, ChromaFormat format, const bool isTff, const bool bClipToRec709 )
{

  GvcFrameUnit cPicYuvTopCSCd;
  GvcFrameUnit cPicYuvBottomCSCd;
  if (ipCSC!=IPCOLOURSPACE_UNCHANGED)
  {
   /* cPicYuvTopCSCd   .createWithoutCUInfo(pPicYuvUserTop   ->getWidth(COMPONENT_Y), pPicYuvUserTop   ->getHeight(COMPONENT_Y), pPicYuvUserTop   ->getChromaFormat() );
    cPicYuvBottomCSCd.createWithoutCUInfo(pPicYuvUserBottom->getWidth(COMPONENT_Y), pPicYuvUserBottom->getHeight(COMPONENT_Y), pPicYuvUserBottom->getChromaFormat() );
    ColourSpaceConvert(*pPicYuvUserTop,    cPicYuvTopCSCd,    ipCSC, false);
    ColourSpaceConvert(*pPicYuvUserBottom, cPicYuvBottomCSCd, ipCSC, false);*/
  }
  GvcFrameUnit *pPicYuvTop    = (ipCSC==IPCOLOURSPACE_UNCHANGED) ? pPicYuvUserTop    : &cPicYuvTopCSCd;
  GvcFrameUnit *pPicYuvBottom = (ipCSC==IPCOLOURSPACE_UNCHANGED) ? pPicYuvUserBottom : &cPicYuvBottomCSCd;

  bool is16bit = false;
  bool nonZeroBitDepthShift=false;

  for(unsigned int ch=0; ch<MAX_NUM_CHANNEL_TYPE; ch++)
  {
    if (m_fileBitdepth[ch] > 8)
    {
      is16bit=true;
    }
    if (m_bitdepthShift[ch] != 0)
    {
      nonZeroBitDepthShift=true;
    }
  }

  GvcFrameUnit *dstPicYuvTop    = NULL;
  GvcFrameUnit *dstPicYuvBottom = NULL;

  for (unsigned int field = 0; field < 2; field++)
  {
    GvcFrameUnit *pPicYuv = (field == 0) ? pPicYuvTop : pPicYuvBottom;

    if (format>=NUM_CHROMA_FORMAT)
    {
      format=pPicYuv->getChromaFormat();
    }

    GvcFrameUnit* &dstPicYuv = (field == 0) ? dstPicYuvTop : dstPicYuvBottom;

    if (nonZeroBitDepthShift)
    {
      /*dstPicYuv = new GvcFrameUnit;
      dstPicYuv->createWithoutCUInfo( pPicYuv->getWidth(COMPONENT_Y), pPicYuv->getHeight(COMPONENT_Y), pPicYuv->getChromaFormat() );

      for(unsigned int comp=0; comp<dstPicYuv->getNumberValidComponents(); comp++)
      {
        const ComponentID compID=ComponentID(comp);
        const ChannelType ch=toChannelType(compID);
        const bool b709Compliance=bClipToRec709 && (-m_bitdepthShift[ch] < 0 && m_MSBExtendedBitDepth[ch] >= 8);     // ITU-R BT.709 compliant clipping for converting say 10b to 8b
        const short minval = b709Compliance? ((   1 << (m_MSBExtendedBitDepth[ch] - 8))   ) : 0;
        const short maxval = b709Compliance? ((0xff << (m_MSBExtendedBitDepth[ch] - 8)) -1) : (1 << m_MSBExtendedBitDepth[ch]) - 1;

        copyPlane(*pPicYuv, compID, *dstPicYuv, compID);
        scalePlane(dstPicYuv->getAddr(compID), dstPicYuv->getStride(compID), dstPicYuv->getWidth(compID), dstPicYuv->getHeight(compID), -m_bitdepthShift[ch], minval, maxval);
      }*/
    }
    else
    {
      dstPicYuv = pPicYuv;
    }
  }

  bool retval = true;

  //assert(dstPicYuvTop->getNumberValidComponents() == dstPicYuvBottom->getNumberValidComponents());
  assert(dstPicYuvTop->getChromaFormat()          == dstPicYuvBottom->getChromaFormat()         );

  for(unsigned int comp=0; retval && MAX_NUM_COMPONENT; comp++)
  {
    const ComponentID compID = ComponentID(comp);
    const ChannelType ch=toChannelType(compID);

    assert(dstPicYuvTop->getWidth          (compID) == dstPicYuvBottom->getWidth          (compID));
    assert(dstPicYuvTop->getHeight         (compID) == dstPicYuvBottom->getHeight         (compID));
    assert(dstPicYuvTop->getComponentScaleX(compID) == dstPicYuvBottom->getComponentScaleX(compID));
    assert(dstPicYuvTop->getComponentScaleY(compID) == dstPicYuvBottom->getComponentScaleY(compID));
    assert(dstPicYuvTop->getStride         (compID) == dstPicYuvBottom->getStride         (compID));

    const unsigned int width444   = dstPicYuvTop->getWidth(COMPONENT_Y)  - (confLeft + confRight);
    const unsigned int height444  = dstPicYuvTop->getHeight(COMPONENT_Y) - (confTop + confBottom);

    if ((width444 == 0) || (height444 == 0))
    {
      printf ("\nWarning: writing %d x %d luma sample output picture!", width444, height444);
    }

    const unsigned int csx = dstPicYuvTop->getComponentScaleX(compID);
    const unsigned int csy = dstPicYuvTop->getComponentScaleY(compID);
    const int planeOffset  = (confLeft>>csx) + ( confTop>>csy) * dstPicYuvTop->getStride(compID); //offset is for entire frame - round up for top field and down for bottom field

    if (! writeField(m_cHandle,
                     (dstPicYuvTop   ->getAddr(compID) + planeOffset),
                     (dstPicYuvBottom->getAddr(compID) + planeOffset),
                     is16bit,
                     dstPicYuvTop->getStride(COMPONENT_Y),
                     width444, height444, compID, dstPicYuvTop->getChromaFormat(), format, m_fileBitdepth[ch], isTff))
    {
      retval=false;
    }
  }

  if (nonZeroBitDepthShift)
  {
    dstPicYuvTop->destroy();
    dstPicYuvBottom->destroy();
    delete dstPicYuvTop;
    delete dstPicYuvBottom;
  }

  cPicYuvTopCSCd.destroy();
  cPicYuvBottomCSCd.destroy();

  return retval;
}

static void
copyPlane(const GvcFrameUnit &src, const ComponentID srcPlane, GvcFrameUnit &dest, const ComponentID destPlane)
{
  const unsigned int width=src.getWidth(srcPlane);
  const unsigned int height=src.getHeight(srcPlane);
  assert(dest.getWidth(destPlane) == width);
  assert(dest.getHeight(destPlane) == height);
  const short *pSrc=src.getAddr(srcPlane);
  short *pDest=dest.getAddr(destPlane);
  const unsigned int strideSrc=src.getStride(srcPlane);
  const unsigned int strideDest=dest.getStride(destPlane);
  for(unsigned int y=0; y<height; y++, pSrc+=strideSrc, pDest+=strideDest)
  {
    memcpy(pDest, pSrc, width*sizeof(short));
  }
}

// static member
void TVideoIOYuv::ColourSpaceConvert(const GvcFrameUnit &src, GvcFrameUnit &dest, const InputColourSpaceConversion conversion, bool bIsForwards)
{
  const ChromaFormat  format=src.getChromaFormat();
  const unsigned int          numValidComp=MAX_NUM_COMPONENT;

  switch (conversion)
  {
    case IPCOLOURSPACE_YCbCrtoYYY:
      if (format!=CHROMA_444)
      {
        // only 444 is handled.
        assert(format==CHROMA_444);
        exit(1);
      }

      {
        for(unsigned int comp=0; comp<numValidComp; comp++)
        {
          copyPlane(src, ComponentID(bIsForwards?0:comp), dest, ComponentID(comp));
        }
      }
      break;
    case IPCOLOURSPACE_YCbCrtoYCrCb:
      {
        for(unsigned int comp=0; comp<numValidComp; comp++)
        {
          copyPlane(src, ComponentID(comp), dest, ComponentID((numValidComp-comp)%numValidComp));
        }
      }
      break;

    case IPCOLOURSPACE_RGBtoGBR:
      {
        if (format!=CHROMA_444)
        {
          // only 444 is handled.
          assert(format==CHROMA_444);
          exit(1);
        }

        // channel re-mapping
        for(unsigned int comp=0; comp<numValidComp; comp++)
        {
          const ComponentID compIDsrc=ComponentID((comp+1)%numValidComp);
          const ComponentID compIDdst=ComponentID(comp);
          copyPlane(src, bIsForwards?compIDsrc:compIDdst, dest, bIsForwards?compIDdst:compIDsrc);
        }
      }
      break;

    case IPCOLOURSPACE_UNCHANGED:
    default:
      {
        for(unsigned int comp=0; comp<numValidComp; comp++)
        {
          copyPlane(src, ComponentID(comp), dest, ComponentID(comp));
        }
      }
      break;
  }
}
