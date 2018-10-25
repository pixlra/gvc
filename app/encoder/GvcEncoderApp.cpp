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
	std::cout << "encoder" << std::endl;

	std::fstream bitstreamFile(m_bitstreamFileName.c_str(), std::fstream::binary | std::fstream::out);
	if (!bitstreamFile)
	{
		fprintf(stderr, "\nfailed to open bitstream file `%s' for writing\n", m_bitstreamFileName.c_str());
		exit(EXIT_FAILURE);
	}

	GvcFrameUnitYuv*       pcFrameYuvOrg = new GvcFrameUnitYuv;
	GvcFrameUnitYuv*       pcFrameYuvRec = NULL;

	// initialize internal class & member variables
	xInitLibCfg();
	xCreateLib();

	// main encoder loop
	int   iNumEncoded = 0;
	bool  bEos = false;

	GvcFrameUnitYuv cFrameYuvTrueOrg;

	// allocate original YUV buffer
	pcFrameYuvOrg->create  ( m_iSourceWidth, m_iSourceHeight, m_chromaFormatIDC, m_uiMaxCUWidth, m_uiMaxCUHeight, m_uiMaxTotalCUDepth, true );
	cFrameYuvTrueOrg.create(m_iSourceWidth, m_iSourceHeight, m_chromaFormatIDC, m_uiMaxCUWidth, m_uiMaxCUHeight, m_uiMaxTotalCUDepth, true );
	pcFrameYuvRec->create( m_iSourceWidth, m_iSourceHeight, m_chromaFormatIDC, m_uiMaxCUWidth, m_uiMaxCUHeight, m_uiMaxTotalCUDepth, true );

	while ( m_iFrameRcvd != m_framesToBeEncoded )
	{
		// get buffers
	//	xGetBuffer(pcFrameYuvRec);

		// read input YUV file
		m_cTVideoIOYuvInputFile.read( pcFrameYuvOrg, &cFrameYuvTrueOrg, IPCOLOURSPACE_UNCHANGED, m_aiPad, m_InputChromaFormatIDC, m_bClipInputVideoToRec709Range );

		// increase number of received frames
		m_iFrameRcvd++;

		bool flush = 0;
		// if end of file (which is only detected on a read failure) flush the encoder of any queued pictures
		if (m_cTVideoIOYuvInputFile.isEof())
		{
			flush = true;
			bEos = true;
			m_iFrameRcvd--;
			//m_cGvcEnc.setFramesToBeEncoded(m_iFrameRcvd); // TODO: Add to new class GvcEncCfg
		}

		// call encoding function for one frame
		//m_cGvcEnc.encode( bEos, flush ? 0 : pcFrameYuvOrg, &cFrameYuvTrueOrg, iNumEncoded); // TODO: Add to GvcEncoder

		// write bistream to file if necessary
		if ( iNumEncoded > 0 )
		{
			m_cTVideoIOYuvReconFile.write( pcFrameYuvRec, IPCOLOURSPACE_UNCHANGED, 0, 0, 0, 0, NUM_CHROMA_FORMAT, m_bClipOutputVideoToRec709Range  );
		}
	}

	//m_cGvcEnc.printSummary(false); // TODO: Add to GvcEncoder
	// delete original YUV buffer
	pcFrameYuvOrg->destroy();
	delete pcFrameYuvOrg;
	pcFrameYuvOrg = NULL;

	// delete used buffers in encoder class
	//m_cGvcEnc.deletePicBuffer(); // TODO: Add to GvcEncoder
	cFrameYuvTrueOrg.destroy();

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
	/*TComVPS vps; // TODO: Complete

	vps.setMaxTLayers                                               ( m_maxTempLayer );
	if (m_maxTempLayer == 1)
	{
		vps.setTemporalNestingFlag(true);
	}
	vps.setMaxLayers                                                ( 1 );
	for(Int i = 0; i < MAX_TLAYER; i++)
	{
		vps.setNumReorderPics                                         ( m_numReorderPics[i], i );
		vps.setMaxDecPicBuffering                                     ( m_maxDecPicBuffering[i], i );
	}
	m_cTEncTop.setVPS(&vps);

	m_cTEncTop.setProfile                                           ( m_profile);
	m_cTEncTop.setLevel                                             ( m_levelTier, m_level);
	m_cTEncTop.setProgressiveSourceFlag                             ( m_progressiveSourceFlag);
	m_cTEncTop.setInterlacedSourceFlag                              ( m_interlacedSourceFlag);
	m_cTEncTop.setNonPackedConstraintFlag                           ( m_nonPackedConstraintFlag);
	m_cTEncTop.setFrameOnlyConstraintFlag                           ( m_frameOnlyConstraintFlag);
	m_cTEncTop.setBitDepthConstraintValue                           ( m_bitDepthConstraint );
	m_cTEncTop.setChromaFormatConstraintValue                       ( m_chromaFormatConstraint );
	m_cTEncTop.setIntraConstraintFlag                               ( m_intraConstraintFlag );
	m_cTEncTop.setOnePictureOnlyConstraintFlag                      ( m_onePictureOnlyConstraintFlag );
	m_cTEncTop.setLowerBitRateConstraintFlag                        ( m_lowerBitRateConstraintFlag );

	m_cTEncTop.setPrintMSEBasedSequencePSNR                         ( m_printMSEBasedSequencePSNR);
	m_cTEncTop.setPrintFrameMSE                                     ( m_printFrameMSE);
	m_cTEncTop.setPrintSequenceMSE                                  ( m_printSequenceMSE);
	m_cTEncTop.setCabacZeroWordPaddingEnabled                       ( m_cabacZeroWordPaddingEnabled );

	m_cTEncTop.setFrameRate                                         ( m_iFrameRate );
	m_cTEncTop.setFrameSkip                                         ( m_FrameSkip );
	m_cTEncTop.setTemporalSubsampleRatio                            ( m_temporalSubsampleRatio );
	m_cTEncTop.setSourceWidth                                       ( m_iSourceWidth );
	m_cTEncTop.setSourceHeight                                      ( m_iSourceHeight );
	m_cTEncTop.setConformanceWindow                                 ( m_confWinLeft, m_confWinRight, m_confWinTop, m_confWinBottom );
	m_cTEncTop.setFramesToBeEncoded                                 ( m_framesToBeEncoded );

	//====== Coding Structure ========
	m_cTEncTop.setIntraPeriod                                       ( m_iIntraPeriod );
	m_cTEncTop.setDecodingRefreshType                               ( m_iDecodingRefreshType );
	m_cTEncTop.setGOPSize                                           ( m_iGOPSize );
	m_cTEncTop.setGopList                                           ( m_GOPList );
	m_cTEncTop.setExtraRPSs                                         ( m_extraRPSs );
	for(Int i = 0; i < MAX_TLAYER; i++)
	{
		m_cTEncTop.setNumReorderPics                                  ( m_numReorderPics[i], i );
		m_cTEncTop.setMaxDecPicBuffering                              ( m_maxDecPicBuffering[i], i );
	}
	for( UInt uiLoop = 0; uiLoop < MAX_TLAYER; ++uiLoop )
	{
		m_cTEncTop.setLambdaModifier                                  ( uiLoop, m_adLambdaModifier[ uiLoop ] );
	}
	m_cTEncTop.setIntraLambdaModifier                               ( m_adIntraLambdaModifier );
	m_cTEncTop.setIntraQpFactor                                     ( m_dIntraQpFactor );

	m_cTEncTop.setQP                                                ( m_iQP );

	m_cTEncTop.setPad                                               ( m_aiPad );

	m_cTEncTop.setAccessUnitDelimiter                               ( m_AccessUnitDelimiter );

	m_cTEncTop.setMaxTempLayer                                      ( m_maxTempLayer );
	m_cTEncTop.setUseAMP( m_enableAMP );

	//===== Slice ========

	//====== Loop/Deblock Filter ========
	m_cTEncTop.setLoopFilterDisable                                 ( m_bLoopFilterDisable       );
	m_cTEncTop.setLoopFilterOffsetInPPS                             ( m_loopFilterOffsetInPPS );
	m_cTEncTop.setLoopFilterBetaOffset                              ( m_loopFilterBetaOffsetDiv2  );
	m_cTEncTop.setLoopFilterTcOffset                                ( m_loopFilterTcOffsetDiv2    );
#if W0038_DB_OPT
	m_cTEncTop.setDeblockingFilterMetric                            ( m_deblockingFilterMetric );
#else
	m_cTEncTop.setDeblockingFilterMetric                            ( m_DeblockingFilterMetric );
#endif

	//====== Motion search ========
	m_cTEncTop.setDisableIntraPUsInInterSlices                      ( m_bDisableIntraPUsInInterSlices );
	m_cTEncTop.setMotionEstimationSearchMethod                      ( m_motionEstimationSearchMethod  );
	m_cTEncTop.setSearchRange                                       ( m_iSearchRange );
	m_cTEncTop.setBipredSearchRange                                 ( m_bipredSearchRange );
	m_cTEncTop.setClipForBiPredMeEnabled                            ( m_bClipForBiPredMeEnabled );
	m_cTEncTop.setFastMEAssumingSmootherMVEnabled                   ( m_bFastMEAssumingSmootherMVEnabled );
	m_cTEncTop.setMinSearchWindow                                   ( m_minSearchWindow );
	m_cTEncTop.setRestrictMESampling                                ( m_bRestrictMESampling );

	//====== Quality control ========
	m_cTEncTop.setMaxDeltaQP                                        ( m_iMaxDeltaQP  );
	m_cTEncTop.setMaxCuDQPDepth                                     ( m_iMaxCuDQPDepth  );
	m_cTEncTop.setDiffCuChromaQpOffsetDepth                         ( m_diffCuChromaQpOffsetDepth );
	m_cTEncTop.setChromaCbQpOffset                                  ( m_cbQpOffset     );
	m_cTEncTop.setChromaCrQpOffset                                  ( m_crQpOffset  );
#if W0038_CQP_ADJ
	m_cTEncTop.setSliceChromaOffsetQpIntraOrPeriodic                ( m_sliceChromaQpOffsetPeriodicity, m_sliceChromaQpOffsetIntraOrPeriodic );
#endif
	m_cTEncTop.setChromaFormatIdc                                   ( m_chromaFormatIDC  );

#if ADAPTIVE_QP_SELECTION
	m_cTEncTop.setUseAdaptQpSelect                                  ( m_bUseAdaptQpSelect   );
#endif

	m_cTEncTop.setUseAdaptiveQP                                     ( m_bUseAdaptiveQP  );
	m_cTEncTop.setQPAdaptationRange                                 ( m_iQPAdaptationRange );
	m_cTEncTop.setExtendedPrecisionProcessingFlag                   ( m_extendedPrecisionProcessingFlag );
	m_cTEncTop.setHighPrecisionOffsetsEnabledFlag                   ( m_highPrecisionOffsetsEnabledFlag );

	m_cTEncTop.setWeightedPredictionMethod( m_weightedPredictionMethod );

	//====== Tool list ========
	m_cTEncTop.setDeltaQpRD                                         ( m_uiDeltaQpRD  );
	m_cTEncTop.setFastDeltaQp                                       ( m_bFastDeltaQP  );
	m_cTEncTop.setUseASR                                            ( m_bUseASR      );
	m_cTEncTop.setUseHADME                                          ( m_bUseHADME    );
	m_cTEncTop.setdQPs                                              ( m_aidQP        );
	m_cTEncTop.setUseRDOQ                                           ( m_useRDOQ     );
	m_cTEncTop.setUseRDOQTS                                         ( m_useRDOQTS   );
#if T0196_SELECTIVE_RDOQ
	m_cTEncTop.setUseSelectiveRDOQ                                  ( m_useSelectiveRDOQ );
#endif
	m_cTEncTop.setRDpenalty                                         ( m_rdPenalty );
	m_cTEncTop.setMaxCUWidth                                        ( m_uiMaxCUWidth );
	m_cTEncTop.setMaxCUHeight                                       ( m_uiMaxCUHeight );
	m_cTEncTop.setMaxTotalCUDepth                                   ( m_uiMaxTotalCUDepth );
	m_cTEncTop.setLog2DiffMaxMinCodingBlockSize                     ( m_uiLog2DiffMaxMinCodingBlockSize );
	m_cTEncTop.setQuadtreeTULog2MaxSize                             ( m_uiQuadtreeTULog2MaxSize );
	m_cTEncTop.setQuadtreeTULog2MinSize                             ( m_uiQuadtreeTULog2MinSize );
	m_cTEncTop.setQuadtreeTUMaxDepthInter                           ( m_uiQuadtreeTUMaxDepthInter );
	m_cTEncTop.setQuadtreeTUMaxDepthIntra                           ( m_uiQuadtreeTUMaxDepthIntra );
	m_cTEncTop.setFastInterSearchMode                               ( m_fastInterSearchMode );
	m_cTEncTop.setUseEarlyCU                                        ( m_bUseEarlyCU  );
	m_cTEncTop.setUseFastDecisionForMerge                           ( m_useFastDecisionForMerge  );
	m_cTEncTop.setUseCbfFastMode                                    ( m_bUseCbfFastMode  );
	m_cTEncTop.setUseEarlySkipDetection                             ( m_useEarlySkipDetection );
	m_cTEncTop.setCrossComponentPredictionEnabledFlag               ( m_crossComponentPredictionEnabledFlag );
	m_cTEncTop.setUseReconBasedCrossCPredictionEstimate             ( m_reconBasedCrossCPredictionEstimate );
	m_cTEncTop.setLog2SaoOffsetScale                                ( CHANNEL_TYPE_LUMA  , m_log2SaoOffsetScale[CHANNEL_TYPE_LUMA]   );
	m_cTEncTop.setLog2SaoOffsetScale                                ( CHANNEL_TYPE_CHROMA, m_log2SaoOffsetScale[CHANNEL_TYPE_CHROMA] );
	m_cTEncTop.setUseTransformSkip                                  ( m_useTransformSkip      );
	m_cTEncTop.setUseTransformSkipFast                              ( m_useTransformSkipFast  );
	m_cTEncTop.setTransformSkipRotationEnabledFlag                  ( m_transformSkipRotationEnabledFlag );
	m_cTEncTop.setTransformSkipContextEnabledFlag                   ( m_transformSkipContextEnabledFlag   );
	m_cTEncTop.setPersistentRiceAdaptationEnabledFlag               ( m_persistentRiceAdaptationEnabledFlag );
	m_cTEncTop.setCabacBypassAlignmentEnabledFlag                   ( m_cabacBypassAlignmentEnabledFlag );
	m_cTEncTop.setLog2MaxTransformSkipBlockSize                     ( m_log2MaxTransformSkipBlockSize  );
	for (UInt signallingModeIndex = 0; signallingModeIndex < NUMBER_OF_RDPCM_SIGNALLING_MODES; signallingModeIndex++)
	{
		m_cTEncTop.setRdpcmEnabledFlag                                ( RDPCMSignallingMode(signallingModeIndex), m_rdpcmEnabledFlag[signallingModeIndex]);
	}
	m_cTEncTop.setUseConstrainedIntraPred                           ( m_bUseConstrainedIntraPred );
	m_cTEncTop.setFastUDIUseMPMEnabled                              ( m_bFastUDIUseMPMEnabled );
	m_cTEncTop.setFastMEForGenBLowDelayEnabled                      ( m_bFastMEForGenBLowDelayEnabled );
	m_cTEncTop.setUseBLambdaForNonKeyLowDelayPictures               ( m_bUseBLambdaForNonKeyLowDelayPictures );
	m_cTEncTop.setPCMLog2MinSize                                    ( m_uiPCMLog2MinSize);
	m_cTEncTop.setUsePCM                                            ( m_usePCM );

	// set internal bit-depth and constants
	for (UInt channelType = 0; channelType < MAX_NUM_CHANNEL_TYPE; channelType++)
	{
		m_cTEncTop.setBitDepth((ChannelType)channelType, m_internalBitDepth[channelType]);
		m_cTEncTop.setPCMBitDepth((ChannelType)channelType, m_bPCMInputBitDepthFlag ? m_MSBExtendedBitDepth[channelType] : m_internalBitDepth[channelType]);
	}

	m_cTEncTop.setPCMLog2MaxSize                                    ( m_pcmLog2MaxSize);
	m_cTEncTop.setMaxNumMergeCand                                   ( m_maxNumMergeCand );


	//====== Weighted Prediction ========
	m_cTEncTop.setUseWP                                             ( m_useWeightedPred     );
	m_cTEncTop.setWPBiPred                                          ( m_useWeightedBiPred   );

	//====== Parallel Merge Estimation ========
	m_cTEncTop.setLog2ParallelMergeLevelMinus2                      ( m_log2ParallelMergeLevel - 2 );

	//====== Slice ========
	m_cTEncTop.setSliceMode                                         ( m_sliceMode );
	m_cTEncTop.setSliceArgument                                     ( m_sliceArgument );

	//====== Dependent Slice ========
	m_cTEncTop.setSliceSegmentMode                                  ( m_sliceSegmentMode );
	m_cTEncTop.setSliceSegmentArgument                              ( m_sliceSegmentArgument );

	if(m_sliceMode == NO_SLICES )
	{
		m_bLFCrossSliceBoundaryFlag = true;
	}
	m_cTEncTop.setLFCrossSliceBoundaryFlag                          ( m_bLFCrossSliceBoundaryFlag );
	m_cTEncTop.setUseSAO                                            ( m_bUseSAO );
	m_cTEncTop.setTestSAODisableAtPictureLevel                      ( m_bTestSAODisableAtPictureLevel );
	m_cTEncTop.setSaoEncodingRate                                   ( m_saoEncodingRate );
	m_cTEncTop.setSaoEncodingRateChroma                             ( m_saoEncodingRateChroma );
	m_cTEncTop.setMaxNumOffsetsPerPic                               ( m_maxNumOffsetsPerPic);

	m_cTEncTop.setSaoCtuBoundary                                    ( m_saoCtuBoundary);
#if OPTIONAL_RESET_SAO_ENCODING_AFTER_IRAP
	m_cTEncTop.setSaoResetEncoderStateAfterIRAP                     ( m_saoResetEncoderStateAfterIRAP);
#endif
	m_cTEncTop.setPCMInputBitDepthFlag                              ( m_bPCMInputBitDepthFlag);
	m_cTEncTop.setPCMFilterDisableFlag                              ( m_bPCMFilterDisableFlag);

	m_cTEncTop.setIntraSmoothingDisabledFlag                        (!m_enableIntraReferenceSmoothing );
	m_cTEncTop.setDecodedPictureHashSEIType                         ( m_decodedPictureHashSEIType );
	m_cTEncTop.setRecoveryPointSEIEnabled                           ( m_recoveryPointSEIEnabled );
	m_cTEncTop.setBufferingPeriodSEIEnabled                         ( m_bufferingPeriodSEIEnabled );
	m_cTEncTop.setPictureTimingSEIEnabled                           ( m_pictureTimingSEIEnabled );
	m_cTEncTop.setToneMappingInfoSEIEnabled                         ( m_toneMappingInfoSEIEnabled );
	m_cTEncTop.setTMISEIToneMapId                                   ( m_toneMapId );
	m_cTEncTop.setTMISEIToneMapCancelFlag                           ( m_toneMapCancelFlag );
	m_cTEncTop.setTMISEIToneMapPersistenceFlag                      ( m_toneMapPersistenceFlag );
	m_cTEncTop.setTMISEICodedDataBitDepth                           ( m_toneMapCodedDataBitDepth );
	m_cTEncTop.setTMISEITargetBitDepth                              ( m_toneMapTargetBitDepth );
	m_cTEncTop.setTMISEIModelID                                     ( m_toneMapModelId );
	m_cTEncTop.setTMISEIMinValue                                    ( m_toneMapMinValue );
	m_cTEncTop.setTMISEIMaxValue                                    ( m_toneMapMaxValue );
	m_cTEncTop.setTMISEISigmoidMidpoint                             ( m_sigmoidMidpoint );
	m_cTEncTop.setTMISEISigmoidWidth                                ( m_sigmoidWidth );
	m_cTEncTop.setTMISEIStartOfCodedInterva                         ( m_startOfCodedInterval );
	m_cTEncTop.setTMISEINumPivots                                   ( m_numPivots );
	m_cTEncTop.setTMISEICodedPivotValue                             ( m_codedPivotValue );
	m_cTEncTop.setTMISEITargetPivotValue                            ( m_targetPivotValue );
	m_cTEncTop.setTMISEICameraIsoSpeedIdc                           ( m_cameraIsoSpeedIdc );
	m_cTEncTop.setTMISEICameraIsoSpeedValue                         ( m_cameraIsoSpeedValue );
	m_cTEncTop.setTMISEIExposureIndexIdc                            ( m_exposureIndexIdc );
	m_cTEncTop.setTMISEIExposureIndexValue                          ( m_exposureIndexValue );
	m_cTEncTop.setTMISEIExposureCompensationValueSignFlag           ( m_exposureCompensationValueSignFlag );
	m_cTEncTop.setTMISEIExposureCompensationValueNumerator          ( m_exposureCompensationValueNumerator );
	m_cTEncTop.setTMISEIExposureCompensationValueDenomIdc           ( m_exposureCompensationValueDenomIdc );
	m_cTEncTop.setTMISEIRefScreenLuminanceWhite                     ( m_refScreenLuminanceWhite );
	m_cTEncTop.setTMISEIExtendedRangeWhiteLevel                     ( m_extendedRangeWhiteLevel );
	m_cTEncTop.setTMISEINominalBlackLevelLumaCodeValue              ( m_nominalBlackLevelLumaCodeValue );
	m_cTEncTop.setTMISEINominalWhiteLevelLumaCodeValue              ( m_nominalWhiteLevelLumaCodeValue );
	m_cTEncTop.setTMISEIExtendedWhiteLevelLumaCodeValue             ( m_extendedWhiteLevelLumaCodeValue );
	m_cTEncTop.setChromaResamplingFilterHintEnabled                 ( m_chromaResamplingFilterSEIenabled );
	m_cTEncTop.setChromaResamplingHorFilterIdc                      ( m_chromaResamplingHorFilterIdc );
	m_cTEncTop.setChromaResamplingVerFilterIdc                      ( m_chromaResamplingVerFilterIdc );
	m_cTEncTop.setFramePackingArrangementSEIEnabled                 ( m_framePackingSEIEnabled );
	m_cTEncTop.setFramePackingArrangementSEIType                    ( m_framePackingSEIType );
	m_cTEncTop.setFramePackingArrangementSEIId                      ( m_framePackingSEIId );
	m_cTEncTop.setFramePackingArrangementSEIQuincunx                ( m_framePackingSEIQuincunx );
	m_cTEncTop.setFramePackingArrangementSEIInterpretation          ( m_framePackingSEIInterpretation );
	m_cTEncTop.setSegmentedRectFramePackingArrangementSEIEnabled    ( m_segmentedRectFramePackingSEIEnabled );
	m_cTEncTop.setSegmentedRectFramePackingArrangementSEICancel     ( m_segmentedRectFramePackingSEICancel );
	m_cTEncTop.setSegmentedRectFramePackingArrangementSEIType       ( m_segmentedRectFramePackingSEIType );
	m_cTEncTop.setSegmentedRectFramePackingArrangementSEIPersistence( m_segmentedRectFramePackingSEIPersistence );
	m_cTEncTop.setDisplayOrientationSEIAngle                        ( m_displayOrientationSEIAngle );
	m_cTEncTop.setTemporalLevel0IndexSEIEnabled                     ( m_temporalLevel0IndexSEIEnabled );
	m_cTEncTop.setGradualDecodingRefreshInfoEnabled                 ( m_gradualDecodingRefreshInfoEnabled );
	m_cTEncTop.setNoDisplaySEITLayer                                ( m_noDisplaySEITLayer );
	m_cTEncTop.setDecodingUnitInfoSEIEnabled                        ( m_decodingUnitInfoSEIEnabled );
	m_cTEncTop.setSOPDescriptionSEIEnabled                          ( m_SOPDescriptionSEIEnabled );
	m_cTEncTop.setScalableNestingSEIEnabled                         ( m_scalableNestingSEIEnabled );
	m_cTEncTop.setTMCTSSEIEnabled                                   ( m_tmctsSEIEnabled );
	m_cTEncTop.setTimeCodeSEIEnabled                                ( m_timeCodeSEIEnabled );
	m_cTEncTop.setNumberOfTimeSets                                  ( m_timeCodeSEINumTs );
	for(Int i = 0; i < m_timeCodeSEINumTs; i++)
	{
		m_cTEncTop.setTimeSet(m_timeSetArray[i], i);
	}
	m_cTEncTop.setKneeSEIEnabled                                    ( m_kneeSEIEnabled );
	m_cTEncTop.setKneeSEIId                                         ( m_kneeSEIId );
	m_cTEncTop.setKneeSEICancelFlag                                 ( m_kneeSEICancelFlag );
	m_cTEncTop.setKneeSEIPersistenceFlag                            ( m_kneeSEIPersistenceFlag );
	m_cTEncTop.setKneeSEIInputDrange                                ( m_kneeSEIInputDrange );
	m_cTEncTop.setKneeSEIInputDispLuminance                         ( m_kneeSEIInputDispLuminance );
	m_cTEncTop.setKneeSEIOutputDrange                               ( m_kneeSEIOutputDrange );
	m_cTEncTop.setKneeSEIOutputDispLuminance                        ( m_kneeSEIOutputDispLuminance );
	m_cTEncTop.setKneeSEINumKneePointsMinus1                        ( m_kneeSEINumKneePointsMinus1 );
	m_cTEncTop.setKneeSEIInputKneePoint                             ( m_kneeSEIInputKneePoint );
	m_cTEncTop.setKneeSEIOutputKneePoint                            ( m_kneeSEIOutputKneePoint );
	m_cTEncTop.setColourRemapInfoSEIFileRoot                        ( m_colourRemapSEIFileRoot );
	m_cTEncTop.setMasteringDisplaySEI                               ( m_masteringDisplay );
#if U0033_ALTERNATIVE_TRANSFER_CHARACTERISTICS_SEI
	m_cTEncTop.setSEIAlternativeTransferCharacteristicsSEIEnable    ( m_preferredTransferCharacteristics>=0     );
  m_cTEncTop.setSEIPreferredTransferCharacteristics               ( UChar(m_preferredTransferCharacteristics) );
#endif

	m_cTEncTop.setTileUniformSpacingFlag                            ( m_tileUniformSpacingFlag );
	m_cTEncTop.setNumColumnsMinus1                                  ( m_numTileColumnsMinus1 );
	m_cTEncTop.setNumRowsMinus1                                     ( m_numTileRowsMinus1 );
	if(!m_tileUniformSpacingFlag)
	{
		m_cTEncTop.setColumnWidth                                     ( m_tileColumnWidth );
		m_cTEncTop.setRowHeight                                       ( m_tileRowHeight );
	}
	m_cTEncTop.xCheckGSParameters();
	Int uiTilesCount = (m_numTileRowsMinus1+1) * (m_numTileColumnsMinus1+1);
	if(uiTilesCount == 1)
	{
		m_bLFCrossTileBoundaryFlag = true;
	}
	m_cTEncTop.setLFCrossTileBoundaryFlag                           ( m_bLFCrossTileBoundaryFlag );
	m_cTEncTop.setEntropyCodingSyncEnabledFlag                      ( m_entropyCodingSyncEnabledFlag );
	m_cTEncTop.setTMVPModeId                                        ( m_TMVPModeId );
	m_cTEncTop.setUseScalingListId                                  ( m_useScalingListId  );
	m_cTEncTop.setScalingListFileName                               ( m_scalingListFileName );
	m_cTEncTop.setSignHideFlag                                      ( m_signHideFlag);
	m_cTEncTop.setUseRateCtrl                                       ( m_RCEnableRateControl );
	m_cTEncTop.setTargetBitrate                                     ( m_RCTargetBitrate );
	m_cTEncTop.setKeepHierBit                                       ( m_RCKeepHierarchicalBit );
	m_cTEncTop.setLCULevelRC                                        ( m_RCLCULevelRC );
	m_cTEncTop.setUseLCUSeparateModel                               ( m_RCUseLCUSeparateModel );
	m_cTEncTop.setInitialQP                                         ( m_RCInitialQP );
	m_cTEncTop.setForceIntraQP                                      ( m_RCForceIntraQP );
#if U0132_TARGET_BITS_SATURATION
	m_cTEncTop.setCpbSaturationEnabled                              ( m_RCCpbSaturationEnabled );
  m_cTEncTop.setCpbSize                                           ( m_RCCpbSize );
  m_cTEncTop.setInitialCpbFullness                                ( m_RCInitialCpbFullness );
#endif
	m_cTEncTop.setTransquantBypassEnableFlag                        ( m_TransquantBypassEnableFlag );
	m_cTEncTop.setCUTransquantBypassFlagForceValue                  ( m_CUTransquantBypassFlagForce );
	m_cTEncTop.setCostMode                                          ( m_costMode );
	m_cTEncTop.setUseRecalculateQPAccordingToLambda                 ( m_recalculateQPAccordingToLambda );
	m_cTEncTop.setUseStrongIntraSmoothing                           ( m_useStrongIntraSmoothing );
	m_cTEncTop.setActiveParameterSetsSEIEnabled                     ( m_activeParameterSetsSEIEnabled );
	m_cTEncTop.setVuiParametersPresentFlag                          ( m_vuiParametersPresentFlag );
	m_cTEncTop.setAspectRatioInfoPresentFlag                        ( m_aspectRatioInfoPresentFlag);
	m_cTEncTop.setAspectRatioIdc                                    ( m_aspectRatioIdc );
	m_cTEncTop.setSarWidth                                          ( m_sarWidth );
	m_cTEncTop.setSarHeight                                         ( m_sarHeight );
	m_cTEncTop.setOverscanInfoPresentFlag                           ( m_overscanInfoPresentFlag );
	m_cTEncTop.setOverscanAppropriateFlag                           ( m_overscanAppropriateFlag );
	m_cTEncTop.setVideoSignalTypePresentFlag                        ( m_videoSignalTypePresentFlag );
	m_cTEncTop.setVideoFormat                                       ( m_videoFormat );
	m_cTEncTop.setVideoFullRangeFlag                                ( m_videoFullRangeFlag );
	m_cTEncTop.setColourDescriptionPresentFlag                      ( m_colourDescriptionPresentFlag );
	m_cTEncTop.setColourPrimaries                                   ( m_colourPrimaries );
	m_cTEncTop.setTransferCharacteristics                           ( m_transferCharacteristics );
	m_cTEncTop.setMatrixCoefficients                                ( m_matrixCoefficients );
	m_cTEncTop.setChromaLocInfoPresentFlag                          ( m_chromaLocInfoPresentFlag );
	m_cTEncTop.setChromaSampleLocTypeTopField                       ( m_chromaSampleLocTypeTopField );
	m_cTEncTop.setChromaSampleLocTypeBottomField                    ( m_chromaSampleLocTypeBottomField );
	m_cTEncTop.setNeutralChromaIndicationFlag                       ( m_neutralChromaIndicationFlag );
	m_cTEncTop.setDefaultDisplayWindow                              ( m_defDispWinLeftOffset, m_defDispWinRightOffset, m_defDispWinTopOffset, m_defDispWinBottomOffset );
	m_cTEncTop.setFrameFieldInfoPresentFlag                         ( m_frameFieldInfoPresentFlag );
	m_cTEncTop.setPocProportionalToTimingFlag                       ( m_pocProportionalToTimingFlag );
	m_cTEncTop.setNumTicksPocDiffOneMinus1                          ( m_numTicksPocDiffOneMinus1    );
	m_cTEncTop.setBitstreamRestrictionFlag                          ( m_bitstreamRestrictionFlag );
	m_cTEncTop.setTilesFixedStructureFlag                           ( m_tilesFixedStructureFlag );
	m_cTEncTop.setMotionVectorsOverPicBoundariesFlag                ( m_motionVectorsOverPicBoundariesFlag );
	m_cTEncTop.setMinSpatialSegmentationIdc                         ( m_minSpatialSegmentationIdc );
	m_cTEncTop.setMaxBytesPerPicDenom                               ( m_maxBytesPerPicDenom );
	m_cTEncTop.setMaxBitsPerMinCuDenom                              ( m_maxBitsPerMinCuDenom );
	m_cTEncTop.setLog2MaxMvLengthHorizontal                         ( m_log2MaxMvLengthHorizontal );
	m_cTEncTop.setLog2MaxMvLengthVertical                           ( m_log2MaxMvLengthVertical );
	m_cTEncTop.setEfficientFieldIRAPEnabled                         ( m_bEfficientFieldIRAPEnabled );
	m_cTEncTop.setHarmonizeGopFirstFieldCoupleEnabled               ( m_bHarmonizeGopFirstFieldCoupleEnabled );

	m_cTEncTop.setSummaryOutFilename                                ( m_summaryOutFilename );
	m_cTEncTop.setSummaryPicFilenameBase                            ( m_summaryPicFilenameBase );
	m_cTEncTop.setSummaryVerboseness                                ( m_summaryVerboseness );
	 */
}

void GvcEncoderApp::xCreateLib()
{
	// Video I/O
	m_cTVideoIOYuvInputFile.open( m_inputFileName,     false, m_inputBitDepth, m_MSBExtendedBitDepth, m_internalBitDepth );  // read  mode
	//m_cTVideoIOYuvInputFile.skipFrames(m_FrameSkip, m_iSourceWidth - m_aiPad[0], m_iSourceHeight - m_aiPad[1], m_InputChromaFormatIDC);

	if (!m_reconFileName.empty())
	{
		m_cTVideoIOYuvReconFile.open(m_reconFileName, true, m_outputBitDepth, m_outputBitDepth, m_internalBitDepth);  // write mode
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




