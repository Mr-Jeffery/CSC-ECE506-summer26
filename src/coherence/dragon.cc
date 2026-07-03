/// @file dragon.cc
/// @brief Implementation of the Dragon coherence protocol

#include "dragon.h"

ADD_COHERENCE_TO_CMD_LINE(Dragon);

void Dragon::PrRd(cache_line* line) {
}

void Dragon::PrWr(cache_line* line) {
}

bool Dragon::BusRd(cache_line* line) {
}

bool Dragon::BusUpdt(cache_line* line) {
}

bool Dragon::doesDirtySharing() {
}

bool Dragon::isWriteBackNeeded(state_e state) {
}
