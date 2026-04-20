#include "seedrng.hpp"

quint32 seed;
bool useseed = false;

void setSeed(quint32 sd) {
    seed = sd;
    useseed = true;
}
quint32 getSeed() {
    return QDateTime::currentMSecsSinceEpoch();
}

QRandomGenerator getRNG() {
    quint32 outseed = useseed? seed : getSeed();
    return { outseed };
}
QRandomGenerator getRNG(quint32 seed2) {
    quint32 outseed = useseed? seed : getSeed();
    return { outseed * (seed2+1) };
}
