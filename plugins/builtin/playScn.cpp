#include "core.hpp"
#include "help.hpp"
#include "playScn.hpp"
#include "items/scheduleInfTxt.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QDir>

void PlayScene::resume() {
    if (done) return MG->nextFC();
    helpStr = &GAME_HELP;
    MG->changeBG("pretty");
    dp.upd();
}
PlayScene::PlayScene() : GraphicGameScene(), tr(), done(false), dp() {
    resume();
    GetFlashCard gfc{};
    schdT = getScheduleInfTxt(gfc);
    scn.addItem(schdT);
    card = new CardGraphicItem(":/BIAssets/card.svg", gfc, 2.5);
    scn.addItem(card);
    scn.addItem(&tr);
    scn.addItem(&dp);
}
PlayScene::~PlayScene() {
    if (tr.scene() == &scn) {
        scn.removeItem(&tr);
    }
    if (dp.scene() == &scn) {
        scn.removeItem(&dp);
    }
    delete card;
}
bool PlayScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    int key = event->key();

    if (card->side == 255) {
        switch (MG->cardFin(card->fc, key, &tr)) {
            case 0:
                card->fc.finish();
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
    dp.setRect(rect);
    auto trRec = tr.getRect();
    auto dpRec = dp.boundingRect();
    card->setRect({
        dpRec.right(),
        rect.y(),
        rect.width()-trRec.width()-(dpRec.right()-rect.left()),
        rect.height()
    });

    schdT->document()->setTextWidth(trRec.width());
    schdT->setPos(trRec.left(), trRec.bottom()+rect.height()*0.05);
}

