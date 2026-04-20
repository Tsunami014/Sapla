#include "filter.hpp"

bool matchesFilter(Note* n, QString filter) {
    if (filter.isEmpty()) return true;
    QString conts = n->contents();
    return conts.contains(filter);
}
