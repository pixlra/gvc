//
// Created by rmonteiro on 24-10-2018.
//

#ifndef GVC_TYPEDEF_H
#define GVC_TYPEDEF_H

#include <vector>

//! \ingroup TLibCommon
//! \{

#define xMalloc( type, len )        malloc   ( sizeof(type)*(len) )
#define xFree( ptr )                free     ( ptr )
template <typename T> inline T Clip3 (const T minVal, const T maxVal, const T a) { return std::min<T> (std::max<T> (minVal, a) , maxVal); }  ///< general min/max clip

static const unsigned int   MAX_UINT =                            0xFFFFFFFFU; ///< max. value of unsigned 32-bit integer
static const int    MAX_INT =                              2147483647; ///< max. value of signed 32-bit integer
static const double MAX_DOUBLE =                             1.7e+308; ///< max. value of Double-type value
// ====================================================================================================================
// Enumeration
// ====================================================================================================================

/// chroma formats (according to semantics of chroma_format_idc)
enum ChromaFormat
{
    CHROMA_400        = 0,
    CHROMA_420        = 1,
    CHROMA_422        = 2,
    CHROMA_444        = 3,
    NUM_CHROMA_FORMAT = 4
};

enum ChannelType
{
    CHANNEL_TYPE_LUMA    = 0,
    CHANNEL_TYPE_CHROMA  = 1,
    MAX_NUM_CHANNEL_TYPE = 2
};

enum ComponentID
{
    COMPONENT_Y       = 0,
    COMPONENT_Cb      = 1,
    COMPONENT_Cr      = 2,
    MAX_NUM_COMPONENT = 3
};

/// supported partition shape
enum PartSize
{
    SIZE_2Nx2N           = 0,           ///< symmetric motion partition,  2Nx2N
    SIZE_2NxN            = 1,           ///< symmetric motion partition,  2Nx N
    SIZE_Nx2N            = 2,           ///< symmetric motion partition,   Nx2N
    SIZE_NxN             = 3,           ///< symmetric motion partition,   Nx N
    SIZE_2NxnU           = 4,           ///< asymmetric motion partition, 2Nx( N/2) + 2Nx(3N/2)
    SIZE_2NxnD           = 5,           ///< asymmetric motion partition, 2Nx(3N/2) + 2Nx( N/2)
    SIZE_nLx2N           = 6,           ///< asymmetric motion partition, ( N/2)x2N + (3N/2)x2N
    SIZE_nRx2N           = 7,           ///< asymmetric motion partition, (3N/2)x2N + ( N/2)x2N
    NUMBER_OF_PART_SIZES = 8
};

/// supported prediction type
enum PredMode
{
    MODE_INTER                 = 0,     ///< inter-prediction mode
    MODE_INTRA                 = 1,     ///< intra-prediction mode
    NUMBER_OF_PREDICTION_MODES = 2,
};


enum InputColourSpaceConversion // defined in terms of conversion prior to input of encoder.
{
    IPCOLOURSPACE_UNCHANGED               = 0,
    IPCOLOURSPACE_YCbCrtoYCrCb            = 1, // Mainly used for debug!
    IPCOLOURSPACE_YCbCrtoYYY              = 2, // Mainly used for debug!
    IPCOLOURSPACE_RGBtoGBR                = 3,
    NUMBER_INPUT_COLOUR_SPACE_CONVERSIONS = 4
};
//! \}

#endif //GVC_TYPEDEF_H
