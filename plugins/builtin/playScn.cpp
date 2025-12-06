#include "playScn.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

const QString HELP_TXT =
    "Click on card to flip, once flipped; &lt;Space&gt; if you got it wrong, &lt;Enter&gt; if right.\n"
    "Press &lt;Esc&gt; to go back to the home screen.";

void PlayScene::resume() {
    if (done) return MG->nextFC();
    helpStr = &HELP_TXT;
    MG->changeBG("pretty");
}
PlayScene::PlayScene() : GraphicGameScene(), tr(Tree::getTree()), done(false) {
    resume();
    GetFlashCard gfc{};
    schdT = getScheduleInfTxt(gfc);
    scn.addItem(schdT);
    card = new CardGraphicItem(":/BIAssets/card.svg", gfc);
    scn.addItem(card);
    scn.addItem(&tr);
}
PlayScene::~PlayScene() {
    if (tr.scene() == &scn) {
        scn.removeItem(&tr);
    }
    delete card;
}
bool PlayScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    int key = event->key();

    if (card->side == 255) {
        switch (MG->cardFin(card->fc, key)) {
            case 0:
                MG->nextFC();
            case 1:
                done = true;
                return true;
        }
    }
    return false;
}

void PlayScene::resize() {
    QRectF rect = view.sceneRect();
    tr.setRect(rect);
    int trW = tr.boundingRect().width();
    card->setRect({rect.x(), rect.y(), rect.width()-trW, rect.height()});

    int thei = rect.height()*0.35;
    QRectF rec(rect.x()+rect.width()-trW, rect.y()+rect.height()-thei, trW, thei);
    schdT->document()->setTextWidth(rec.width());
    schdT->setPos(rec.topLeft());
}

