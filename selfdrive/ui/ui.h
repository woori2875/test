#pragma once

#define UI_FEATURE_BRAKE 1
#define UI_FEATURE_AUTOHOLD 1
//#define UI_FEATURE_DASHCAM 1

#define UI_FEATURE_LEFT 1
#define UI_FEATURE_RIGHT 1

#define UI_FEATURE_LEFT_Y 220
#define UI_FEATURE_RIGHT_Y 20

#define UI_FEATURE_LEFT_REL_DIST 1
#define UI_FEATURE_LEFT_REL_SPEED 1
#define UI_FEATURE_LEFT_REAL_STEER 1
#define UI_FEATURE_LEFT_DESIRED_STEER 1

#define UI_FEATURE_RIGHT_CPU_TEMP 1
#define UI_FEATURE_RIGHT_AMBIENT_TEMP 1
#define UI_FEATURE_RIGHT_BATTERY_LEVEL 1
#define UI_FEATURE_RIGHT_GPS_ALTITUDE 1
#define UI_FEATURE_RIGHT_GPS_ACCURACY 1
#define UI_FEATURE_RIGHT_GPS_SATELLITE 1

#include <atomic>
#include <map>
#include <memory>
#include <string>

#include <QObject>
#include <QTimer>
#include <QColor>

#include "nanovg.h"

#include "cereal/messaging/messaging.h"
#include "common/transformations/orientation.hpp"
#include "selfdrive/camerad/cameras/camera_common.h"
#include "selfdrive/common/mat.h"
#include "selfdrive/common/modeldata.h"
#include "selfdrive/common/params.h"
#include "selfdrive/common/util.h"
#include "selfdrive/common/visionimg.h"
#include "selfdrive/common/touch.h"

#define COLOR_BLACK nvgRGBA(0, 0, 0, 255)
#define COLOR_BLACK_ALPHA(x) nvgRGBA(0, 0, 0, x)
#define COLOR_WHITE nvgRGBA(255, 255, 255, 255)
#define COLOR_WHITE_ALPHA(x) nvgRGBA(255, 255, 255, x)
#define COLOR_RED nvgRGBA(255, 0, 0, 255)
#define COLOR_RED_ALPHA(x) nvgRGBA(255, 0, 0, x)
#define COLOR_YELLOW nvgRGBA(255, 255, 0, 255)
#define COLOR_YELLOW_ALPHA(x) nvgRGBA(255, 255, 0, x)
#define COLOR_OCHRE nvgRGBA(218, 111, 37, 255)
#define COLOR_OCHRE_ALPHA(x) nvgRGBA(218, 111, 37, x)
#define COLOR_GREEN nvgRGBA(0, 255, 0, 255)
#define COLOR_GREEN_ALPHA(x) nvgRGBA(0, 255, 0, x)
#define COLOR_BLUE nvgRGBA(0, 0, 255, 255)
#define COLOR_BLUE_ALPHA(x) nvgRGBA(0, 0, 255, x)
#define COLOR_ORANGE nvgRGBA(255, 175, 3, 255)
#define COLOR_ORANGE_ALPHA(x) nvgRGBA(255, 175, 3, x)
#define COLOR_GREY nvgRGBA(191, 191, 191, 1
#define COLOR_ENGAGED nvgRGBA(0, 170, 255, 255)
#define COLOR_ENGAGED_ALPHA(x) nvgRGBA(0, 170, 255, x)
//#define COLOR_WARNING nvgRGBA(218, 111, 37, 100)
//#define COLOR_WARNING_ALPHA(x) nvgRGBA(218, 111, 37, x)
#define COLOR_ENGAGEABLE nvgRGBA(23, 51, 73, 100)
#define COLOR_ENGAGEABLE_ALPHA(x) nvgRGBA(23, 51, 73, x)
#define COLOR_LIME nvgRGBA(0, 255, 0, 255)
#define COLOR_LIME_ALPHA(x) nvgRGBA(0, 255, 0, x)
#define COLOR_FORIP nvgRGBA(231, 255, 51, 255)
#define COLOR_FORIP_ALPHA(x) nvgRGBA(231, 255, 51, x)
#define COLOR_FORGEAR nvgRGBA(77, 178, 255, 255)
#define COLOR_FORGEAR_ALPHA(x) nvgRGBA(77, 178, 255, x)
#define COLOR_AQUA nvgRGBA(0, 255, 255, 255)
#define COLOR_AQUA_ALPHA(x) nvgRGBA(0, 255, 255, x)
#define COLOR_ORANGERED nvgRGBA(255, 069, 000, 255)
#define COLOR_ORANGERED_ALPHA(x) nvgRGBA(255, 069, 000, x)
#define COLOR_SKYBLUE nvgRGBA(135, 206, 235, 255)
#define COLOR_SKYBLUE_ALPHA(x) nvgRGBA(0, 255, 255, x)

typedef cereal::CarControl::HUDControl::AudibleAlert AudibleAlert;

// TODO: this is also hardcoded in common/transformations/camera.py
// TODO: choose based on frame input size
const float y_offset = Hardware::EON() ? 0.0 : 150.0;
const float ZOOM = Hardware::EON() ? 2138.5 : 2912.8;

typedef struct Rect {
  int x, y, w, h;
  int centerX() const { return x + w / 2; }
  int centerY() const { return y + h / 2; }
  int right() const { return x + w; }
  int bottom() const { return y + h; }
  bool ptInRect(int px, int py) const {
    return px >= x && px < (x + w) && py >= y && py < (y + h);
  }
} Rect;

typedef struct Alert {
  QString text1;
  QString text2;
  QString type;
  cereal::ControlsState::AlertSize size;
  AudibleAlert sound;
  bool equal(const Alert &a2) {
    return text1 == a2.text1 && text2 == a2.text2 && type == a2.type;
  }
} Alert;

