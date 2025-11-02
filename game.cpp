#include "game.hpp"
#include "core.hpp"
#include "scenes/homeScn.hpp"
#include "scenes/browseScn.hpp"
#include "scenes/gameView.hpp"
#include "games/getGames.hpp"
#include "base/font.hpp"
#include "base/svgRend.hpp"
#include <QRandomGenerator>
#include <QTimer>
#include <QKeyEvent>

MainGame::MainGame() : s{0, 0}, logLay(), logLayWrap(this) {
    setFont(getFont(1.5));
    setStyleSheet(
        "QTextEdit {"
            "background: rgba(110, 60, 30, 220);"
            "border-radius: 6px;"
            "border: 1px solid #666;"
            "margin: 4px;"
            "padding: 4px;"
        "}"
        "QTextEdit:focus {"
            "border: 2px solid #333;"
        "}"
        "SvgBtn {"
            "color: black;"
        "}"
    );
    bg = new SvgWidget(this);
    logLayWrap.setLayout(&logLay);
}
void MainGame::initScene() {
    curScene = new HomeScene();
    setCentralWidget(curScene);
    bg->lower();
    logLayWrap.raise();
}

void MainGame::changeBG(QString bgName) {
    QString pth = ":/assets/bgs/"+bgName+".svg";
    bg->renderer()->load(RenderSvg(pth));
}

void MainGame::changeScene(BaseScene* newScene) {
    BaseScene* oldScene = curScene;
    curScene = newScene;
    QTimer::singleShot(0, this, [this, newScene]() {
        setCentralWidget(newScene);
        bg->lower();
        logLayWrap.raise();
    });
}

void MainGame::showFC() {
    nextFC();  // TODO: Make these different
}
void MainGame::nextFC() {
    int gSze = games.size();
    if (gSze == 0) return;
    int gidx = QRandomGenerator::global()->bounded(gSze);
    for (int _ = 0; _ < gSze; _++) {
        bool success = games[gidx]->run();
        if (success) break;
        gidx = (gidx + 1) % gSze;
    }
}

bool MainGame::handleEv(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            MG->changeScene(new HomeScene());
            return true;
        case Qt::Key_B:
            MG->changeScene(new BrowseScene());
            return true;
        case Qt::Key_G:
            MG->changeScene(new GameViewScene());
            return true;
        case Qt::Key_P:
            MG->showFC();
            return true;
    }
    return false;
}

void MainGame::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    bg->setGeometry(rect());
    fixLogs();
    bg->lower();
    logLayWrap.raise();
}
void MainGame::fixLogs() {
    const int margin = 8;
    int w = width() / 3;
    logLayWrap.setFixedWidth(w);
    logLayWrap.adjustSize();

    int h = logLayWrap.height();
    logLayWrap.move(width() - w - margin, height() - h - margin);
    logLayWrap.resize(w, h);
}

