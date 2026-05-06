#include "filter.hpp"
#include "features.hpp"

bool matchesFilter(Note* n, QString filter) {
    if (filter.isEmpty()) return true;
    QString conts = n->contents();
    conts = ScheduleFeat::instance->replacements(conts, SIDE_HIDE);
    return conts.contains(filter);
}
