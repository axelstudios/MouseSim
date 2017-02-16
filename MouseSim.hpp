#ifndef MOUSESIM_HPP
#define MOUSESIM_HPP

#include <QSystemTrayIcon>
#include <QWidget>

class QAction;
class QMenu;
class QSharedMemory;
class QSettings;

class MouseSim : public QWidget
{
  Q_OBJECT

public:
  MouseSim();

private slots:
  void iconActivated(const QSystemTrayIcon::ActivationReason reason) const;
  static void sendInput();
  void enable() const;
  void disable() const;
  void autostart(const bool enabled) const;
  static void about();

private:
  const QString m_appPath;

  QIcon *m_activated;
  QIcon *m_deactivated;

  QTimer *m_timer;

  QAction *m_enableAction;
  QAction *m_disableAction;
  QAction *m_autostartAction;
  QAction *m_aboutAction;
  QAction *m_quitAction;

  QSystemTrayIcon *m_trayIcon;
  QMenu *m_trayIconMenu;

  QSettings *m_settings;
  QSharedMemory *m_mem;
};

#endif // MOUSESIM_HPP
