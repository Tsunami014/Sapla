#pragma once
#include <QMenuBar>
#include <QMenu>

void initMenu(QMenuBar* bar);

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

struct _MenuesTyp {
    _MenuBase* FileMenu;
    _MenuBase* ViewMenu;
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

