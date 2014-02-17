#include "MouseSim.hpp"

#include <QAction>
#include <QCoreApplication>
#include <QMenu>
#include <QTimer>

#include <Windows.h>

MouseSim::MouseSim()
{
  enableAction = new QAction(tr("&Enable"), this);
  connect(enableAction, SIGNAL(triggered()), this, SLOT(enable()));

  disableAction = new QAction(tr("&Disable"), this);
  connect(disableAction, SIGNAL(triggered()), this, SLOT(disable()));

  quitAction = new QAction(tr("&Quit"), this);
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(enableAction);
  trayIconMenu->addAction(disableAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setToolTip("MouseSim: Enabled");
  trayIcon->setContextMenu(trayIconMenu);

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(sendInput()));

  enable();
  trayIcon->show();
}

void MouseSim::iconActivated(QSystemTrayIcon::ActivationReason reason)
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

void MouseSim::enable()
{
  m_timer->start(1000);
  trayIcon->setIcon(QIcon(":/images/mouse.png"));
  trayIcon->setToolTip("MouseSim: Enabled");
  sendInput();
  enableAction->setDisabled(true);
  disableAction->setEnabled(true);
}

void MouseSim::disable()
{
  m_timer->stop();
  trayIcon->setIcon(QIcon(":/images/mouse-gray.png"));
  trayIcon->setToolTip("MouseSim: Disabled");
  disableAction->setDisabled(true);
  enableAction->setEnabled(true);
}
