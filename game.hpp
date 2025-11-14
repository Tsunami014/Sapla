#pragma once
#include <QMainWindow>
#include <QStackedLayout>
#include "base/svgWid.hpp"
#include "scenes/gameScn.hpp"
#include "wids/svgBtn.hpp"
#include "wids/ovrlWrap.hpp"

class FlashCard;  // Forward reference
struct Stats {  // TODO: Save/load
    uint32_t goods;
    uint32_t bads;
};

class MainGame : public QMainWindow {
    Q_OBJECT
public:
    MainGame();
    void initScene();

    void changeScene(BaseScene* newScene, bool resume = false);
    void changeBG(QString bgName);

    bool handleEv(QKeyEvent* event);
    void resizeEvent(QResizeEvent *event);
    void fixLogs();

    bool cardFin(const FlashCard& card, bool correct);
    Stats s;
    QVBoxLayout logLay;
    OverlayWrapper logLayWrap;
    BaseScene* curScene;

public slots:
    void nextFC();
    void showFC();

private:
    SvgWidget* bg;
    QPointer<GameScene> curGame;
    QPointer<BaseScene> pendingScn;
    QWidget* sceneStash;
    bool resume;
};
