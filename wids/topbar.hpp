#pragma once
#include "svgBtn.hpp"
#include <QWidget>
#include <QHBoxLayout>

struct Shortcut {
    int key;
    int mods;
};
struct BtnFeatures {
    QString label;
    QString apply;
    std::optional<Shortcut> shortcut;
    QString context;
    QString help;
};

class Topbar : public QWidget {
    Q_OBJECT
public:
    Topbar(QWidget* parent = nullptr);
    void close();
    inline void resizeEvent(QResizeEvent* ev) override { resize(); QWidget::resizeEvent(ev); }

protected:
    void createItems();
    void onBtnPush(const QString& apply);

    QHBoxLayout* barLay;
    QGridLayout* gridLay;
    QWidget* grid;
    struct Btns {
        SvgBtn* btn;
        QString help;
    };
    Btns settings;
    std::vector<Btns> btns;
    std::unordered_map<int, std::vector<std::pair<int, QString>>> cuts; /** This is a mapping of keys to a list of (modifiers, apply) */

private slots:
    void toggleXpanded();

private:
    void resize();

    SvgBtn* xpandBtn;
    SvgBtn* settingsBtn;
    bool xpanded = false;
};

