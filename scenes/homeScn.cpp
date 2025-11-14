#include "homeScn.hpp"
#include "browseScn.hpp"
#include "pluginView.hpp"
#include "../base/font.hpp"
#include "../wids/svgBtn.hpp"
#include "../core.hpp"
#include <QLabel>
#include <QTextOption>
#include <QBoxLayout>

const QString HELP_TXT = "Just press a button to do stuff";

HomeScene::HomeScene() : BaseScene() {
    helpStr = &HELP_TXT;
    MG->changeBG("pretty");

    auto* lay = new QVBoxLayout(this);
    lay->addStretch(1);
    auto* txt = new QLabel(this);
    txt->setFont(getFont(2));
    txt->setText("<h1>Sapla</h1>");
    txt->setStyleSheet("color: #954535;");
    txt->setAlignment(Qt::AlignHCenter);
    lay->addWidget(txt, 2);

    auto* playBtn = new SvgBtn(":/assets/btn.svg", this);
    playBtn->setFont(getFont(3));
    playBtn->setText("Play!");
    connect(playBtn, &SvgBtn::clicked, this, [](){ MG->showFC(); });
    auto* hlay1 = new QHBoxLayout();
    hlay1->addStretch();
    hlay1->addWidget(playBtn);
    hlay1->addStretch();

    auto* browseBtn = new SvgBtn(":/assets/btn.svg", this);
    browseBtn->setFont(getFont(3));
    browseBtn->setText("Browse cards");
    connect(browseBtn, &SvgBtn::clicked, this, [](){ MG->changeScene(new BrowseScene()); });
    auto* GVBtn = new SvgBtn(":/assets/btn.svg", this);
    GVBtn->setFont(getFont(3));
    GVBtn->setText("Manage plugins");
    connect(GVBtn, &SvgBtn::clicked, this, [](){ MG->changeScene(new PlugViewScene()); });
    auto* hlay2 = new QHBoxLayout();
    hlay2->addStretch();
    hlay2->addWidget(browseBtn);
    hlay2->addWidget(GVBtn);
    hlay2->addStretch();

    lay->addLayout(hlay1, 1);
    lay->addLayout(hlay2, 1);
    lay->addStretch(2);
}

