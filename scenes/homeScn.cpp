#include "homeScn.hpp"
#include "browseScn.hpp"
#include "pluginView.hpp"
#include "xtra/lineEdit2.hpp"
#include "../extra/deckOpts.hpp"
#include "../base/font.hpp"
#include "../base/svgRend.hpp"
#include "../wids/svgBtn.hpp"
#include "../notes/decks.hpp"
#include "../notes/getNotes.hpp"
#include "../core.hpp"
#include "../help.hpp"
#include <QLabel>
#include <QLineEdit>
#include <QTextOption>
#include <QBoxLayout>

const QString HELP_TXT = HOME_HELP;

HomeScene::HomeScene() : BaseScene(), deleteTmps("Delete temporary decks", Menues->FileMenu) {
    helpStr = &HELP_TXT;
    MG->changeBG("pretty");

    auto font2 = getFont(2);
    auto font3 = getFont(3);

    auto* lay = new QVBoxLayout(this);
    lay->addStretch(1);
    auto* txt = new QLabel("<h1>Sapla</h1>", this);
    txt->setFont(font2);
    txt->setAlignment(Qt::AlignHCenter);
    lay->addWidget(txt, 2);

    auto* playBtn = new SvgBtn(":/assets/btn.svg", this);
    playBtn->setFont(font3);
    playBtn->setText("Play!");
    connect(playBtn, &SvgBtn::clicked, this, [](){ MG->showFC(); });
    auto* hlay1 = new QHBoxLayout();
    hlay1->addStretch();
    hlay1->addWidget(playBtn);
    hlay1->addStretch();

    auto* browseBtn = new SvgBtn(":/assets/btn.svg", this);
    browseBtn->setFont(font3);
    browseBtn->setText("Browse cards");
    connect(browseBtn, &SvgBtn::clicked, this, [](){ MG->changeScene(new BrowseScene()); });
    auto* GVBtn = new SvgBtn(":/assets/btn.svg", this);
    GVBtn->setFont(font3);
    GVBtn->setText("Manage plugins");
    connect(GVBtn, &SvgBtn::clicked, this, [](){ MG->changeScene(new PlugViewScene()); });
    auto* hlay2 = new QHBoxLayout();
    hlay2->addStretch();
    hlay2->addWidget(browseBtn);
    hlay2->addWidget(GVBtn);
    hlay2->addStretch();

    auto* deckLabl = new QLabel("<h3>Deck: </h3>", this);
    deckLabl->setFont(font2);
    deckLabl->setAlignment(Qt::AlignCenter);
    deckNam = new QComboBox(this);
    deckNam->setEditable(true);
    deckNam->setFont(font2);
    deckNam->lineEdit()->setFont(font2);
    deckNam->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* deckLabl2 = new QLabel("<h3> Rename: </h3>", this);
    deckLabl2->setFont(font2);
    deckLabl2->setAlignment(Qt::AlignCenter);
    auto* deckNam2 = new LineEdit2(this);
    deckNam2->setFont(font2);
    deckNam2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* optsBtn = new SvgBtn(":/assets/btn.svg", this);
    optsBtn->setPadding(20, -20);
    optsBtn->setFont(font2);
    optsBtn->setText("Deck options");
    auto indexChanged = [=](int index){
        if (index < 0 || index >= decks.size()) return;
        if (!deckNam2) return;
        changeDeck(decks[index]);
        QSignalBlocker blocker(deckNam2);
        deckNam2->setText(decks[index]);
        blocker.unblock();

        QPalette palette = deckNam2->palette();
        palette.setColor(QPalette::Text, getQCol("alight", 40, 50, 15));
        deckNam2->setPalette(palette);
    };
    auto delfn = [=](){
        int idx = deleteDeck();
        if (idx == -1) return;
        QSignalBlocker blocker(deckNam);
        deckNam->removeItem(idx);
        deckNam->setCurrentIndex(0);
        blocker.unblock();
        indexChanged(0);
    };
    connect(deckNam2, &LineEdit2::emptyBackspace, this, delfn);
    connect(deckNam2, &QLineEdit::textChanged, this, [=](const QString& newtxt){
        int idx = renameDeck(newtxt);
        if (idx != -2) {
            QSignalBlocker blocker(deckNam);
            if (idx >= 0) {
                deckNam->setItemText(idx, newtxt);
            } else if (idx < -1) {
                deckNam->clear();
                deckNam->addItems(QStringList(decks.begin(), decks.end()));
                deckNam->setCurrentIndex(-idx - 10);
                idx = 0;
            }
        }

        QString col;
        if (idx >= 0 || idx == -2) {
            col = getCol("alight", 80, 120);
        } else {
            col = getCol("red", 20, 40, 20);
        }
        deckNam2->setStyleSheet(QString("QLineEdit { color: %1 }").arg(col));
    });
    connect(deckNam, &QComboBox::currentIndexChanged, this, indexChanged);
    connect(optsBtn, &SvgBtn::clicked, this, showDeckOpts);
    connect(&deleteTmps, &QAction::triggered, this, [this](){
        clearTempNotes();
        initNotes();
        dialogClose(); // For setting the items and idx
    });

    auto* hlay3 = new QHBoxLayout();
    hlay3->addStretch();
    hlay3->addWidget(deckLabl);
    hlay3->addWidget(deckNam);
    hlay3->addWidget(optsBtn);
    hlay3->addStretch();
    auto* hlay4 = new QHBoxLayout();
    hlay4->addStretch();
    hlay4->addWidget(deckLabl2);
    hlay4->addWidget(deckNam2);
    hlay4->addStretch();

    lay->addLayout(hlay1, 1);
    lay->addLayout(hlay2, 1);
    lay->addLayout(hlay3, 1);
    lay->addLayout(hlay4, 1);
    lay->addStretch(2);
    dialogClose(); // For setting the items and idx
}
void HomeScene::dialogClose() {
    QSignalBlocker blocker(deckNam);
    deckNam->clear();
    deckNam->addItems(QStringList(decks.begin(), decks.end()));
    int idx = deckIdx();
    deckNam->setCurrentIndex(idx);
    blocker.unblock();
    emit deckNam->currentIndexChanged(idx);
}

