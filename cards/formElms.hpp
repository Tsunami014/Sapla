#pragma once
#include <QVBoxLayout>

namespace Form {
    void labelField(QVBoxLayout* lay, const QString& txt);
    void textField(QVBoxLayout* lay, const QString& initTxt, std::function<void(const QString&)> onChange = nullptr);
}

