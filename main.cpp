#include <QApplication>
#include <QScreen>
#include <QLabel>
#include <QWidget>
#include <QGraphicsView>
#include "main.hpp"

QGraphicsScene* scene = nullptr;
MainGame* MG = nullptr;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    scene = new QGraphicsScene();

    QGraphicsView* view = new QGraphicsView(scene);

    MG = new MainGame();

    // Get the primary screen's available geometry
    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    view->resize(screenRect.width(), screenRect.height());
    view->move(screenRect.topLeft());

    view->show();
    return app.exec();
}
