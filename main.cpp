#include <QApplication>
#include <QLabel>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget window;

    QGraphicsScene *scene = new QGraphicsScene();
    scene->addRect(0, 0, 100, 100);

    QGraphicsView *view = new QGraphicsView(scene);

    QVBoxLayout *layout = new QVBoxLayout(&window);
    layout->addWidget(view);
    layout->setContentsMargins(0,0,0,0);

    window.resize(800, 600);
    window.show();
    return app.exec();
}

