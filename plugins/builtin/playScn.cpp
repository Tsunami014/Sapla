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
    card = new CardGraphicItem(":/BIAssets/card.svg", NextFC());
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

    if (card->side == 255 && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
        done = true;
        if (MG->cardFin(card->fc, key != Qt::Key_Space)) return true;
        MG->nextFC();
        return true;
    }
    return false;
}

void PlayScene::resize() {
    QRectF rect = view.sceneRect();
    tr.setRect(rect);
    card->setRect({rect.x(), rect.y(), rect.width()-tr.boundingRect().width(), rect.height()});
}

