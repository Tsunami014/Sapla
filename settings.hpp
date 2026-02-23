#pragma once
#include <QString>

void initSettings();
void initDeckSettings();

QString getStrSetting(QString key, bool global = true, QString deflt = "");
float getNumSetting(QString key, bool global = true, float deflt = 0.0);

void setStrSetting(QString key, QString value, bool global = true);
void setNumSetting(QString key, float value, bool global = true);
