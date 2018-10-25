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

#ifndef __TCOMCHROMAFORMAT__
#define __TCOMCHROMAFORMAT__

#include <iostream>
#include <vector>
#include <assert.h>
#include "TypeDef.h"


//======================================================================================================================
//Chroma format utility functions  =====================================================================================
//======================================================================================================================

static inline ChannelType toChannelType             (const ComponentID id)                         { return (id==COMPONENT_Y)? CHANNEL_TYPE_LUMA : CHANNEL_TYPE_CHROMA; }
static inline bool        isLuma                    (const ComponentID id)                         { return (id==COMPONENT_Y);                                          }
static inline bool        isLuma                    (const ChannelType id)                         { return (id==CHANNEL_TYPE_LUMA);                                    }
static inline bool        isChroma                  (const ComponentID id)                         { return (id!=COMPONENT_Y);                                          }
static inline bool        isChroma                  (const ChannelType id)                         { return (id!=CHANNEL_TYPE_LUMA);                                    }
static inline unsigned int        getChannelTypeScaleX      (const ChannelType id, const ChromaFormat fmt) { return (isLuma(id) || (fmt==CHROMA_444)) ? 0 : 1;                  }
static inline unsigned int        getChannelTypeScaleY      (const ChannelType id, const ChromaFormat fmt) { return (isLuma(id) || (fmt!=CHROMA_420)) ? 0 : 1;                  }
static inline unsigned int        getComponentScaleX        (const ComponentID id, const ChromaFormat fmt) { return getChannelTypeScaleX(toChannelType(id), fmt);               }
static inline unsigned int        getComponentScaleY        (const ComponentID id, const ChromaFormat fmt) { return getChannelTypeScaleY(toChannelType(id), fmt);               }
static inline unsigned int        getNumberValidChannelTypes(const ChromaFormat fmt)                       { return (fmt==CHROMA_400) ? 1 : MAX_NUM_CHANNEL_TYPE;               }
static inline unsigned int        getNumberValidComponents  (const ChromaFormat fmt)                       { return (fmt==CHROMA_400) ? 1 : MAX_NUM_COMPONENT;                  }
static inline bool        isChromaEnabled           (const ChromaFormat fmt)                       { return  fmt!=CHROMA_400;                                           }
static inline ComponentID getFirstComponentOfChannel(const ChannelType id)                         { return (isLuma(id) ? COMPONENT_Y : COMPONENT_Cb);                  }

InputColourSpaceConversion stringToInputColourSpaceConvert(const std::string &value, const bool bIsForward);
std::string getListOfColourSpaceConverts(const bool bIsForward);

//------------------------------------------------

static inline unsigned int getTotalSamples(const unsigned int width, const unsigned int height, const ChromaFormat format)
{
  const unsigned int samplesPerChannel = width * height;

  switch (format)
  {
    case CHROMA_400: return  samplesPerChannel;           break;
    case CHROMA_420: return (samplesPerChannel * 3) >> 1; break;
    case CHROMA_422: return  samplesPerChannel * 2;       break;
    case CHROMA_444: return  samplesPerChannel * 3;       break;
    default:
      std::cerr << "ERROR: Unrecognised chroma format in getTotalSamples()" << std::endl;
      exit(1);
      break;
  }

  return MAX_UINT;
}

//------------------------------------------------

static inline unsigned int getTotalBits(const unsigned int width, const unsigned int height, const ChromaFormat format, const int bitDepths[MAX_NUM_CHANNEL_TYPE])
{
  const unsigned int samplesPerChannel = width * height;

  switch (format)
  {
    case CHROMA_400: return  samplesPerChannel *  bitDepths[CHANNEL_TYPE_LUMA];                                              break;
    case CHROMA_420: return (samplesPerChannel * (bitDepths[CHANNEL_TYPE_LUMA]*2 +   bitDepths[CHANNEL_TYPE_CHROMA]) ) >> 1; break;
    case CHROMA_422: return  samplesPerChannel * (bitDepths[CHANNEL_TYPE_LUMA]   +   bitDepths[CHANNEL_TYPE_CHROMA]);        break;
    case CHROMA_444: return  samplesPerChannel * (bitDepths[CHANNEL_TYPE_LUMA]   + 2*bitDepths[CHANNEL_TYPE_CHROMA]);        break;
    default:
      std::cerr << "ERROR: Unrecognised chroma format in getTotalSamples()" << std::endl;
      exit(1);
      break;
  }

  return MAX_UINT;
}


//------------------------------------------------

// In HM, a CU only has one chroma intra prediction direction, that corresponds to the top left luma intra prediction
// even if the NxN PU split occurs when 4 sub-TUs exist for chroma.
// Use this function to allow NxN PU splitting for chroma.

static inline bool enable4ChromaPUsInintraNxNCU(const ChromaFormat chFmt)
{
  return (chFmt == CHROMA_444);
}


//------------------------------------------------

//returns the part index of the luma region that is co-located with the specified chroma region

static inline unsigned int
getChromasCorrespondingPULumaIdx(const unsigned int lumaZOrderIdxInCtu,
                                 const ChromaFormat chFmt,
                                 const int partsPerMinCU  // 1<<(2*(sps->getMaxTotalCUDepth() - sps->getLog2DiffMaxMinCodingBlockSize()))
                                 )
{
  return enable4ChromaPUsInintraNxNCU(chFmt) ? lumaZOrderIdxInCtu : lumaZOrderIdxInCtu & (~(partsPerMinCU-1));
}

