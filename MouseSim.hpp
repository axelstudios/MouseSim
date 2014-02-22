#ifndef MOUSESIM_HPP
#define MOUSESIM_HPP

#include <QSystemTrayIcon>
#include <QWidget>

class QAction;
class QMenu;

class MouseSim : public QWidget
{
  Q_OBJECT

 public:
  MouseSim();

 private slots:
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void sendInput();
  void enable();
  void disable();

 private:
  QTimer *m_timer;

  QAction *enableAction;
  QAction *disableAction;
  QAction *quitAction;

  QSystemTrayIcon *trayIcon;
  QMenu *trayIconMenu;
};

#endif MOUSESIM_HPP
