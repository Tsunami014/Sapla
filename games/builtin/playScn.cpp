#include "playScn.hpp"
#include "winScn.hpp"
#include "items/cardIt.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

const QString HELP_TXT =
    "Click on card to flip, once flipped; &lt;Space&gt; if you got it wrong, &lt;Enter&gt; if right.\n"
    "Press &lt;Esc&gt; to go back to the home screen.";

PlayScene::PlayScene() : BaseScene(), tr(this) {
    helpStr = &HELP_TXT;
    MG->changeBG("pretty");
    card = new CardGraphicItem(":/BIAssets/card.svg", *NextFC());
    MScene->addItem(card);
}
PlayScene::~PlayScene() {
    delete card;
}

void PlayScene::onEvent(QEvent* event) {
    if (MG->handleEv(event)) return;
    if (event->type() == QEvent::KeyPress) {
        auto* keyEvent = (QKeyEvent*)event;
        int key = keyEvent->key();

        if (card->side == 255 && (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)) {
            if (key == Qt::Key_Space) {
                MG->s.bads++;
                tr.grow(-10);
            } else {
                MG->s.goods++;
                if (!tr.grow(50)) {
                    MG->changeScene(new WinScene());
                }
            }
            card->finish();
            if (!tr.isDone()) {
                delete card;
                card = new CardGraphicItem(":/BIAssets/card.svg", *NextFC());
                MScene->addItem(card);
                resize();
            }
        }
    }
}

void PlayScene::resize() {
    tr.setRect(rect);
    card->setRect({rect.x(), rect.y(), rect.width()-tr.boundingRect().width(), rect.height()});
}

