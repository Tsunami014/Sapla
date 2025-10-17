#include "formElms.hpp"
#include "getCards.hpp"
#include "../items/base/markdown.hpp"
#include <QLabel>

namespace Form {

    void spacing(QBoxLayout* lay, int amount) {
        lay->addSpacing(amount);
    }

    void labelField(QVBoxLayout* lay, const QString& txt) {
        lay->addWidget(new QLabel(txt));
    }

    void textField(QVBoxLayout* lay, QString* initTxt, std::function<void(const QString&)> onChange, bool update) {
        MarkdownEdit* edit = new MarkdownEdit(*initTxt);
        lay->addWidget(edit);

        if (onChange) {
            QObject::connect(edit, &MarkdownEdit::textChanged, [=](){
                if (update) *initTxt = edit->getMarkdown();
                onChange(edit->getMarkdown());
            });
        }
    }

    void textXtraField(QVBoxLayout* lay, SideXtra* init, std::function<void(int, const QString&)> onChange, bool update) {
        QHBoxLayout* lay2 = new QHBoxLayout();

        auto makeTextEdit = [&](QBoxLayout* lay, QString* text, const QString& label, int index, bool mid = false) {
            auto* lab = new QLabel(label);
            lay->addWidget(lab);
            lab->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

            MarkdownEdit* edit = new MarkdownEdit(*text);
            qreal heiOffs;
            if (!mid) {
                heiOffs = 2.5;
            } else {
                heiOffs = 5;
            }
            edit->setMaximumHeight(edit->fontMetrics().height()*(heiOffs+0.2) + edit->frameWidth()*2 + 8);
            lay->addWidget(edit);
            QObject::connect(edit, &MarkdownEdit::textChanged, [=]() {
                if (update) *text = edit->getMarkdown();
                if (onChange) onChange(index, edit->getMarkdown());
                writeCards();
            });
            return edit;
        };

        QVBoxLayout* play = new QVBoxLayout();
        makeTextEdit(play, &init->fprefix, "Front prefx", -2);
        makeTextEdit(play, &init->bprefix, "Back prefx", -1);
        lay2->addLayout(play, 1);

        QVBoxLayout* tlay = new QVBoxLayout();
        makeTextEdit(tlay, &init->txt, "Text:", 0, true);
        lay2->addLayout(tlay, 3);

        QVBoxLayout* blay = new QVBoxLayout();
        makeTextEdit(blay, &init->fsuffix, "Front suffx", 1);
        makeTextEdit(blay, &init->bsuffix, "Back suffx", 2);
        lay2->addLayout(blay, 1);

        lay->addLayout(lay2);
    }

}

