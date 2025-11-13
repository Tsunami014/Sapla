#include "playScn.hpp"
#include "items/cardIt.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

const QString HELP_TXT =
    "Click on card to flip, once flipped; &lt;Space&gt; if you got it wrong, &lt;Enter&gt; if right.\n"
    "Press &lt;Esc&gt; to go back to the home screen.";

PlayScene::PlayScene() : GraphicGameScene(), tr(Tree::getTree()), done(false) {
    helpStr = &HELP_TXT;
    MG->changeBG("pretty");
    card = new CardGraphicItem(":/BIAssets/card.svg", *NextFC());
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
        card->finish();
        done = true;
        if (key == Qt::Key_Space) {
            MG->s.bads++;
            tr.grow(-10);
        } else {
            MG->s.goods++;
            if (tr.grow(50)) {
                return true;
            }
        }
        MG->nextFC();
        return true;
    }
    return false;
}
void PlayScene::resume() {
    if (done) MG->nextFC();
}

void PlayScene::resize() {
    QRectF rect = view.sceneRect();
    tr.setRect(rect);
    card->setRect({rect.x(), rect.y(), rect.width()-tr.boundingRect().width(), rect.height()});
}

