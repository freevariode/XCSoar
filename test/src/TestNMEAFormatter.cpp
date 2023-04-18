/* Copyright_License {

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

#include "Formatter/NMEAFormatter.hpp"
#include "util/StringAPI.hxx"
#include "TestUtil.hpp"
#include "NMEA/Info.hpp"
#include "Device/Parser.hpp"

int main()
{
  plan_tests(9);

  char buffer[100];
  NMEAParser parser;
  NMEAInfo info;

  info.Reset();
  info.clock = TimeStamp{FloatDuration{1}};
  info.date_time_utc = BrokenDateTime(2015, 7, 23, 15, 50, 11);
  info.time_available.Update(info.clock);

  // test formatter without valid GPS data
  FormatGPRMC(buffer, sizeof(buffer), info);
  ok1(StringIsEqual(buffer, 
                    "GPRMC,155011,V,0000.000,N,00000.000,E,000.0,000.0,230715,000.0,E"));

  FormatGPGSA(buffer, sizeof(buffer), info);
  ok1(StringIsEqual(buffer, 
                    "GPGSA,A,1,,,,,,,,,,,,,-1.0,-1.0,-1.0"));

  FormatGPGGA(buffer, sizeof(buffer), info);
  ok1(StringIsEqual(buffer, 
                    "GPGGA,155011.00,0000.000,N,00000.000,E,0,00,-1.0,0.000,M,,,,0000"));

  // parse NMEA sentences to set valid GPS data
  ok1(parser.ParseLine("$GPRMC,082311,A,5103.5403,N,00741.5742,E,055.3,022.4,230610,000.3,W*6C",
                       info));
  ok1(parser.ParseLine("$GPGSA,A,3,01,20,19,13,15,04,,,,,,,40.4,24.4,32.2*0A",
                       info));
  ok1(parser.ParseLine("$GPGGA,082311.0,5103.5403,N,00741.5742,E,2,06,24.4,18.893,M,,,,0000*09",
                       info));

  FormatGPRMC(buffer, sizeof(buffer), info);
  ok1(StringIsEqual(buffer, 
                    "GPRMC,082311,A,5103.540,N,00741.574,E,055.3,022.4,230610,000.3,W"));

  FormatGPGSA(buffer, sizeof(buffer), info);
  ok1(StringIsEqual(buffer, 
                    "GPGSA,A,3,01,20,19,13,15,04,,,,,,,40.4,24.4,32.2"));

  FormatGPGGA(buffer, sizeof(buffer), info);
  ok1(StringIsEqual(buffer,
                    "GPGGA,082311.00,5103.540,N,00741.574,E,2,06,24.4,18.893,M,,,,0000"));

  return exit_status();
}