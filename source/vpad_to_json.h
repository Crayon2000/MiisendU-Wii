#pragma once

#include <string>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>

/**
 * Struct to hold all controllers data.
 */
typedef struct {
    WPADData* wpad[4]; /**< Wii Remotes. */
    PADStatus* pad[PAD_CHANMAX]; /**< GameCube Controller. */
} PADData;

std::string pad_to_json(PADData pad_data);
