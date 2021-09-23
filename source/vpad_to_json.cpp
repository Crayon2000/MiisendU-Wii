#include <map>
#include <string.h>
#include <jansson.h>
#include "vpad_to_json.h"

/**
 * Mask for the Wii Remote.
 */
static const std::map wiimask = {
    std::pair{WPAD_BUTTON_LEFT, 0x0001},
    {WPAD_BUTTON_RIGHT, 0x0002},
    {WPAD_BUTTON_DOWN, 0x0004},
    {WPAD_BUTTON_UP, 0x0008},
    {WPAD_BUTTON_PLUS, 0x0010},
    {WPAD_BUTTON_2, 0x0100},
    {WPAD_BUTTON_1, 0x0200},
    {WPAD_BUTTON_B, 0x0400},
    {WPAD_BUTTON_A, 0x0800},
    {WPAD_BUTTON_MINUS, 0x1000},
    {WPAD_BUTTON_HOME, 0x8000},
};

/**
 * Mask for the Nunchuk.
 */
static const std::map nunchukmask = {
    std::pair{WPAD_NUNCHUK_BUTTON_Z, 0x2000},
    {WPAD_NUNCHUK_BUTTON_C, 0x4000}
};

/**
 * Mask for the Classic Controller.
 */
static const std::map classicmask = {
    std::pair{WPAD_NUNCHUK_BUTTON_Z, 0x2000},
    {WPAD_NUNCHUK_BUTTON_C, 0x4000}
};

/**
 * Convert GamePad data to JSON string used by UsendMii.
 * @param[in] pad_data Controllers data.
 * @param[out] out Buffer where to copy the formatted data.
 * @param[in] out_size Size of the out buffer.
 */
