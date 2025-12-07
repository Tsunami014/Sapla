#include "playScn.hpp"
#include "items/cardLayouts.hpp"
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
}
PlayScene::PlayScene()
    : GraphicGameScene(), main(new GLayoutGraphicItem()), tr(Tree::getTree()) {
        scn.addItem(main);
        scn.addItem(&tr);

        schdT = new QGraphicsTextItem();
        scn.addItem(schdT);
        schdT->setZValue(5);
        styleScheduleInfTxt(schdT);

        overlay = NULL;

        while (true) {
            GetFlashCard fc = GetFlashCard();
            auto lay = Single;
            auto* nCGI = new GridCGI(lay.fname, fc);
            if (!main->addItem(nCGI, lay)) {
                delete nCGI;
                break;
            }
        }
        main->update();
        resume();
    }
PlayScene::~PlayScene() {
    if (tr.scene() == &scn) {
        scn.removeItem(&tr);
    }
    scn.clear();
}

void PlayScene::cardClicked(FlashCard* card) {
    setScheduleInfTxt(schdT, card);
}

bool PlayScene::keyEv(QKeyEvent* event) {
    if (MG->handleEv(event)) return true;
    int key = event->key();

    if (hasOverlay()) {
        GridCGI* git;
        int resp;
        for (auto& it : main->grid) {
            if (it.item->side == 255) {
                git = it.item;
                resp = MG->cardFin(it.item->fc, key);
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
    int trW = tr.boundingRect().width();
    main->setRect({rect.x(), rect.y(), rect.width()-trW, rect.height()});
    if (overlay != NULL) {
        overlay->setRect(rect);
    }
    int thei = rect.height()*0.35;
    QRectF rec(rect.x()+rect.width()-trW, rect.y()+rect.height()-thei, trW, thei);
    schdT->document()->setTextWidth(rec.width());
    schdT->setPos(rec.topLeft());
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

