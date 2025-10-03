#include "game.hpp"
#include "main.hpp"

MainGame::MainGame(BaseScene* scene) {
    bg = new SvgGraphicItem(":/assets/bg.svg");
    bg->setZValue(-100);
    MScene->addItem(bg);

    curScene = scene;
    MScene->addItem(scene);
}

void MainGame::changeScene(BaseScene* newScene) {
    MScene->removeItem(curScene);
    delete curScene;
    curScene = newScene;
    MScene->addItem(newScene);
}

void MainGame::resizeEvent(QRectF& newSze) {
    bg->setRect(newSze);
    curScene->setRect(newSze);
}

