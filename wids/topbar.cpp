#include "topbar.hpp"
#include "../core.hpp"
#include "../menu.hpp"
#include "../notes/features.hpp"
#include "../base/svgRend.hpp"
#include <QDialog>
#include <QTextEdit>

Topbar::Topbar(QWidget* parent) : QWidget(parent) {
    auto* mainLay = new QVBoxLayout(this);

    barLay = new QHBoxLayout();
    xpandBtn = new SvgBtn(":/assets/btn2.svg", this);
    connect(xpandBtn, &SvgBtn::clicked, this, &Topbar::toggleXpanded);

    grid = new QWidget(this);
    gridLay = new QGridLayout(grid);
    settingsBtn = new SvgBtn(":/assets/btn2.svg", this);
    settingsBtn->setText("⚙️");
    connect(settingsBtn, &SvgBtn::clicked, this, &Topbar::makeSettings);
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
            QString help = b.help;
            connect(newB, &SvgBtn::clicked, this, [this, apply, help](){ onBtnPush(apply, help); });
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
    int hei = xpandBtn->fontMetrics().height() * 1.6;
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

Topbar* Topbar::clone() const {
    auto* copy = new Topbar(parentWidget());
    if (xpanded) copy->toggleXpanded();
    return copy;
}

void Topbar::makeSettings() {
    QDialog* dialog = new QDialog;
    dialog->setWindowTitle("Top bar settings");
    dialog->resize(500, 700);
    dialog->setAttribute(Qt::WA_DeleteOnClose); // Auto-delete when closed
    dialog->setStyleSheet(QString(
        "background-color: %1;"
        "color: black;"
        "border-radius: 6px;"
    ).arg(getCol("alight", 30, 30)));

    auto* txtArea = new QTextEdit(dialog);
    txtArea->setReadOnly(true);

    auto* newTB = clone();
    newTB->setParent(dialog);
    newTB->toggleXpanded();
    newTB->xpandBtn->setEnabled(false);
    newTB->settingsBtn->setEnabled(false);
    connect(newTB, &Topbar::onBtnPush, this, [=](const QString& apply, const QString help){
        txtArea->setText(help);
    });

    auto* lay = new QVBoxLayout(dialog);
    lay->addWidget(newTB, 3);
    lay->addWidget(txtArea, 1);

    dialogging = true;
    MG->curScene->dialogOpen();
    QObject::connect(dialog, &QDialog::finished, [=](int result){
        MG->curScene->dialogClose();
    });
    dialog->exec();
}

