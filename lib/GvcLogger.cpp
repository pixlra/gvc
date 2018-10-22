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
 * \file     GvcLogger.cpp
 * \brief    Main GVC encoder
 */

#include "GvcLogger.h"

#include <cstdarg>
#include <cstring>
#include <iostream>

static enum GVC_LOG_LEVEL Log_Level = GVC_LOG_ERROR;

void GvcLogger::SetReportLevel( enum GVC_LOG_LEVEL level )
{
  Log_Level = level;
}

enum GVC_LOG_LEVEL& GvcLogger::ReportingLevel()
{
  return Log_Level;
}

std::ostringstream& GvcLogger::Get(enum GVC_LOG_LEVEL level)
{
//   os << "- " << std::chrono::system_clock::now();
//   os << " " << ToString(level) << ": ";
//   os << std::string(level > logDEBUG ? 0 : level - logDEBUG, '\t');
   messageLevel = level;
   return os;
}
GvcLogger::~GvcLogger()
{
   if (messageLevel >= GvcLogger::ReportingLevel())
   {
      os << std::endl;
      fprintf(stderr, "%s", os.str().c_str());
      fflush(stderr);
   }
}
