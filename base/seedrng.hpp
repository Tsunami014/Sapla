#pragma once
#include <QRandomGenerator>
#include <QDateTime>
#include <utility>

extern quint32 seed;
extern bool useseed;

void setSeed(quint32 sd);
quint32 getSeed();

QRandomGenerator getRNG();
QRandomGenerator getRNG(quint32 seed2);

template<typename... Args>
auto rng_bounded(Args&&... args) {
    if (useseed) return QRandomGenerator(seed).bounded(std::forward<Args>(args)...);
    return QRandomGenerator::global()->bounded(std::forward<Args>(args)...);
}

template<typename Seed, typename... Args>
auto rng_bounded_seed(Seed&& seed2, Args&&... args) {
    return getRNG(seed2).bounded(std::forward<Args>(args)...);
}
