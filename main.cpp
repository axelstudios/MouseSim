#include <QApplication>
#ifdef BUILD_STATIC
#include <QtPlugin>
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

#include "MouseSim.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  app.setApplicationName("MouseSim");
  app.setQuitOnLastWindowClosed(false);

  MouseSim mousesim;

  return app.exec();
}
