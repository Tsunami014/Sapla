#pragma once
#include <QMenuBar>
#include <QMenu>

void initMenu(QMenuBar* bar);

void makeDialog(const QString& HtmlTxt, const QString& title);
extern bool dialogging;

class _MenuBase : public QMenu {
public:
    using QMenu::QMenu;
    QAction* insertBefore;
};
class Menu : public _MenuBase {
public:
    Menu(const QString& title);
    ~Menu();
};
class MenuAction : public QAction {
public:
    MenuAction(const QString& title);
    ~MenuAction();
};

struct _MenuesTyp {
    _MenuBase* FileMenu;
    _MenuBase* GotoMenu;
    _MenuBase* HelpMenu;
};
extern _MenuesTyp* Menues;

class MenuItem : public QAction {
public:
    MenuItem(const QString& text, _MenuBase* parentMenu);
    ~MenuItem();
private:
    QMenu* parentMen;
};

