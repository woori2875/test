#include "selfdrive/ui/qt/home.h"

#include <QDate>
#include <QTime>
#include <QLocale>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "selfdrive/common/params.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/widgets/drive_stats.h"
#include "selfdrive/ui/qt/widgets/prime.h"

// HomeWindow: the container for the offroad and onroad UIs

HomeWindow::HomeWindow(QWidget* parent) : QWidget(parent) {
  QHBoxLayout *main_layout = new QHBoxLayout(this);
  main_layout->setMargin(0);
  main_layout->setSpacing(0);

  sidebar = new Sidebar(this);
  main_layout->addWidget(sidebar);
  QObject::connect(this, &HomeWindow::update, sidebar, &Sidebar::updateState);
  QObject::connect(sidebar, &Sidebar::openSettings, this, &HomeWindow::openSettings);

  slayout = new QStackedLayout();
  main_layout->addLayout(slayout);

  home = new OffroadHome();
  slayout->addWidget(home);

  onroad = new OnroadWindow(this);
  slayout->addWidget(onroad);

  QObject::connect(this, &HomeWindow::update, onroad, &OnroadWindow::updateStateSignal);
  QObject::connect(this, &HomeWindow::offroadTransitionSignal, onroad, &OnroadWindow::offroadTransitionSignal);

  driver_view = new DriverViewWindow(this);
  connect(driver_view, &DriverViewWindow::done, [=] {
    showDriverView(false);
  });
  slayout->addWidget(driver_view);
  setAttribute(Qt::WA_NoSystemBackground);
}

void HomeWindow::showSidebar(bool show) {
  sidebar->setVisible(show);
}

void HomeWindow::offroadTransition(bool offroad) {
  sidebar->setVisible(offroad);
  if (offroad) {
    slayout->setCurrentWidget(home);
  } else {
    slayout->setCurrentWidget(onroad);
  }
  emit offroadTransitionSignal(offroad);
}

void HomeWindow::showDriverView(bool show) {
  if (show) {
    emit closeSettings();
    slayout->setCurrentWidget(driver_view);
  } else {
    slayout->setCurrentWidget(home);
  }
  sidebar->setVisible(show == false);
}

