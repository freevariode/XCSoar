// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#include "SystemDialog.hpp"
#include "system/FileUtil.hpp"
#include "Dialogs/WidgetDialog.hpp"
#include "Dialogs/Message.hpp"
#include "Dialogs/Error.hpp"
#include "UIGlobals.hpp"
#include "Language/Language.hpp"
#include "Widget/RowFormWidget.hpp"
#include "DisplayOrientation.hpp"
#include "Hardware/RotateDisplay.hpp"
#include "System.hpp"

#include <sys/stat.h>

class SystemWidget final
  : public RowFormWidget {

public:
  explicit SystemWidget(const DialogLook &look):RowFormWidget(look) {}

private:
  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent,
               const PixelRect &rc) noexcept override;
};

class BrightnessWidget final
  : public RowFormWidget {

  enum Buttons {
    INCREASE_BRIGHTNESS,
    DECREASE_BRIGHTNESS,
  };

  Button *increase_brightness;
  Button *decrease_brightness;

public:
  explicit BrightnessWidget(const DialogLook &look):RowFormWidget(look) {}

private:
  void IncreaseBrightness();
  void DecreaseBrightness();
  void UpdateBrightnessButtons(uint_least8_t value);

  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent,
               const PixelRect &rc) noexcept override;
};

class RotationWidget final
  : public RowFormWidget {

  enum Buttons {
    LANDSCAPE,
    PORTRAIT,
    LANDSCAPE_REVERSE,
    PORTRAIT_REVERSE,
  };

  Button *landscape;
  Button *portrait;
  Button *landscape_reverse;
  Button *portrait_reverse;

public:
  explicit RotationWidget(const DialogLook &look):RowFormWidget(look) {}

private:
  void Rotate(DisplayOrientation orientation);
  void UpdateRotateButtons();

  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent,
               const PixelRect &rc) noexcept override;
};

class SSHWidget final
  : public RowFormWidget {

  enum Buttons {
    ENABLE_SSH,
    ENABLE_SSH_TEMPORARY,
    DISABLE_SSH,
  };

  Button *enable_ssh;
  Button *enable_ssh_temporary;
  Button *disable_ssh;

public:
  explicit SSHWidget(const DialogLook &look):RowFormWidget(look) {}

private:
  void EnableSSH(bool temporary);
  void DisableSSH();
  void UpdateSSHButtons();

  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent,
               const PixelRect &rc) noexcept override;
};

class VARIODWidget final
  : public RowFormWidget {

  enum Buttons {
    ENABLE_VARIOD,
    DISABLE_VARIOD,
  };

  Button *enable_variod;
  Button *disable_variod;

public:
  explicit VARIODWidget(const DialogLook &look):RowFormWidget(look) {}

private:
  void EnableVARIOD();
  void DisableVARIOD();
  void UpdateVARIODButtons();

  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent,
               const PixelRect &rc) noexcept override;
};

class SENSORDWidget final
  : public RowFormWidget {

  enum Buttons {
    ENABLE_SENSORD,
    DISABLE_SENSORD,
  };

  Button *enable_sensord;
  Button *disable_sensord;

public:
  explicit SENSORDWidget(const DialogLook &look):RowFormWidget(look) {}

private:
  void EnableSENSORD();
  void DisableSENSORD();
  void UpdateSENSORDButtons();

  /* virtual methods from class Widget */
  void Prepare(ContainerWindow &parent,
               const PixelRect &rc) noexcept override;
};

