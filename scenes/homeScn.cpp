#include "homeScn.hpp"
#include "browseScn.hpp"
#include "../items/base/font.hpp"
#include "../core.hpp"
#include <QTextOption>
#include <QTextDocument>

const QString HELP_TXT = "Just press a button to do stuff";

HomeScene::HomeScene() : BaseScene() {
    helpStr = &HELP_TXT;
    MG->changeBG("pretty");
    txt = new QGraphicsTextItem(this);
    txt->setHtml("<b>Sapla</b>");
    txt->setDefaultTextColor(QColor(149, 69, 53));
    QTextOption opt = txt->document()->defaultTextOption();
    opt.setAlignment(Qt::AlignHCenter);
    txt->document()->setDefaultTextOption(opt);

    playBtn = new TxtBtnItem(":/assets/btn.svg", "Play!", this);
    playBtn->setTxtColour(QColor(184, 115, 51));
    playBtn->onClick = []() {
        MG->nextFC();
    };
    browseBtn = new TxtBtnItem(":/assets/btn.svg", "Browse cards", this);
    browseBtn->setTxtColour(QColor(184, 115, 51));
    browseBtn->onClick = []() {
        MG->changeScene(new BrowseScene());
    };
}

void HomeScene::resize() {
    txt->setFont(getFont(rect.height()*0.2));
    txt->setTextWidth(rect.width());

    playBtn->setFont(getFont(rect.height()*0.1));
    playBtn->setPos(QPointF(
        (rect.width() - playBtn->boundingRect().width())/2, rect.height() * 0.35
    ));
    browseBtn->setFont(getFont(rect.height()*0.1));
    browseBtn->setPos(QPointF(
        (rect.width() - browseBtn->boundingRect().width())/2, rect.height() * 0.36 + playBtn->boundingRect().height()
    ));
}
