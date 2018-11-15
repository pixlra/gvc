//
// Created by rmonteiro on 24-10-2018.
//

#include "GvcBlockUnit.h"
#include "GvcFrameUnit.h"

GvcBlockUnit::GvcBlockUnit()
{}

GvcBlockUnit::~GvcBlockUnit()
{}

void GvcBlockUnit::create(ChromaFormat chromaFormatIDC, unsigned int uiNumPartition, unsigned int uiWidth,
                          unsigned int uiHeight, int unitSize)
{
    m_pcFrame              = NULL;
    m_uiNumPartition     = uiNumPartition;
    m_unitSize = unitSize;
}