const Alert CONTROLS_WAITING_ALERT = {"오픈파일럿을 사용할수없습니다", "프로세스가 준비중입니다",
                                      "프로세스가 준비중입니다", cereal::ControlsState::AlertSize::MID,
                                      AudibleAlert::NONE};

const Alert CONTROLS_UNRESPONSIVE_ALERT = {"즉시 핸들을 잡아주세요", "프로세스가 응답하지않습니다",
                                           "프로세스가 응답하지않습니다", cereal::ControlsState::AlertSize::FULL,
                                           AudibleAlert::CHIME_WARNING_REPEAT};
const int CONTROLS_TIMEOUT = 5;

const int bdr_s = 10;
const int header_h = 420;
const int footer_h = 280;
const Rect laneless_btn = {1585, 905, 140, 140};

const int UI_FREQ = 20;   // Hz

typedef enum UIStatus {
  STATUS_DISENGAGED,
  STATUS_ENGAGED,
  STATUS_WARNING,
  STATUS_ALERT,
} UIStatus;

const QColor bg_colors [] = {
  [STATUS_DISENGAGED] =  QColor(0x00, 0x00, 0x00, 0xff),
  [STATUS_ENGAGED] = QColor(0x87, 0xce, 0xeb, 0x30),
  [STATUS_WARNING] = QColor(0x80, 0x80, 0x80, 0x0f),
  [STATUS_ALERT] = QColor(0xC9, 0x22, 0x31, 0x65),
};

typedef struct {
  float x, y;
} vertex_data;

typedef struct {
  vertex_data v[TRAJECTORY_SIZE * 2];
  int cnt;
} line_vertices_data;

typedef struct UIScene {

  mat3 view_from_calib;
  bool world_objects_visible;
  int lateralControlSelect;
  float output_scale;
//깜박이 추가
  bool leftBlinker;
  bool rightBlinker;
  int blinker_blinkingrate;
  bool brakePress;
  bool brakeLights;
//깜박이 추가 종료 
//bsd
  bool leftblindspot;
  bool rightblindspot;
  int blindspot_blinkingrate = 120;
  int car_valid_status_changed = 0;
  float currentGear;
  bool kr_date_show;
  bool kr_time_show;
  bool read_params_once = false;
  cereal::CarState::GearShifter getGearShifter;

//bsd
 // bool is_rhd;
 // bool driver_view;
  float tpmsFl, tpmsFr, tpmsRl, tpmsRr;
  bool is_OpenpilotViewEnabled;
  bool steerOverride;
  float angleSteers;
  float angleSteersDes;
  cereal::PandaState::PandaType pandaType;
  
  int laneless_mode;

  cereal::CarState::Reader car_state;
  cereal::ControlsState::Reader controls_state;
  cereal::LateralPlan::Reader lateral_plan;

  // modelV2
  float lane_line_probs[4];
  float road_edge_stds[2];
  line_vertices_data track_vertices;
  line_vertices_data lane_line_vertices[4];
  line_vertices_data road_edge_vertices[2];

  bool dm_active, engageable;

  // lead
  vertex_data lead_vertices_radar[2];
  vertex_data lead_vertices[2];

  float light_sensor, accel_sensor, gyro_sensor;
  bool started, ignition, is_metric, longitudinal_control, end_to_end;
  uint64_t started_frame;
  
  struct _LateralPlan
  {
    float laneWidth;

    float dProb;
    float lProb;
    float rProb;

    bool lanelessModeStatus;
  } lateralPlan;

  // neokii dev UI
  cereal::CarControl::Reader car_control;
  cereal::CarParams::Reader car_params;
  cereal::GpsLocationData::Reader gps_ext;
  cereal::LiveParametersData::Reader live_params;
  //gps
  int satelliteCount;
  float gpsAccuracy;
 // opkr
  bool touched2 = false;
  float brightness_off;
  int setbtn_count = 0;
  int homebtn_count = 0;
  cereal::ControlsState::Reader controls_state;
  struct _screen
  {
     int  nTime;
     int  autoScreenOff;
     int  brightness;
     int  awake;
  } scr;
} UIScene;

typedef struct UIState {
  int fb_w = 0, fb_h = 0;
  NVGcontext *vg;

  // images
  std::map<std::string, int> images;

  std::unique_ptr<SubMaster> sm;

  UIStatus status;
  UIScene scene = {};

  bool awake;

  float car_space_transform[6];
  bool wide_camera;

  //
  bool show_debug_ui, custom_lead_mark;
  bool show_cgear_ui;
  TouchState touch;
  int lock_on_anim_index;
  
  bool sidebar_view;//opkr
} UIState;


class QUIState : public QObject {
  Q_OBJECT

public:
  QUIState(QObject* parent = 0);

  // TODO: get rid of this, only use signal
  inline static UIState ui_state = {0};

signals:
  void uiUpdate(const UIState &s);
  void offroadTransition(bool offroad);

private slots:
  void update();

private:
  QTimer *timer;
  bool started_prev = true;
};


// device management class

class Device : public QObject {
  Q_OBJECT

public:
  Device(QObject *parent = 0);

private:
  // auto brightness
  const float accel_samples = 5*UI_FREQ;

  bool awake;
  int awake_timeout = 0;
  float accel_prev = 0;
  float gyro_prev = 0;
  float last_brightness = 0;
  FirstOrderFilter brightness_filter;

  QTimer *timer;
  int sleep_time = -1; // opkr
  
  void updateBrightness(const UIState &s);
  void updateWakefulness(const UIState &s);

signals:
  void displayPowerChanged(bool on);

public slots:
  void setAwake(bool on, bool reset);
  void update(const UIState &s);
};
