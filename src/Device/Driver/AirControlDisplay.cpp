// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#include "Device/Driver/AirControlDisplay.hpp"
#include "Device/Driver.hpp"
#include "Device/Util/NMEAWriter.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/InputLine.hpp"
#include "NMEA/Checksum.hpp"
#include "Atmosphere/Pressure.hpp"
#include "RadioFrequency.hpp"
#include "TransponderSquawk.hpp"
#include "Units/System.hpp"
#include "Math/Util.hpp"
#include "Operation/Operation.hpp"
#include "Formatter/NMEAFormatter.hpp"
#include "NMEA/MoreData.hpp"
#include "Operation/Operation.hpp"
#include "time/TimeoutClock.hpp"

using std::string_view_literals::operator""sv;

 class ACDDevice : public AbstractDevice {
   Port &port;
   PeriodClock status_clock;
   static bool SendIntQNH;
   static double lastSendPascalPresure;

 public:
   ACDDevice(Port &_port):port(_port) { SendIntQNH = false; lastSendPascalPresure = 0;}

   /* virtual methods from class Device */
   bool ParseNMEA(const char *line, struct NMEAInfo &info) override;
   bool PutQNH(const AtmosphericPressure &pres,OperationEnvironment &env) override;
   bool PutVolume(unsigned volume, OperationEnvironment &env) override;
   bool PutStandbyFrequency(RadioFrequency frequency,
                            const TCHAR *name,
                            OperationEnvironment &env) override;
   bool PutSquawk(TransponderSquawk squawk, OperationEnvironment &env) override;
   void OnSensorUpdate(const MoreData &basic) override;
   void OnCalculatedUpdate(const MoreData &basic, const DerivedInfo &calculated) override;
   static bool ParsePAAVS(NMEAInputLine &line, NMEAInfo &info,Port &port);
 private:
   static void setLastSendInternalQNH(bool intQNH);
   static bool getLastSendInternalQNHStatus();
   static void setLastSendPascalPresure(double pascal);
   static double getLastSendPascalPresure();
 };

void
ACDDevice::OnCalculatedUpdate(const MoreData &basic,const DerivedInfo &calculated)
{
   NullOperationEnvironment env;
   char buffer[100];
  
    if (basic.settings.qnh_available.IsValid()){
      unsigned qnh = basic.settings.qnh.GetPascal();
      sprintf(buffer,"PAAVC,S,ALT,QNH,%u",qnh);
      PortWriteNMEA(port, buffer, env);
      setLastSendPascalPresure(basic.settings.qnh.GetPascal());
      setLastSendInternalQNH(true);
    }
}

 bool ACDDevice::SendIntQNH = false;
 double ACDDevice::lastSendPascalPresure = 0;

 void
 ACDDevice::setLastSendInternalQNH(bool intQNH){
     SendIntQNH = intQNH;
 }

 bool
 ACDDevice::getLastSendInternalQNHStatus(){
     return SendIntQNH;
 }

 void 
 ACDDevice::setLastSendPascalPresure(double pascal){
   lastSendPascalPresure = pascal;
 }

 double 
 ACDDevice::getLastSendPascalPresure(){
   return lastSendPascalPresure;
 }

 bool
 ACDDevice::ParsePAAVS(NMEAInputLine &line, NMEAInfo &info,Port &port)
{
  double value;
  bool result = false;
  NullOperationEnvironment env;
   
  const auto type = line.ReadView();
  char buffer[100];

    /*to synch internal and external QNH at XCSoar after setting internal QNH*/
  if (getLastSendInternalQNHStatus() == true) {
	   unsigned qnhAsInt = value;
       sprintf(buffer, "PAAVC,S,ALT,QNH,%u", qnhAsInt);
       PortWriteNMEA(port, buffer, env);
       setLastSendPascalPresure(value);
       setLastSendInternalQNH(false);
  }

/*    if external QNH was changed by a other device, ACD will be synchonised
  if (getLastSendPascalPresure() != info.settings.qnh.GetPascal() && info.settings.qnh_available.IsValid()){
	   unsigned qnhAsInt = uround(info.settings.qnh.GetPascal());
       sprintf(buffer, "PAAVC,S,ALT,QNH,%u", qnhAsInt);
       PortWriteNMEA(port, buffer, env);
       setLastReceivedPascalPresure(info.settings.qnh.GetPascal());
       setLastSendPascalPresure(info.settings.qnh.GetPascal());
  }*/

  if (type == "ALT"sv) {
    /*
    $PAAVS,ALT,<ALTQNE>,<ALTQNH>,<QNH>
    <ALTQNE> Current QNE altitude in meters with two decimal places
    <ALTQNH> Current QNH altitude in meters with two decimal places
    <QNH> Current QNH setting in pascal (unsigned integer (e.g. 101325))
    */
      if (line.ReadChecked(value))
        info.ProvidePressureAltitude(value);

      if (line.ReadChecked(value))
        info.ProvideBaroAltitudeTrue(value);

      if (line.ReadChecked(value)) {
        auto qnh = AtmosphericPressure::Pascal(value);
        info.settings.ProvideQNH(qnh,info.clock);
        setLastSendPascalPresure(value);
        return true;
	  }
      else {
        return true;
      }
  }
  else if (type == "COM"sv) {
    /*
    $PAAVS,COM,<CHN1>,<CHN2>,<RXVOL1>,<RXVOL2>,<DWATCH>,<RX1>,<RX2>,<TX1>
    <CHN1> Primary radio channel;
           25kHz frequencies and 8.33kHz channels as unsigned integer
           values between 118000 and 136990
    <CHN2> Secondary radio channel;
           25kHz frequencies and 8.33kHz channels as unsigned integer
           values between 118000 and 136990
    <RXVOL1> Primary radio channel volume (Unsigned integer values, 0–100)
    <RXVOL2> Secondary radio channel volume (Unsigned integer values, 0–100)
    <DWATCH> Dual watch mode (0 = off; 1 = on)
    <RX1> Primary channel rx state (0 = no signal rec; 1 = signal rec)
    <RX2> Secondary channel rx state (0 = no signal rec; 1 = signal rec)
    <TX1> Transmit active (0 = no transmission; 1 = transmitting signal)
    */

    if (line.ReadChecked(value)) {
      info.settings.has_active_frequency.Update(info.clock);
      info.settings.active_frequency = RadioFrequency::FromKiloHertz(value);
      result = true;
    }

    if (line.ReadChecked(value)) {
      info.settings.has_standby_frequency.Update(info.clock);
      info.settings.standby_frequency = RadioFrequency::FromKiloHertz(value);
      result = true;
    }

    unsigned volume;
    if (line.ReadChecked(volume)){
      info.settings.ProvideVolume(volume, info.clock);
    } 
  }
  else if (type == "XPDR"sv) {
    /*
    $PAAVS,XPDR,<SQUAWK>,<ACTIVE>,<ALTINH>
    <SQUAWK> Squawk code value;
             Octal unsigned integer value between 0000 and 7777 (digits 0–7).
    <ACTIVE> Active flag;
             0: standby (transponder is switched off / "SBY" mode)
             1: active (transponder is switched on / "ALT" or "ON" mode
                dependent of ALTINH)
    <ALTINH> Altitude inhibit flag;
             0: transmit altitude ("ALT" mode if active)
             1: do not transmit altitude ("ON" mode if active)
    */
      if (line.ReadChecked(value)) {
        info.settings.has_squawk.Update(info.clock);
        info.settings.squawk = TransponderSquawk(value);
      }
       result = true;
  }
   return result; 
}

