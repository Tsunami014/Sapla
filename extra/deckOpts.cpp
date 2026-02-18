#include "deckOpts.hpp"
#include "../core.hpp"
#include "../menu.hpp"
#include "../notes/getNotes.hpp"
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

    auto* delBtn = new SvgBtn(":/assets/btn.svg");
    delBtn->setPadding(20, -20);
    delBtn->setFont(font2);
    delBtn->setText("Delete deck");
    QObject::connect(delBtn, &SvgBtn::clicked, dialog, [dialog](){
        if (deleteDeck() != -1) {
            dialog->accept();
        }
    });

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(title);
    layout->addWidget(delBtn);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    QObject::connect(dialog, &QDialog::finished, [=](int result){
        MG->curScene->dialogClose();
    });
    dialogging = true;
    MG->curScene->dialogOpen();
    dialog->exec();
}