void
SystemWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                      [[maybe_unused]] const PixelRect &rc) noexcept
{
  AddButton("Brightness", [](){ 
    const DialogLook &look = UIGlobals::GetDialogLook();
    TWidgetDialog<BrightnessWidget>
      dialog(WidgetDialog::Full{}, UIGlobals::GetMainWindow(), look, "Brightness");
    dialog.AddButton(_("Close"), mrOK);
    dialog.SetWidget(look);
    dialog.ShowModal();
  });

  AddButton("Display Rotation", [](){ 
    const DialogLook &look = UIGlobals::GetDialogLook();
    TWidgetDialog<RotationWidget>
      dialog(WidgetDialog::Full{}, UIGlobals::GetMainWindow(), look, "Display Rotation");
    dialog.AddButton(_("Close"), mrOK);
    dialog.SetWidget(look);
    dialog.ShowModal();
  });

// TODO create language dialog.
//  AddButton("Language", [](){ ShowLanguageDialog(); });

  AddButton("SSH", [](){ 
    const DialogLook &look = UIGlobals::GetDialogLook();
    TWidgetDialog<SSHWidget>
      dialog(WidgetDialog::Full{}, UIGlobals::GetMainWindow(), look, "SSH");
    dialog.AddButton(_("Close"), mrOK);
    dialog.SetWidget(look);
    dialog.ShowModal();
  });

  AddButton("Variod", [](){ 
    const DialogLook &look = UIGlobals::GetDialogLook();
    TWidgetDialog<VARIODWidget>
      dialog(WidgetDialog::Full{}, UIGlobals::GetMainWindow(), look, "Variod");
    dialog.AddButton(_("Close"), mrOK);
    dialog.SetWidget(look);
    dialog.ShowModal();
  });

  AddButton("Sensord", [](){ 
    const DialogLook &look = UIGlobals::GetDialogLook();
    TWidgetDialog<SENSORDWidget>
      dialog(WidgetDialog::Full{}, UIGlobals::GetMainWindow(), look, "Sensord");
    dialog.AddButton(_("Close"), mrOK);
    dialog.SetWidget(look);
    dialog.ShowModal();
  });
}

void
BrightnessWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                          [[maybe_unused]] const PixelRect &rc) noexcept
{
  increase_brightness = AddButton("Increase Brightness", [this]() { IncreaseBrightness(); });
  decrease_brightness = AddButton("Decrease Brightness", [this]() { DecreaseBrightness(); });
  uint_least8_t current_brightness = OpenvarioGetBrightness();
  UpdateBrightnessButtons(current_brightness);
}

inline void
BrightnessWidget::IncreaseBrightness()
{
  uint_least8_t current_brightness = OpenvarioGetBrightness();
  OpenvarioSetBrightness(current_brightness + 1);
  UpdateBrightnessButtons(current_brightness + 1);
}

inline void
BrightnessWidget::DecreaseBrightness()
{
  uint_least8_t current_brightness = OpenvarioGetBrightness();
  OpenvarioSetBrightness(current_brightness - 1);
  UpdateBrightnessButtons(current_brightness - 1);
}

inline void
BrightnessWidget::UpdateBrightnessButtons(uint_least8_t value)
{
  if(decrease_brightness != nullptr) {
    decrease_brightness->SetEnabled(value != 1);
  }
  if(increase_brightness != nullptr) {
    increase_brightness->SetEnabled(value < 10);
  }
}

void
RotationWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                        [[maybe_unused]] const PixelRect &rc) noexcept
{
  landscape = AddButton("Landscape", [this]() {
    Rotate(DisplayOrientation::LANDSCAPE);
  });
  portrait = AddButton("Portrait", [this]() {
    Rotate(DisplayOrientation::PORTRAIT);
  });
  landscape_reverse = AddButton("Landscape Reverse", [this]() {
    Rotate(DisplayOrientation::REVERSE_LANDSCAPE);
  });
  portrait_reverse = AddButton("Portrait Reverse", [this]() {
    Rotate(DisplayOrientation::REVERSE_PORTRAIT);
  });
  UpdateRotateButtons();
}

inline void
RotationWidget::Rotate(DisplayOrientation orientation)
{
  try {
    OpenvarioSetRotation(orientation);
    UpdateRotateButtons();
  } catch (...) {
    ShowError(std::current_exception(), "File not found");
    return;
  }
}

inline void
RotationWidget::UpdateRotateButtons()
{
  DisplayOrientation orientation;
  try {
    orientation = OpenvarioGetRotation();
  } catch (...) {
    ShowError(std::current_exception(), "File not found");
    return;
  }

  if (landscape != nullptr) {
    landscape->SetEnabled(
      orientation != DisplayOrientation::LANDSCAPE);
  }
  if (portrait != nullptr) {
    portrait->SetEnabled(
      orientation != DisplayOrientation::PORTRAIT);
  }
  if (landscape_reverse != nullptr) {
    landscape_reverse->SetEnabled(
      orientation != DisplayOrientation::REVERSE_LANDSCAPE);
  }
  if (portrait_reverse != nullptr) {
    portrait_reverse->SetEnabled(
      orientation != DisplayOrientation::REVERSE_PORTRAIT);
  }
}

