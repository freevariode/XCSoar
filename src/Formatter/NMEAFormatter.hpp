/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2023 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/
#pragma once

#include "NMEA/Info.hpp"

/**
 * GPRMC,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>
 * 
 * <1>  UTC Time of position fix, hhmmss format
 * <2>  Status, A = Valid position, V = NAV receiver warning
 * <3>  Latitude,ddmm.mmmm format (leading zeros will be transmitted)
 * <4>  Latitude hemisphere, N or S
 * <5>  Longitude,dddmm.mmmm format (leading zeros will be transmitted)
 * <6>  Longitude hemisphere, E or W
 * <7>  Speed over ground, 0.0 to 999.9 knots
 * <8>  Course over ground 000.0 to 359.9 degrees, true
 *      (leading zeros will be transmitted)
 * <9>  UTC date of position fix, ddmmyy format
 * <10> Magnetic variation, 000.0 to 180.0 degrees
 *      (leading zeros will be transmitted)
 * <11> Magnetic variation direction, E or W
 *      (westerly variation adds to true course)
 */
void
FormatGPRMC(char *buffer, size_t size, const NMEAInfo &info) noexcept;

/*
 * GPGGA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>,<13>,<14>
 *
 * <1>  Universal Time Coordinated (UTC), hhmmss.ss
 * <2>  Latitude,ddmm.mmmm format (leading zeros will be transmitted)
 * <3>  Latitude hemisphere, N or S
 * <4>  Longitude,dddmm.mmmm format (leading zeros will be transmitted)
 * <5>  Longitude hemisphere, E or W
 * <6>  GPS Quality Indicator,
 *      0 - fix not available,
 *      1 - GPS fix,
 *      2 - Differential GPS fix
 *      (values above 2 are 2.3 features)
 *      3 = PPS fix
 *      4 = Real Time Kinematic
 *      5 = Float RTK
 *      6 = estimated (dead reckoning)
 *      7 = Manual input mode
 *      8 = Simulation mode
 * <7>  Number of satellites in view, 00 - 12
 * <8>  Horizontal Dilution of precision (meters)
 * <9>  Antenna Altitude above/below mean-sea-level (geoid) (in meters)
 * <10> Units of antenna altitude, meters
 * <11> Geoidal separation, the difference between the WGS-84 earth
 *      ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level
 *      below ellipsoid
 *      (not implemented)
 * <12> Units of geoidal separation, meters 
 *      (not implemented)
 * <13> Age of differential GPS data, time in seconds since last SC104
 *      type 1 or 9 update, null field when DGPS is not used 
 *      (not implemented)
 * <14> Differential reference station ID, 0000-1023
 */
void
FormatGPGGA(char *buffer, size_t size, const NMEAInfo &info) noexcept;

/*
 * GPGSA,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>,<13>,<14>,<15>,<16>,<17>
 *
 *  <1> Selection mode
 *        M=Manual, forced to operate in 2D or 3D
 *        A=Automatic, 3D/2D
 *  <2> Mode (1 = no fix, 2 = 2D fix, 3 = 3D fix)
 *  <3> ID of 1st satellite used for fix
 *  <4> ID of 2nd satellite used for fix
 *  ...
 *  <14> ID of 12th satellite used for fix
 *  <15> PDOP
 *  <16> HDOP
 *  <17> VDOP
 */
void
FormatGPGSA(char *buffer, size_t size, const NMEAInfo &info) noexcept;