void pad_to_json(PADData pad_data, char* out, uint32_t out_size)
{
    json_t *root = json_object();

    // Wii Remotes
    if(pad_data.wpad[WPAD_CHAN_0] != nullptr ||
       pad_data.wpad[WPAD_CHAN_1] != nullptr ||
       pad_data.wpad[WPAD_CHAN_2] != nullptr ||
       pad_data.wpad[WPAD_CHAN_3] != nullptr)
    {
        json_t *wiiremotes = json_array();
        json_object_set_new_nocheck(root, "wiiRemotes", wiiremotes);
        for(int i = 0; i < 4; ++i)
        {
            if(pad_data.wpad[i] == nullptr)
            {
                continue;
            }

            u32 holdwii = 0;
            u32 badwii = pad_data.wpad[i]->btns_h;
            for (auto const& [oldid, newid] : wiimask)
            {
                if(badwii & oldid) {
                    holdwii |= newid;
                }
            }

            json_t *wiiremote = json_object();
            json_object_set_new_nocheck(wiiremote, "order", json_integer(i + 1));
            json_object_set_new_nocheck(wiiremote, "hold", json_integer(holdwii));
            json_object_set_new_nocheck(wiiremote, "posX", json_integer(pad_data.wpad[i]->ir.x));
            json_object_set_new_nocheck(wiiremote, "posY", json_integer(pad_data.wpad[i]->ir.y));
            //json_object_set_new_nocheck(wiiremote, "angleX", json_real(pad_data.wpad[i]->angle.x));
            //json_object_set_new_nocheck(wiiremote, "angleY", json_real(pad_data.wpad[i]->angle.y));
            switch(pad_data.wpad[i]->exp.type)
            {
                case EXP_NUNCHUK:
                    { // Nunchuk
                        u32 holdnunchuk = 0;
                        u32 badnunchuk = pad_data.wpad[i]->btns_h;
                        for (auto const& [oldid, newid] : nunchukmask)
                        {
                            if(badnunchuk & oldid) {
                                holdnunchuk |= newid;
                            }
                        }

                        json_t *extension = json_object();
                        json_object_set_new_nocheck(wiiremote, "extension", extension);
                        json_object_set_new_nocheck(extension, "type", json_string("nunchuk"));
                        json_object_set_new_nocheck(extension, "hold", json_integer(holdnunchuk));
                        json_object_set_new_nocheck(extension, "stickX", json_real(pad_data.wpad[i]->exp.nunchuk.js.pos.x));
                        json_object_set_new_nocheck(extension, "stickY", json_real(pad_data.wpad[i]->exp.nunchuk.js.pos.y));
                    }
                    break;
                case EXP_CLASSIC:
                    { // Classic Controller
                        u32 holdclassic = 0;
                        u32 badclassic = pad_data.wpad[i]->btns_h;
                        holdclassic = badclassic; // fix me

/*
#define WPAD_CLASSIC_BUTTON_UP					(0x0001u<<16)
#define WPAD_CLASSIC_BUTTON_LEFT				(0x0002u<<16)
#define WPAD_CLASSIC_BUTTON_ZR					(0x0004u<<16)
#define WPAD_CLASSIC_BUTTON_X					(0x0008u<<16)
#define WPAD_CLASSIC_BUTTON_A					(0x0010u<<16)
#define WPAD_CLASSIC_BUTTON_Y					(0x0020u<<16)
#define WPAD_CLASSIC_BUTTON_B					(0x0040u<<16)
#define WPAD_CLASSIC_BUTTON_ZL					(0x0080u<<16)
#define WPAD_CLASSIC_BUTTON_FULL_R				(0x0200u<<16)
#define WPAD_CLASSIC_BUTTON_PLUS				(0x0400u<<16)
#define WPAD_CLASSIC_BUTTON_HOME				(0x0800u<<16)
#define WPAD_CLASSIC_BUTTON_MINUS				(0x1000u<<16)
#define WPAD_CLASSIC_BUTTON_FULL_L				(0x2000u<<16)
#define WPAD_CLASSIC_BUTTON_DOWN				(0x4000u<<16)
#define WPAD_CLASSIC_BUTTON_RIGHT				(0x8000u<<16)

        * 0x00000001 Up button
        * 0x00000002 Left button
        * 0x00000004 ZR trigger
        * 0x00000008 x button
        * 0x00000010 a button
        * 0x00000020 y button
        * 0x00000040 b button
        * 0x00000080 ZL trigger
        * 0x00000200 R trigger
        * 0x00000400 + button
        * 0x00000800 HOME button
        * 0x00001000 - button
        * 0x00002000 L trigger
        * 0x00004000 Down button
        * 0x00008000 Right button
        * 0x00010000 Left stick emulated left
        * 0x00020000 Left stick emulated right
        * 0x00040000 Left stick emulated down
        * 0x00080000 Left stick emulated up
        * 0x00100000 Right stick emulated left
        * 0x00200000 Right stick emulated right
        * 0x00400000 Right stick emulated down
        * 0x00800000 Right stick emulated up
*/

                        json_t *extension = json_object();
                        json_object_set_new_nocheck(wiiremote, "extension", extension);
                        json_object_set_new_nocheck(extension, "type", json_string("classic"));
                        json_object_set_new_nocheck(extension, "hold", json_integer(holdclassic));
                        json_object_set_new_nocheck(extension, "lStickX", json_real(pad_data.wpad[i]->exp.classic.ljs.pos.x));
                        json_object_set_new_nocheck(extension, "lStickY", json_real(pad_data.wpad[i]->exp.classic.ljs.pos.y));
                        json_object_set_new_nocheck(extension, "rStickX", json_real(pad_data.wpad[i]->exp.classic.rjs.pos.x));
                        json_object_set_new_nocheck(extension, "rStickY", json_real(pad_data.wpad[i]->exp.classic.rjs.pos.y));
                        json_object_set_new_nocheck(extension, "lTrigger", json_real(pad_data.wpad[i]->exp.classic.l_shoulder));
                        json_object_set_new_nocheck(extension, "rTrigger", json_real(pad_data.wpad[i]->exp.classic.r_shoulder));
                    }
                    break;
                case EXP_GUITAR_HERO_3:
                    [[fallthrough]];
                case EXP_WII_BOARD:
                    [[fallthrough]];
                case EXP_MOTION_PLUS:
                    [[fallthrough]];
                default:
                    break;
            }
            json_array_append(wiiremotes, wiiremote);
        }
    }

    // GameCube Controllers
    if(pad_data.pad[PAD_CHAN0] != nullptr ||
       pad_data.pad[PAD_CHAN1] != nullptr ||
       pad_data.pad[PAD_CHAN2] != nullptr ||
       pad_data.pad[PAD_CHAN3] != nullptr)
    {
        json_t *gccontrollers = json_array();
        json_object_set_new_nocheck(root, "gameCubeControllers", gccontrollers);
        for(int i = 0; i < 4; ++i)
        {
            if(pad_data.pad[i] == nullptr)
            {
                continue;
            }

            json_t *gccontroller = json_object();
            json_object_set_new_nocheck(gccontroller, "order", json_integer(i + 1));
            json_object_set_new_nocheck(gccontroller, "hold", json_integer(pad_data.pad[i]->button));
            json_array_append(gccontrollers, gccontroller);
        }
    }

    // Convert to string
    char* s = json_dumps(root, JSON_COMPACT | JSON_REAL_PRECISION(10));
    strncpy(out, s, out_size);
    free(s);

    json_decref(root);
}
