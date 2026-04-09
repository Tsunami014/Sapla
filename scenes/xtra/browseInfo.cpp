#include "browseInfo.hpp"
#include "notes/features.hpp"
#include "browseInfoBase.hpp"


void showDialog(QString title, QMap<QString, QString> objs) {
    QDialog* dialog = new QDialog;
    dialog->setWindowTitle(title);
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

    MG->curScene->dialogOpen();
    QObject::connect(dialog, &QDialog::finished, [=](int result){
        MG->curScene->dialogClose();
    });
    dialog->show();
}


class FeatInfo : public SelectInfo {
    virtual QString title() { return "FeatInfo"; }
    virtual QMap<QString, QString> getObjs() {
        QMap<QString, QString> merged;
        for (const auto& f : Feats) {
            merged.insert(f->help());
        }
        return merged;
    }
};
class ArgInfo : public TextInfo {
    virtual QString title() { return "ArgInfo"; }
    virtual QString getTxt() {
        return
//"<h2>Argument specifier options</h2>"
//"<p>The argument name in the argument specifier can have some suffixes, for example "+code("[hi]")+"</p>"
"<h2>Argument usage</h2>"
// and suffixes
"<p>Arguments can be used with prefixes, for example "+cod("%.name")+".</p>"
"<p>They can be stacked and will be executed in the order you put them in, so e.g. "+cod("^.")+" would end out being lower case (first upper <i>then</i> lower case).</p>"
"<h3>Argument prefixes</h3>"
"<ul>"
    "<li>"+cod(".")+" - converts text to lower case</li>"
    "<li>"+cod("^")+" - converts text to upper case</li>"
"</ul>"
//"<h3>Argument suffixes</h3>"
;
    }
};


template<typename T>
void BrowseInfos::mkhelp(QString nam) {
    auto it = new MenuItem(nam, Menues->HelpMenu);
    helps.push_back(it);
    connect(it, &QAction::triggered, this, []() {
        Dialog* obj = new T();
        obj->run();
        delete obj;
    });
}
BrowseInfos::BrowseInfos() {
    mkhelp<FeatInfo>("Note feature help");
    mkhelp<ArgInfo>("Argument help");
}
BrowseInfos::~BrowseInfos() {
    for (auto& h : helps) delete h;
}
