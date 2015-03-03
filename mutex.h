#pragma once

#include "stdint.h"

uint8_t lock_mutex(uint32_t mutex);

void unlock_mutex(uint32_t mutex);
