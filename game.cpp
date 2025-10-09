#include "game.hpp"
#include "main.hpp"

MainGame::MainGame(BaseScene* scene) {
    bg = new SvgGraphicItem(":/assets/bg.svg");
    bg->setZValue(-100);
    MScene->addItem(bg);

    curScene = scene;
    curScene->setZValue(-1);
    MScene->addItem(scene);
}

void MainGame::changeScene(BaseScene* newScene) {
    MScene->removeItem(curScene);
    delete curScene;
    MScene->views()[0]->viewport()->unsetCursor();
    curScene = newScene;
    MScene->addItem(newScene);
    newScene->setRect(bg->boundingRect());
}

void MainGame::resizeEvent(QRectF& newSze) {
    bg->setRect(newSze);
    curScene->setRect(newSze);
}

