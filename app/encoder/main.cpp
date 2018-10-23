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
 * \file     main.cpp
 * \brief    Encoder app main file
 */

#include <time.h>
#include <iostream>
#include "GvcEncoderApp.h"

int main( int argc, char* argv[] )
{
    GvcEncoderApp  cGvcEncoderApp;

    cGvcEncoderApp.create();

    // parse configuration
    if(!cGvcEncoderApp.parseCfg( argc, argv ))
    {
        cGvcEncoderApp.destroy();
        return 1;
    }

    // starting time
    double dResult;
    clock_t lBefore = clock();

    // call encoding function
    cGvcEncoderApp.encode();

    // ending time
    dResult = (double)(clock()-lBefore) / CLOCKS_PER_SEC;
    printf("\n Total Time: %12.3f sec.\n", dResult);

    // destroy application encoder class
    cGvcEncoderApp.destroy();

    return 0;
}
