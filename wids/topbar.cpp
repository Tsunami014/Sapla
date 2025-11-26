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
    gridLay = new QVBoxLayout(grid);
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
            newB->setWordWrap(false);
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
    for (auto* l : gridRows) delete l;
    gridRows.clear();
    QHBoxLayout* hbox = barLay;
    int maxWid = rect().width() - xpandBtn->sizeHint().width() - barLay->spacing();
    for (int i=0; i<btns.size(); i++) {
        auto* btn = btns[i].btn;
        int wid = btn->sizeHint().width() + hbox->spacing()*2;
        maxWid -= wid;
        if (maxWid < 0) {
            hbox = new QHBoxLayout();
            gridRows.push_back(hbox);
            maxWid = rect().width() - wid;
            gridLay->addLayout(hbox);
        }
        hbox->addWidget(btn);
    }

    barLay->addWidget(xpandBtn);
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
    MG->styliseDialog(dialog);

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

