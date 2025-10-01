#include "game.hpp"
#include "main.hpp"

MainGame::MainGame() {
    bg = new SvgGraphicItem(":/assets/bg.svg");
    MScene->addItem(bg);

    MScene->addRect(0, 0, 100, 100);
}

void MainGame::resizeEvent(QRectF& newSze) {
    bg->setRect(newSze);
}

