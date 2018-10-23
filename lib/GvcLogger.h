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
 * \file     GvcLogger.h
 * \brief    Main GVC encoder
 */

#ifndef __GVCLOGGER_H__
#define __GVCLOGGER_H__

#include <sstream>

enum GVC_LOG_LEVEL
{
  GVC_LOG_ERROR = 0,
  GVC_LOG_WARNINGS = 1,
  GVC_LOG_INFO = 2,
};

#define LOG(level) \
if (level > GvcLogger::ReportingLevel()) ; \
else GvcLogger().Get(level)

/**
 * \class    GvcLogger
 * \brief    Main GVC encoder class
 */
class GvcLogger
{
public:
  GvcLogger() {}
   virtual ~GvcLogger();
   std::ostringstream& Get(enum GVC_LOG_LEVEL level = GVC_LOG_ERROR);
public:
   static void SetReportLevel( enum GVC_LOG_LEVEL level );
   static enum GVC_LOG_LEVEL& ReportingLevel();
protected:
   std::ostringstream os;
private:
   GvcLogger(const GvcLogger&);
   GvcLogger& operator =(const GvcLogger&);
private:
   enum GVC_LOG_LEVEL messageLevel;
};

#endif  // __GVCLOGGER_H__