//------------------------------------------------

// If chroma format is 4:2:2 and a chroma-square-sub-tu is possible for the smallest TU, then increase the depth by 1 to allow for more parts.

static inline unsigned int getMaxCUDepthOffset(const ChromaFormat chFmt, const unsigned int quadtreeTULog2MinSize)
{
  return (chFmt==CHROMA_422 && quadtreeTULog2MinSize>2) ? 1 : 0;
}

//======================================================================================================================
//intra prediction  ====================================================================================================
//======================================================================================================================

static inline bool filterintraReferenceSamples (const ChannelType chType, const ChromaFormat chFmt, const bool intraReferenceSmoothingDisabled)
{
  return (!intraReferenceSmoothingDisabled) && (isLuma(chType) || (chFmt == CHROMA_444));
}


//======================================================================================================================
//Transform and Quantisation  ==========================================================================================
//======================================================================================================================

/*static inline bool TUCompRectHasAssociatedTransformSkipFlag(const TComRectangle &rectSamples, const unsigned int transformSkipLog2MaxSize)
{
  return (rectSamples.width <= (1<<transformSkipLog2MaxSize));
}


//------------------------------------------------

static inline int getTransformShift(const int channelBitDepth, const unsigned int uiLog2TrSize, const int maxLog2TrDynamicRange)
{
  return maxLog2TrDynamicRange - channelBitDepth - uiLog2TrSize;
}


//------------------------------------------------

static inline int getScaledChromaQP(int unscaledChromaQP, const ChromaFormat chFmt)
{
  return g_aucChromaScale[chFmt][Clip3(0, (chromaQPMappingTableSize - 1), unscaledChromaQP)];
}*/


//======================================================================================================================
//Scaling lists  =======================================================================================================
//======================================================================================================================

static inline int getScalingListType(const PredMode predMode, const ComponentID compID)
{
  return ((predMode != MODE_INTER) ? 0 : MAX_NUM_COMPONENT) + compID;
}


//------------------------------------------------


//======================================================================================================================
//Context variable selection  ==========================================================================================
//======================================================================================================================

//context variable source tables

/*static const unsigned int significanceMapContextStartTable[MAX_NUM_CHANNEL_TYPE] = {FIRST_SIG_FLAG_CTX_LUMA, FIRST_SIG_FLAG_CTX_CHROMA};
static const unsigned int contextSetStartTable            [MAX_NUM_CHANNEL_TYPE] = {FIRST_CTX_SET_LUMA,      FIRST_CTX_SET_CHROMA     };
static const unsigned int CBFContextStartTable            [MAX_NUM_CHANNEL_TYPE] = {FIRST_CBF_CTX_LUMA,      FIRST_CBF_CTX_CHROMA     };


//------------------------------------------------

//Function for last-significant-coefficient context selection parameters

static inline void getLastSignificantContextParameters (const ComponentID  component,
                                                        const int          width,
                                                        const int          height,
                                                              int         &result_offsetX,
                                                              int         &result_offsetY,
                                                              int         &result_shiftX,
                                                              int         &result_shiftY)
{
  const unsigned int convertedWidth  = g_aucConvertToBit[width];
  const unsigned int convertedHeight = g_aucConvertToBit[height];

  result_offsetX = (isChroma(component)) ? 0               : ((convertedWidth  * 3) + ((convertedWidth  + 1) >> 2));
  result_offsetY = (isChroma(component)) ? 0               : ((convertedHeight * 3) + ((convertedHeight + 1) >> 2));
  result_shiftX  = (isChroma(component)) ? convertedWidth  : ((convertedWidth  + 3) >> 2);
  result_shiftY  = (isChroma(component)) ? convertedHeight : ((convertedHeight + 3) >> 2);
}

//------------------------------------------------

//Function for significance map context index offset selection

static inline unsigned int getSignificanceMapContextOffset (const ComponentID component)
{
  return significanceMapContextStartTable[toChannelType(component)];
}


//------------------------------------------------

// Function for greater-than-one map/greater-than-two map context set selection

static inline unsigned int getContextSetIndex (const ComponentID  component,
                                       const unsigned int         subsetIndex,
                                       const bool         foundACoefficientGreaterThan1)
{
  const unsigned int notFirstSubsetOffset     = (isLuma(component) && (subsetIndex > 0)) ? 2 : 0;
  const unsigned int foundAGreaterThan1Offset = foundACoefficientGreaterThan1            ? 1 : 0;

  return contextSetStartTable[toChannelType(component)] + notFirstSubsetOffset + foundAGreaterThan1Offset;
}


//------------------------------------------------

//Function for CBF context index offset

static inline unsigned int getCBFContextOffset (const ComponentID component)
{
  return CBFContextStartTable[toChannelType(component)];
}


//======================================================================================================================
//Entropy coding parameters ============================================================================================
//======================================================================================================================

void getTUEntropyCodingParameters(      TUEntropyCodingParameters &result,
                                  class TComTU                    &rTu,
                                  const ComponentID                component);

*/
//======================================================================================================================
//End  =================================================================================================================
//======================================================================================================================

#endif
