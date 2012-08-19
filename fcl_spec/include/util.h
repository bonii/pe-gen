/*
    util.h is part of FCL_SPEC
    util.h - Utility functions and headers for FCL
    
    Copyright(C) 2012 Vivek Shah <bonii(at)kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

    FCL_SPEC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FCL_SPEC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FCL_SPEC.  If not, see <http://www.gnu.org/licenses/>.

    Written by Vivek Shah <bonii@kompiler.org>, Tim van Deurzen <tim(at)kompiler.org>

*/

#ifndef UTIL_H
#define UTIL_H

/*
 * For annotating calls for CMIX one may use the special macro __CMIX(),
 * however if compiling normally these annotations break the code. This macro
 * will remove the annotations when compiling without CMIX.
 */
#ifndef __CMIX
#define __CMIX(x)
#endif

#endif
