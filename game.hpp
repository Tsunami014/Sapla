#pragma once
#include <QMainWindow>
#include <QStackedLayout>
#include "base/svgWid.hpp"
#include "scenes/gameScn.hpp"
#include "wids/svgBtn.hpp"
#include "wids/ovrlWrap.hpp"

class FlashCard;  // Forward reference

struct spareStyls {
    QString listWidCol;
    QString dialogStyl;
    QString logBgCol;
    QString mdHlCol;
};

class MainGame : public QMainWindow {
    Q_OBJECT
public:
    MainGame();
    void initScene();
    void initStyles();
    void styliseDialog(QDialog* dialog, const QString& xtraStyl = "");

    void removeGame();
    void changeScene(BaseScene* newScene, bool resume = false);
    void changeBG(QString bgName);

    bool handleEv(QKeyEvent* event);
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void fixLogs();

    int cardFin(FlashCard* card, int key);
    QVBoxLayout logLay;
    OverlayWrapper logLayWrap;
    BaseScene* curScene;
    spareStyls styls;

public slots:
    void nextFC();
    void showFC();

private:
    SvgWidget* bg;
    QPointer<GameScene> curGame;
    QWidget* sceneStash;
};