#ifdef QCOM2
void HomeWindow::mouseReleaseEvent(QMouseEvent* e) {
#else  
void HomeWindow::mousePressEvent(QMouseEvent* e) {
#endif
  // Laneless mode
  if (QUIState::ui_state.scene.started && !sidebar->isVisible() && QUIState::ui_state.scene.end_to_end && laneless_btn.ptInRect(e->x(), e->y())) {
    QUIState::ui_state.scene.laneless_mode = QUIState::ui_state.scene.laneless_mode + 1;
    if (QUIState::ui_state.scene.laneless_mode > 2) {
      QUIState::ui_state.scene.laneless_mode = 0;
    }
    if (QUIState::ui_state.scene.laneless_mode == 0) {
      Params().put("LanelessMode", "0", 1);
    } else if (QUIState::ui_state.scene.laneless_mode == 1) {
      Params().put("LanelessMode", "1", 1);
    } else if (QUIState::ui_state.scene.laneless_mode == 2) {
      Params().put("LanelessMode", "2", 1);
    }
    return;
  }
  // Handle sidebar collapsing
  if (onroad->isVisible() && (!sidebar->isVisible() || e->x() > sidebar->width())) {
    sidebar->setVisible(!sidebar->isVisible() && !onroad->isMapVisible());
    QUIState::ui_state.sidebar_view = !QUIState::ui_state.sidebar_view; // opkr
  }
  
  QUIState::ui_state.scene.setbtn_count = 0; // opkr
  QUIState::ui_state.scene.homebtn_count = 0;
  if (QUIState::ui_state.scene.started && QUIState::ui_state.scene.scr.autoScreenOff != -2) {
    QUIState::ui_state.scene.touched2 = true;
    QTimer::singleShot(500, []() { QUIState::ui_state.scene.touched2 = false; });
  } // opkr
}

// OffroadHome: the offroad home page

OffroadHome::OffroadHome(QWidget* parent) : QFrame(parent) {
  QVBoxLayout* main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(40, 40, 40, 45);

  // top header
  QHBoxLayout* header_layout = new QHBoxLayout();
  header_layout->setContentsMargins(15, 15, 15, 0);
  header_layout->setSpacing(16);

  date = new QLabel();
  header_layout->addWidget(date, 1, Qt::AlignHCenter | Qt::AlignLeft);

  update_notif = new QPushButton("UPDATE");
  update_notif->setVisible(false);
  update_notif->setStyleSheet("background-color: #364DEF;");
  QObject::connect(update_notif, &QPushButton::clicked, [=]() { center_layout->setCurrentIndex(1); });
  header_layout->addWidget(update_notif, 0, Qt::AlignHCenter | Qt::AlignRight);

  alert_notif = new QPushButton();
  alert_notif->setVisible(false);
  alert_notif->setStyleSheet("background-color: #E22C2C;");
  QObject::connect(alert_notif, &QPushButton::clicked, [=] { center_layout->setCurrentIndex(2); });
  header_layout->addWidget(alert_notif, 0, Qt::AlignHCenter | Qt::AlignRight);

  header_layout->addWidget(new QLabel(getBrandVersion()), 0, Qt::AlignHCenter | Qt::AlignRight);

  main_layout->addLayout(header_layout);

  // main content
  main_layout->addSpacing(25);
  center_layout = new QStackedLayout();

  QWidget* statsAndSetupWidget = new QWidget(this);
  QHBoxLayout* statsAndSetup = new QHBoxLayout(statsAndSetupWidget);
  statsAndSetup->setMargin(0);
  statsAndSetup->setSpacing(30);
  statsAndSetup->addWidget(new DriveStats, 1);
  statsAndSetup->addWidget(new SetupWidget);

  center_layout->addWidget(statsAndSetupWidget);

  // add update & alerts widgets
  update_widget = new UpdateAlert();
  QObject::connect(update_widget, &UpdateAlert::dismiss, [=]() { center_layout->setCurrentIndex(0); });
  center_layout->addWidget(update_widget);
  alerts_widget = new OffroadAlert();
  QObject::connect(alerts_widget, &OffroadAlert::dismiss, [=]() { center_layout->setCurrentIndex(0); });
  center_layout->addWidget(alerts_widget);

  main_layout->addLayout(center_layout, 1);

  // set up refresh timer
  timer = new QTimer(this);
  timer->callOnTimeout(this, &OffroadHome::refresh);

  setStyleSheet(R"(
    * {
     color: white;
    }
    OffroadHome {
      background-color: black;
    }
    OffroadHome > QPushButton {
      padding: 15px 30px;
      border-radius: 5px;
      font-size: 40px;
      font-weight: 500;
    }
    OffroadHome > QLabel {
      font-size: 55px;
    }
  )");
}

void OffroadHome::showEvent(QShowEvent *event) {
  refresh();
  timer->start(10 * 1000);
}

void OffroadHome::hideEvent(QHideEvent *event) {
  timer->stop();
}

void OffroadHome::refresh() {
  QLocale::setDefault(QLocale::Korean);
  QString date_kr = QDate::currentDate().toString(Qt::DefaultLocaleLongDate);
  QString time_kr = QTime::currentTime().toString(Qt::DefaultLocaleShortDate);
  date->setText(date_kr + " " + time_kr);

  bool updateAvailable = update_widget->refresh();
  int alerts = alerts_widget->refresh();

  // pop-up new notification
  int idx = center_layout->currentIndex();
  if (!updateAvailable && !alerts) {
    idx = 0;
  } else if (updateAvailable && (!update_notif->isVisible() || (!alerts && idx == 2))) {
    idx = 1;
  } else if (alerts && (!alert_notif->isVisible() || (!updateAvailable && idx == 1))) {
    idx = 2;
  }
  center_layout->setCurrentIndex(idx);

  update_notif->setVisible(updateAvailable);
  alert_notif->setVisible(alerts);
  if (alerts) {
    alert_notif->setText(QString::number(alerts) + " 경고" + (alerts > 1 ? "S" : ""));
  }
}
