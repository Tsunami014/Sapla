#include "../core.hpp"
#include "../notes/features.hpp"
#include "../base/flowlayout.hpp"
#include <QDialog>
#include <QTextEdit>

QMap<QString, QString> allFeatHelps() {
    QMap<QString, QString> merged;
    for (const auto& f : Feats) {
        merged.insert(f->help());
    }
    return merged;
}

void showFeatInfo() {
    QDialog* dialog = new QDialog;
    dialog->setWindowTitle("Top bar settings");
    dialog->resize(500, 700);
    dialog->setAttribute(Qt::WA_DeleteOnClose); // Auto-delete when closed
    dialog->setWindowModality(Qt::ApplicationModal); 
    MG->styliseDialog(dialog);

    auto* txtArea = new QTextEdit(dialog);
    txtArea->setReadOnly(true);

    auto btnPush = [txtArea](const QString help) {
        txtArea->setText(help);
    };

    FlowLayout *layout = new FlowLayout;
    auto feats = allFeatHelps();
    for (const auto &title : feats.keys()) {
        auto* newB = new SvgBtn(":/assets/btn2.svg", dialog);
        newB->setText(title);
        QString txt = title + "\n\n" + feats[title];
        newB->setToolTip(txt);
        newB->setWordWrap(false);
        QObject::connect(newB, &SvgBtn::clicked, dialog,
            [txt, btnPush]() mutable{ btnPush(txt); });
        layout->addWidget(newB);
    }

    auto* lay = new QVBoxLayout(dialog);
    lay->addLayout(layout, 3);
    lay->addWidget(txtArea, 1);

    MG->curScene->dialogOpen();
    QObject::connect(dialog, &QDialog::finished, [=](int result){
        MG->curScene->dialogClose();
    });
    dialog->show();
}