void
SSHWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                   [[maybe_unused]] const PixelRect &rc) noexcept
{
  enable_ssh = AddButton("Enable SSH (permanent)", [this]() { EnableSSH(false); });
  enable_ssh_temporary = AddButton("Enable SSH temporarily (until reboot)", [this]() { EnableSSH(true); });
  disable_ssh = AddButton("Disable SSH", [this]() { DisableSSH(); });
  UpdateSSHButtons();
}

inline void
SSHWidget::EnableSSH(bool temporary)
{
  OpenvarioEnableSSH(temporary);
  UpdateSSHButtons();
}

inline void
SSHWidget::DisableSSH()
{
  OpenvarioDisableSSH();
  UpdateSSHButtons();
}

inline void
SSHWidget::UpdateSSHButtons()
{
  SSHStatus status = OpenvarioGetSSHStatus();

  if (enable_ssh != nullptr) {
    enable_ssh->SetEnabled(
      status == SSHStatus::DISABLED ||
      status == SSHStatus::TEMPORARY);
  }
  if (enable_ssh_temporary != nullptr) {
    enable_ssh_temporary->SetEnabled(
      status == SSHStatus::DISABLED ||
      status == SSHStatus::ENABLED);
  }
  if (disable_ssh != nullptr) {
    disable_ssh->SetEnabled(
      status == SSHStatus::ENABLED ||
      status == SSHStatus::TEMPORARY);
  }
}

void
VARIODWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                   [[maybe_unused]] const PixelRect &rc) noexcept
{
  enable_variod = AddButton("Enable Variod", [this]() { EnableVARIOD(); });
  disable_variod = AddButton("Disable Variod", [this]() { DisableVARIOD(); });
  UpdateVARIODButtons();
}

inline void
VARIODWidget::EnableVARIOD()
{
  OpenvarioEnableVARIOD();
  UpdateVARIODButtons();
}

inline void
VARIODWidget::DisableVARIOD()
{
  OpenvarioDisableVARIOD();
  UpdateVARIODButtons();
}

inline void
VARIODWidget::UpdateVARIODButtons()
{
  VARIODStatus status = OpenvarioGetVARIODStatus();

  if (enable_variod != nullptr) {
    enable_variod->SetEnabled(
      status == VARIODStatus::DISABLED);
  }
  if (disable_variod != nullptr) {
    disable_variod->SetEnabled(
      status == VARIODStatus::ENABLED);
  }
}

void
SENSORDWidget::Prepare([[maybe_unused]] ContainerWindow &parent,
                   [[maybe_unused]] const PixelRect &rc) noexcept
{
  enable_sensord = AddButton("Enable Sensord", [this]() { EnableSENSORD(); });
  disable_sensord = AddButton("Disable Sensord", [this]() { DisableSENSORD(); });
  UpdateSENSORDButtons();
}

inline void
SENSORDWidget::EnableSENSORD()
{
  OpenvarioEnableSENSORD();
  UpdateSENSORDButtons();
}

inline void
SENSORDWidget::DisableSENSORD()
{
  OpenvarioDisableSENSORD();
  UpdateSENSORDButtons();
}

inline void
SENSORDWidget::UpdateSENSORDButtons()
{
  SENSORDStatus status = OpenvarioGetSENSORDStatus();

  if (enable_sensord != nullptr) {
    enable_sensord->SetEnabled(
      status == SENSORDStatus::DISABLED);
  }
  if (disable_sensord != nullptr) {
    disable_sensord->SetEnabled(
      status == SENSORDStatus::ENABLED);
  }
}

void
ShowSystemDialog()
{
  const DialogLook &look = UIGlobals::GetDialogLook();
  TWidgetDialog<SystemWidget>
    dialog(WidgetDialog::Full{}, UIGlobals::GetMainWindow(), look, "System Settings");
  dialog.AddButton(_("Close"), mrOK);
  dialog.SetWidget(look);
  dialog.ShowModal();
}
