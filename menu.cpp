#include "menu.hpp"
#include "main.hpp"
#include "log.hpp"
#include "help.hpp"
#include "scenes/homeScn.hpp"

QMenuBar* bar = nullptr;
QAction* last = nullptr;
_MenuesTyp* Menues = new _MenuesTyp;

void textWindow(const QString& txt, const QString& title) {
    QWidget* w = new QWidget();
    w->setWindowTitle(title);
    QTextEdit* txtEd = new QTextEdit;
    txtEd->setReadOnly(true);
    txtEd->setHtml(txt);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(txtEd);
    w->setLayout(layout);

    w->resize(500, 700);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

void initMenu(QMenuBar* b) {
    bar = b;
    b->setStyleSheet(
        "QMenuBar {"
            "background: rgb(90, 40, 10);"
        "}"
        "QMenuBar::item {"
            "padding: 4px;"
            "background: rgb(70, 35, 10);"
            "border-radius: 6px;"
        "}"
        "QMenuBar::item:selected {"
            "background: rgb(110, 60, 30);"
        "}"
        "QMenu {"
            "background: rgba(90,40,10,220);"
            "border-radius: 6px;"
        "}"
        "QMenu::item {"
            "border-radius: 4px;"
            "padding: 4px;"
        "}"
        "QMenu::item:selected {"
            "background: rgba(255,255,255,30);"
        "}"
    );

    auto* fmenu = new _MenuBase("File");
    fmenu->insertBefore = fmenu->addSeparator();
    bar->addMenu(fmenu);
    Menues->FileMenu = fmenu;

    auto* vmenu = new _MenuBase("View");
    vmenu->addAction("Go home", []() {
        MG->changeScene(new HomeScene());
    });
    vmenu->insertBefore = vmenu->addSeparator();
    bar->addMenu(vmenu);
    Menues->ViewMenu = vmenu;

    auto* hmenu = new _MenuBase("Help");
    hmenu->addAction("Application help", []() { textWindow(APP_HELP, "Application help"); });
    hmenu->addAction("This screen help", []() { textWindow(*helpStr, "This screen help"); });
    hmenu->addSeparator();
    hmenu->addAction("Logs", []() { showLogWindow(); });
    hmenu->insertBefore = hmenu->addSeparator();
    Menues->HelpMenu = fmenu;
    bar->addMenu(hmenu);
    last = hmenu->menuAction();
}

Menu::Menu(const QString& title) : _MenuBase(title, bar) {}
Menu::~Menu() {
    bar->removeAction(this->menuAction());
}

MenuItem::MenuItem(const QString& txt, _MenuBase* pmen)
    : QAction(txt, pmen), parentMen(pmen) {
        parentMen->insertAction(pmen->insertBefore, this);
    }
MenuItem::~MenuItem() {
    parentMen->removeAction(this);
}

