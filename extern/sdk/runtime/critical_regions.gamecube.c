#include "critical_regions.h"

// MSL's locks around its shared state (critical_regions.h). In this build they do nothing.

void __kill_critical_regions(void) {
}

void __begin_critical_region(int region) {
}

void __end_critical_region(int region) {
}
