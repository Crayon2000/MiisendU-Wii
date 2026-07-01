#pragma once

#include <array>
#include <string>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>

/**
 * Structure to hold all controllers data.
 */
struct PADData {
    /**
     * Wii Remotes.
     */
    std::array<WPADData*, 4> wpad{};
    /**
     * GameCube Controller.
     */
    std::array<PADStatus*, PAD_CHANMAX> pad{};
};

std::string pad_to_json(const PADData& pad_data);
