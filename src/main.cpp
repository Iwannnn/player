#include <QApplication>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_AUDIO);
  QApplication app(argc, argv);
  MainWindow w;
  w.show();
  return app.exec();
}