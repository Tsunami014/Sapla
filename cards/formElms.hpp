#pragma once
#include <QVBoxLayout>
#include "cardTyps.hpp"

namespace Form {
    void labelField(QVBoxLayout* lay, const QString& txt);
    void textField(QVBoxLayout* lay, const QString& initTxt, std::function<void(const QString&)> onChange = nullptr);
    void textXtraField(QVBoxLayout* lay, const SideXtra& init, std::function<void(int, const QString&)> onChange = nullptr);
}

