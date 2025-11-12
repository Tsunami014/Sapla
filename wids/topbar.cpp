#include "topbar.hpp"
#include "../notes/features.hpp"

Topbar::Topbar(QWidget* parent) : QWidget(parent) {
    auto* mainLay = new QVBoxLayout(this);

    barLay = new QHBoxLayout();
    xpandBtn = new SvgBtn(":/assets/btn2.svg", this);
    connect(xpandBtn, &SvgBtn::clicked, this, &Topbar::toggleXpanded);

    grid = new QWidget(this);
    gridLay = new QGridLayout(grid);
    settingsBtn = new SvgBtn(":/assets/btn2.svg", this);
    settingsBtn->setText("⚙️");
    // connect(settingsBtn, &SvgBtn::clicked, this, &Topbar::);
    settings = Btns{settingsBtn, "Edit the settings for this bar"};

    mainLay->addLayout(barLay);
    mainLay->addWidget(grid);

    createItems();
    resize();
    xpanded = true;  // Force close
    close();
}
void Topbar::createItems() {
    for (auto& f : Feats) {
        for (auto& b : f->btns()) {
            auto* newB = new SvgBtn(":/assets/btn2.svg", this);
            newB->setText(b.label);
            newB->setToolTip(b.context);
            QString apply = b.apply;
            connect(newB, &SvgBtn::clicked, this, [this, apply](){ onBtnPush(apply); });
            btns.push_back({newB, b.help});
            if (b.shortcut) {
                auto found = cuts.find(b.shortcut->key);
                if (found != cuts.end()) {
                    found->second.emplace_back(b.shortcut->mods, apply);
                }
            }
        }
    }
    btns.push_back(settings);
}

void Topbar::resize() {
    int hei = xpandBtn->fontMetrics().height() * 1.5;
    int wid = hei * 2;
    int fatness = wid * 1.05;  // layout width
    int maxWid = rect().width();
    int perRow = maxWid/fatness;
    if (perRow == 0) perRow = 1;

    for (int i=0; i<btns.size(); i++) {
        if (i >= perRow) {
            btns[i].btn->setParent(grid); 
            // We have to do this funky thing because the first row has 1 less item
            gridLay->addWidget(btns[i].btn, int((i+1)/perRow)-1, (i+1)%perRow);
        } else {
            btns[i].btn->setParent(this); 
            barLay->addWidget(btns[i].btn);
        }
        btns[i].btn->setFixedSize(wid, hei);
    }
    barLay->addWidget(xpandBtn);
    xpandBtn->setFixedSize(wid, hei);
}

void Topbar::close() {
    if (xpanded) toggleXpanded();
}
void Topbar::toggleXpanded() {
    xpanded = !xpanded;
    grid->setVisible(xpanded);
    xpandBtn->setText(xpanded ? "▲" : "▼");
}

