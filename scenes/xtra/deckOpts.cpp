#include "deckOpts.hpp"
#include "core.hpp"
#include "menu.hpp"
#include "notes/decks.hpp"
#include "notes/eximport.hpp"
#include "wids/svgBtn.hpp"
#include "base/font.hpp"
#include "../homeScn.hpp"
#include <QDialog>
#include <QCheckBox>
#include <QVBoxLayout>

void showDeckOpts() {
    if (!checkValidDeck()) return;
    QDialog* dialog = new QDialog;
    dialog->setWindowTitle("Deck options");
    dialog->setAttribute(Qt::WA_DeleteOnClose); // Auto-delete when closed
    MG->styliseDialog(dialog);
    auto font2 = getFont(2);

    auto mkBtn = [dialog, font2](QString txt, auto func){
        auto* btn = new SvgBtn(":/assets/btn.svg");
        btn->setPadding(0, -20);
        btn->setFont(font2);
        btn->setText(txt);
        QObject::connect(btn, &SvgBtn::clicked, dialog, func);
        return btn;
    };

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    {
        auto title = new QLabel(QString("<h2>Deck options for %1:</h2>").arg(curDeck));
        title->setFont(font2);
        layout->addWidget(title);
    } {
        auto* lay2 = new QHBoxLayout();
        lay2->addWidget(mkBtn("Delete deck", [dialog](){
            if (deleteDeck(false) != -1)
                dialog->accept();
        }));
        lay2->addWidget(mkBtn("Export deck", [dialog](){
            dialog->accept();
            tryExport();
        }));
        layout->addLayout(lay2);
    }

    layout->addSpacing(20);
    SvgBtn* cpybtn;
    {
        auto* lay2 = new QHBoxLayout();
        auto title2 = new QLabel("<h3>Copy deck options</h3>");
        title2->setFont(font2);
        title2->setAlignment(Qt::AlignCenter);
        lay2->addWidget(title2);

        cpybtn = new SvgBtn(":/assets/btn.svg");
        cpybtn->setPadding(0, -20);
        cpybtn->setFont(font2);
        cpybtn->setText("Copy deck");

        lay2->addWidget(cpybtn);
        layout->addLayout(lay2);
    }

    auto temp = new QCheckBox("Temporarily copy", dialog);
    temp->setFont(font2);
    temp->setCheckState(Qt::Checked);
    layout->addWidget(temp);

    QObject::connect(cpybtn, &SvgBtn::clicked, dialog, [=](){
        if (copyDeck(Copy_Regular, temp->checkState() == Qt::Checked)) {
            dialog->accept();
        }
    });

    layout->addSpacing(20);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    QObject::connect(dialog, &QDialog::finished, [=](int result){
        dialogging = false;
        MG->curScene->dialogClose();
    });
    dialogging = true;
    MG->curScene->dialogOpen();
    dialog->exec();
}
