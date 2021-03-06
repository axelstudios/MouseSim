#include "MouseSim.hpp"
#include "AboutBox.hpp"

#include <QCoreApplication>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSharedMemory>
#include <QTimer>

#include <Windows.h>

MouseSim::MouseSim() :
  m_appPath("\"" + QCoreApplication::applicationFilePath().replace("/", "\\") + "\"")
{
  m_mem = new QSharedMemory("MouseSimRunning", this);
  if (!m_mem->create(1)) QTimer::singleShot(0, qApp, SLOT(quit()));

  auto img = QImage(":/icons/mouse_16.png");
  m_activated = new QIcon(QPixmap::fromImage(img));
  auto size = img.size();
  auto width = size.width(),
       height = size.height();

  for (auto i = 0; i < width; ++i) {
    for (auto j = 0; j < height; ++j) {
      auto pixel = img.pixel(i, j);
      auto gray = qGray(pixel);
      auto alpha = qAlpha(pixel);
      img.setPixel(i, j, qRgba(gray, gray, gray, alpha));
    }
  }
  m_deactivated = new QIcon(QPixmap::fromImage(img));

  m_settings = new QSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
  auto checked = m_settings->value("MouseSim", QVariant("")).toString() == m_appPath;

  m_enableAction = new QAction("&Enable", this);
  m_disableAction = new QAction("&Disable", this);
  m_autostartAction = new QAction("Autostart &MouseSim", this);
  m_autostartAction->setCheckable(true);
  m_autostartAction->setChecked(checked);
  m_aboutAction = new QAction("&About MouseSim", this);
  m_quitAction = new QAction("&Quit", this);

  connect(m_enableAction, &QAction::triggered, this, &MouseSim::enable);
  connect(m_disableAction, &QAction::triggered, this, &MouseSim::disable);
  connect(m_autostartAction, &QAction::triggered, this, &MouseSim::autostart);
  connect(m_aboutAction, &QAction::triggered, this, &MouseSim::about);
  connect(m_quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

  m_trayIconMenu = new QMenu(this);
  m_trayIconMenu->addAction(m_enableAction);
  m_trayIconMenu->addAction(m_disableAction);
  m_trayIconMenu->addAction(m_autostartAction);
  m_trayIconMenu->addAction(m_aboutAction);
  m_trayIconMenu->addSeparator();
  m_trayIconMenu->addAction(m_quitAction);

  m_trayIcon = new QSystemTrayIcon(this);
  m_trayIcon->setContextMenu(m_trayIconMenu);

  connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MouseSim::iconActivated);

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &MouseSim::sendInput);

  enable();
  m_trayIcon->show();
}

void MouseSim::iconActivated(QSystemTrayIcon::ActivationReason reason) const
{
  if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
    if (m_timer->isActive()) {
      disable();
    } else {
      enable();
    }
  }
}

void MouseSim::sendInput()
{
  INPUT input;
  input.type = INPUT_MOUSE;
  input.mi.dx = 0;
  input.mi.dy = 0;
  input.mi.mouseData = 0;
  input.mi.dwFlags = MOUSEEVENTF_MOVE;
  input.mi.time = 0;
  input.mi.dwExtraInfo = 0;

  SendInput(1, &input, sizeof(INPUT));
}

void MouseSim::enable() const
{
  m_timer->start(1000);
  m_trayIcon->setIcon(*m_activated);
  m_trayIcon->setToolTip("MouseSim: Enabled");
  sendInput();
  m_enableAction->setDisabled(true);
  m_disableAction->setEnabled(true);
}

void MouseSim::disable() const
{
  m_timer->stop();

  m_trayIcon->setIcon(*m_deactivated);
  m_trayIcon->setToolTip("MouseSim: Disabled");
  m_disableAction->setDisabled(true);
  m_enableAction->setEnabled(true);
}

void MouseSim::autostart(const bool enabled) const
{
  if (enabled) {
    m_settings->setValue("MouseSim", m_appPath);
  } else {
    m_settings->remove("MouseSim");
  }
}

void MouseSim::about()
{
  QMessageBox about(QMessageBox::NoIcon, "About MouseSim", MOUSESIM_ABOUTBOX);
  about.exec();
}
