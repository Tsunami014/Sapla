#include "deckOpts.hpp"
#include "../core.hpp"
#include "../menu.hpp"
#include "../notes/decks.hpp"
#include "../wids/svgBtn.hpp"
#include "../base/font.hpp"
#include "../scenes/homeScn.hpp"
#include <QDialog>
#include <QVBoxLayout>

void showDeckOpts() {
    if (!checkValidDeck()) return;
    QDialog* dialog = new QDialog;
    dialog->setWindowTitle("Deck options");
    dialog->setAttribute(Qt::WA_DeleteOnClose); // Auto-delete when closed
    MG->styliseDialog(dialog);
    auto font2 = getFont(2);

    auto title = new QLabel(QString("<h1>Deck options for %1:</h1>").arg(curDeck));
    title->setFont(font2);

    auto mkBtn = [dialog, font2](QString txt, auto func){
        auto* btn = new SvgBtn(":/assets/btn.svg");
        btn->setPadding(20, -20);
        btn->setFont(font2);
        btn->setText(txt);
        QObject::connect(btn, &SvgBtn::clicked, dialog, func);
        return btn;
    };

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(title);
    layout->addWidget(mkBtn("Delete deck", [dialog](){
        if (deleteDeck() != -1) {
            dialog->accept();
        }
    }));
    layout->addWidget(mkBtn("Copy deck", [dialog](){
        if (copyDeck() != -1) {
            dialog->accept();
        }
    }));

    layout->setSizeConstraint(QLayout::SetFixedSize);
    QObject::connect(dialog, &QDialog::finished, [=](int result){
        MG->curScene->dialogClose();
    });
    dialogging = true;
    MG->curScene->dialogOpen();
    dialog->exec();
}
