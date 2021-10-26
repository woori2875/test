#pragma once

#include <QPushButton>
#include <QSoundEffect>

#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/ui.h"
#include <QComboBox>
#include <QAbstractItemView>

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

// 제어 설정
class LateralControl : public AbstractControl {
  Q_OBJECT

public:
  LateralControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;

  void refresh();
};
