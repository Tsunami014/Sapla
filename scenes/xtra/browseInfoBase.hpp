#include "core.hpp"
#include "base/flowlayout.hpp"
#include <QDialog>
#include <QTextEdit>
#include <QMap>


class Dialog {
public:
    Dialog() {
        dialog = new QDialog;
        dialog->resize(500, 700);
        dialog->setAttribute(Qt::WA_DeleteOnClose); // Auto-delete when closed
        MG->styliseDialog(dialog);
    }
    virtual ~Dialog() {}
    void run() {
        dialog->setWindowTitle(title());
        addItems();
        dialog->setModal(false);
        dialog->show();
    }
protected:
    virtual QString title() = 0;
    virtual void addItems() = 0;
    QDialog* dialog;
};

class SelectInfo : public Dialog {
protected:
    void addItems() override {
        auto* txtArea = new QTextEdit(dialog);
        txtArea->setReadOnly(true);

        auto btnPush = [txtArea](const QString help) {
            txtArea->setText(help);
        };

        auto objs = getObjs();
        FlowLayout *layout = new FlowLayout;
        for (const auto &title : objs.keys()) {
            auto* newB = new SvgBtn(":/assets/btn2.svg", dialog);
            newB->setText(title);
            QString txt = title + "\n\n" + objs[title];
            newB->setToolTip(txt);
            newB->setWordWrap(false);
            QObject::connect(newB, &SvgBtn::clicked, dialog,
                [txt, btnPush]() mutable{ btnPush(txt); });
            layout->addWidget(newB);
        }

        auto* lay = new QVBoxLayout(dialog);
        lay->addLayout(layout, 3);
        lay->addWidget(txtArea, 1);
        dialog->setLayout(lay);
    }
    virtual QMap<QString, QString> getObjs() = 0;
};
class TextInfo : public Dialog {
protected:
    void addItems() override {
        auto* txtArea = new QTextEdit(dialog);
        txtArea->setReadOnly(true);
        txtArea->setText(getTxt());

        auto* lay = new QVBoxLayout(dialog);
        lay->addWidget(txtArea);
        dialog->setLayout(lay);
    }
    virtual QString getTxt() = 0;
};
