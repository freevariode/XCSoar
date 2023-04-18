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

#include "Units/Units.hpp"
#include "NMEAFormatter.hpp"

void
FormatGPRMC(char *buffer, size_t size, const NMEAInfo &info) noexcept
{
  const GeoPoint location = info.location_available
    ? info.location
    : GeoPoint::Zero();

  const BrokenDateTime now = info.time_available &&
    info.date_time_utc.IsDatePlausible()
    ? info.date_time_utc
    : BrokenDateTime::NowUTC();

  StringFormat(buffer, size,
               "GPRMC,%02u%02u%02u,%c,%02u%02u.%03u,%c,%03u%02u.%03u,%c,%05.1f,%05.1f,%02u%02u%02u,%05.1f,%c",
               now.hour, now.minute, now.second,
               info.location_available ? 'A' : 'V',
               (int)location.latitude.AbsoluteDegrees(),
               location.latitude.ToDMM().minutes,
               location.latitude.ToDMM().decimal_minutes,
               location.latitude.IsNegative() ? 'S' : 'N',
               (int)location.longitude.AbsoluteDegrees(),
               location.longitude.ToDMM().minutes,
               location.longitude.ToDMM().decimal_minutes,
               location.longitude.IsNegative() ? 'W' : 'E',
               (double)Units::ToUserUnit(info.ground_speed, Unit::KNOTS),
               (double)info.track.Degrees(),
               now.day, now.month, now.year % 100,
               info.variation.AbsoluteDegrees(),
               info.variation.IsNegative() ? 'W' : 'E');
}

void
FormatGPGGA(char *buffer, size_t size, const NMEAInfo &info) noexcept
{
  const GeoPoint location = info.location_available
    ? info.location
    : GeoPoint::Zero();

  const BrokenDateTime now = info.time_available &&
    info.date_time_utc.IsDatePlausible()
    ? info.date_time_utc
    : BrokenDateTime::NowUTC();

  StringFormat(buffer, size,
               "GPGGA,%02u%02u%02u.00,%02u%02u.%03u,%c,%03u%02u.%03u,%c,%u,%02u,%.1f,%.3f,%c,,,,0000",
               now.hour, now.minute, now.second,
               (int)location.latitude.AbsoluteDegrees(),
               location.latitude.ToDMM().minutes,
               location.latitude.ToDMM().decimal_minutes,
               location.latitude.IsNegative() ? 'S' : 'N',
               (int)location.longitude.AbsoluteDegrees(),
               location.longitude.ToDMM().minutes,
               location.longitude.ToDMM().decimal_minutes,
               location.longitude.IsNegative() ? 'W' : 'E',
               (unsigned)info.gps.fix_quality,
               info.gps.satellites_used_available ? info.gps.satellites_used : 0,
               info.gps.hdop,
               info.gps_altitude,
               'M');
}

void
FormatGPGSA(char *buffer, size_t size, const NMEAInfo &info) noexcept
{
  unsigned gps_status;

  if (!info.location_available)
    gps_status = 1;
  else if (!info.gps_altitude_available)
    gps_status = 2;
  else
    gps_status = 3;

  NarrowString<256> sat_ids;
  sat_ids.clear();
  for (unsigned i = 0; i < GPSState::MAXSATELLITES; ++i) {
    if (info.gps.satellite_ids[i] > 0 && info.gps.satellite_ids_available) {
      sat_ids.AppendFormat("%02u,", info.gps.satellite_ids[i]);
    } else {
      sat_ids.append(",");
    }
  }

  StringFormat(buffer, size,
               "GPGSA,A,%u,%s%.1f,%.1f,%.1f",
               gps_status,
               sat_ids.c_str(),
               info.gps.pdop,
               info.gps.hdop,
               info.gps.vdop);
}