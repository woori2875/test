#pragma once

#include <QPushButton>

#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"

// SSH enable toggle
class SshToggle : public ToggleControl {
  Q_OBJECT

public:
  SshToggle() : ToggleControl("Enable SSH", "", "", Hardware::get_ssh_enabled()) {
    QObject::connect(this, &SshToggle::toggleFlipped, [=](bool state) {
      Hardware::set_ssh_enabled(state);
    });
  }
};

// SSH key management widget
class SshControl : public ButtonControl {
  Q_OBJECT

public:
  SshControl();

private:
  Params params;

  QLabel username_label;

  void refresh();
  void getUserKeys(const QString &username);
};

//date
class KRDateToggle : public ToggleControl {
  Q_OBJECT

public:
  KRDateToggle() : ToggleControl("주행화면 날짜 표시", "주행화면에 현재 날짜를 표시합니다.", "../assets/offroad/icon_shell.png", Params().getBool("KRDateShow")) {
    QObject::connect(this, &KRDateToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("KRDateShow", status);
      if (state) {
        QUIState::ui_state.scene.kr_date_show = true;
      } else {
        QUIState::ui_state.scene.kr_date_show = false;
      }
    });
  }
};

class KRTimeToggle : public ToggleControl {
  Q_OBJECT

public:
  KRTimeToggle() : ToggleControl("주행화면 시간 표시", "주행화면에 현재 시간을 표시합니다.", "../assets/offroad/icon_shell.png", Params().getBool("KRTimeShow")) {
    QObject::connect(this, &KRTimeToggle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("KRTimeShow", status);
      if (state) {
        QUIState::ui_state.scene.kr_time_show = true;
      } else {
        QUIState::ui_state.scene.kr_time_show = false;
      }
    });
  }
};

// LateralControlSelect
class LateralControlSelect : public AbstractControl {
  Q_OBJECT

public:
  LateralControlSelect();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;

  void refresh();
};

// openpilot Preview
class OpenpilotView : public AbstractControl {
  Q_OBJECT

public:
  OpenpilotView();

private:
  QPushButton btn;
  Params params;
  
  void refresh();
};