bool
ACDDevice::PutQNH(const AtmosphericPressure &pres, OperationEnvironment &env)
{
  char buffer[100];
  unsigned qnh = uround(pres.GetPascal());
  sprintf(buffer, "PAAVC,S,ALT,QNH,%u", qnh);
  PortWriteNMEA(port, buffer, env);
  setLastSendPascalPresure(pres.GetPascal());
  setLastSendInternalQNH(true);
  return true;
}

bool
ACDDevice::PutVolume(unsigned volume, OperationEnvironment &env)
{
  char buffer[100];
  sprintf(buffer, "PAAVC,S,COM,RXVOL1,%u", volume);
  PortWriteNMEA(port, buffer, env);
  return true;
}

bool
ACDDevice::PutStandbyFrequency(RadioFrequency frequency,
                                   [[maybe_unused]] const TCHAR *name,
                                   OperationEnvironment &env)
{
  char buffer[100];
  unsigned freq = frequency.GetKiloHertz();
  sprintf(buffer, "PAAVC,S,COM,CHN2,%u", freq);
  PortWriteNMEA(port, buffer, env);
  return true;
}

bool
 ACDDevice::PutSquawk(TransponderSquawk squawk, OperationEnvironment &env)
 {
   char buffer[100];
   sprintf(buffer, "PAAVC,S,XPDR,SQUAWK,%04u", squawk.GetCode());
   PortWriteNMEA(port, buffer, env);
   return true;
 }

bool
ACDDevice::ParseNMEA(const char *_line, NMEAInfo &info)
{
  if (!VerifyNMEAChecksum(_line))
    return false;

  NMEAInputLine line(_line);

  if (line.ReadCompare("$PAAVS"))
    return ParsePAAVS(line, info, port);
  else
    return false;
}

void
ACDDevice::OnSensorUpdate(const MoreData &basic)
{
  NullOperationEnvironment env;

  if (basic.gps.fix_quality != FixQuality::NO_FIX &&
      
      status_clock.CheckUpdate(std::chrono::seconds(1))) {

    char buffer[100];

    FormatGPRMC(buffer, sizeof(buffer), basic);
    PortWriteNMEA(port, buffer, env);

    FormatGPGSA(buffer, sizeof(buffer), basic);
    PortWriteNMEA(port, buffer, env);

    FormatGPGGA(buffer, sizeof(buffer), basic);
    PortWriteNMEA(port, buffer, env);
  }
}

static Device *
AirControlDisplayCreateOnPort([[maybe_unused]] const DeviceConfig &config, Port &com_port)
{
  return new ACDDevice(com_port);
}

const struct DeviceRegister acd_driver = {
  _T("ACD"),
  _T("Air Control Display"),
  DeviceRegister::RECEIVE_SETTINGS | DeviceRegister::SEND_SETTINGS,
  AirControlDisplayCreateOnPort,
};
