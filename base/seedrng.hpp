#pragma once
#include <QRandomGenerator>
#include <QDateTime>

inline quint32 seed;
inline void newSeed() {
    seed = QDateTime::currentMSecsSinceEpoch();
}

inline QRandomGenerator getRNG(quint32 seed_) {
    return { seed + seed_*0xAC };
}
inline QRandomGenerator getRNG(QString seed_) { return getRNG(qHash(seed_)); }
