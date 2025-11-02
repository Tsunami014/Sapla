#pragma once
#include <QMainWindow>
#include <QStackedLayout>
#include "base/svgWid.hpp"
#include "scenes/baseScn.hpp"
#include "wids/svgBtn.hpp"

struct Stats {  // TODO: Save/load
    uint32_t goods;
    uint32_t bads;
};

class Flicker; // forward ref
class MainGame : public QMainWindow {
    Q_OBJECT
public:
    MainGame();
    void initScene();

    void changeScene(BaseScene* newScene);
    void changeBG(QString bgName);

    bool handleEv(QKeyEvent* event);
    void resizeEvent(QResizeEvent *event);
    void fixLogs();

    Stats s;
    QVBoxLayout logLay;
    BaseScene* curScene;

public slots:
    void nextFC();
    void showFC();

private:
    SvgWidget* bg;
    QWidget logLayWrap;
    Flicker* f;
};
