#include "core.hpp"
#include "help.hpp"
#include "playScn.hpp"
#include "gridIts/cardLayouts.hpp"
#include "items/scheduleInfTxt.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QRandomGenerator>

void PlayScene::resume() {
    if (main->grid.empty()) {
        MG->nextFC();
        return;
    }
    helpStr = &GAME_HELP;
    MG->changeBG("pretty");
    dp.upd();
}
PlayScene::PlayScene()
    : GraphicGameScene(), main(new GLayoutGraphicItem()), tr(), dp() {
        scn.addItem(main);
        scn.addItem(&tr);
        scn.addItem(&dp);

        schdT = new QGraphicsTextItem();
        scn.addItem(schdT);
        schdT->setZValue(5);
        styleScheduleInfTxt(schdT);

        overlay = NULL;

        uint8_t amnt = QRandomGenerator::global()->bounded(1, GLayoutGraphicItem::Cols * 2 - 1);
        for (uint8_t i = 0; i < amnt; i++) {
            GetFlashCard fc = GetFlashCard();
            auto lay = Single;
            auto* nCGI = new GridCGI(lay.fname, fc);
            if (!main->addItem(nCGI, lay)) {
                delete nCGI;
                break;
            }
            QObject::connect(nCGI, &CardGraphicItem::flipped, nCGI, [=](bool back){
                if (back) setScheduleInfTxt(schdT, nCGI->fc);
                else schdT->setPlainText("");
            });
        }
        main->update();
        resume();
    }
PlayScene::~PlayScene() {
    if (tr.scene() == &scn)
        scn.removeItem(&tr);
    if (dp.scene() == &scn)
        scn.removeItem(&dp);
    scn.clear();
}

bool PlayScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    int key = event->key();

    if (hasOverlay()) {
        GridCGI* git;
        int resp = -1;
        for (auto& it : main->grid) {
            if (it.item->side == 255) {
                git = it.item;
                resp = MG->cardFin(it.item->fc, key, &tr);
                break;
            }
        }
        if (resp == -1 || !git) return false;
        schdT->setPlainText("");
        removeOverlay();
        scn.removeItem(git);
        main->removeItem(git);
        delete git;
        main->update();
        main->updateAllChildren();
        if (resp == 0) resume();  // Only for checking if empty
        return true;
    }
    return false;
}

void PlayScene::resize() {
    QRectF rect = view.sceneRect();
    tr.setRect(rect);
    dp.setRect(rect);
    auto trRec = tr.getRect();
    auto dpRec = dp.boundingRect();
    main->setRect({
        dpRec.right(),
        rect.y(),
        rect.width()-trRec.width()-(dpRec.right()-rect.left()),
        rect.height()
    });
    if (overlay != NULL)
        overlay->setRect(rect);

    schdT->document()->setTextWidth(trRec.width());
    schdT->setPos(trRec.left(), trRec.bottom()+rect.height()*0.05);
}

void PlayScene::setOverlay(QGraphicsRectItem* ovl) {
    overlay = ovl;
    overlay->setRect(view.sceneRect());
}
bool PlayScene::hasOverlay() {
    return overlay != NULL;
}
void PlayScene::removeOverlay() {
    if (hasOverlay()) {
        scn.removeItem(overlay);
        delete overlay;
        overlay = NULL;
    }
}

