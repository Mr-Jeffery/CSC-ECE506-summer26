/// @file mesi.cc
/// @brief Implementation of the MESI coherence protocol

#include "mesi.h"

ADD_COHERENCE_TO_CMD_LINE(MESI);

void MESI::PrRd(cache_line* line) {
}

void MESI::PrWr(cache_line* line) {
}

bool MESI::BusRd(cache_line* line) {
}

bool MESI::BusRdX(cache_line* line) {
}

bool MESI::BusUpgr(cache_line* line) {
}

bool MESI::isWriteBackNeeded(state_e state) {
}
