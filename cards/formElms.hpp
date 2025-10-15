#pragma once
#include <QVBoxLayout>
#include "cardTyps.hpp"

namespace Form {
    void spacing(QBoxLayout* lay, int amount);
    void labelField(QVBoxLayout* lay, const QString& txt);
    void textField(QVBoxLayout* lay, QString* initTxt, std::function<void(const QString&)> onChange = nullptr, bool update = true);
    void textXtraField(QVBoxLayout* lay, SideXtra* init, std::function<void(int, const QString&)> onChange = nullptr, bool update = true);
}

