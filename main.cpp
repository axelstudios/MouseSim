#include <QApplication>

#include "MouseSim.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  MouseSim mouseSim;

  return app.exec();
}
