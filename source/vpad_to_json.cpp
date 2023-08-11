#include "vpad_to_json.h"
#include <map>
#include "rapidjson/writer.h"

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
 * Get the calibrated stick value.
 * @param pos The position.
 * @param min The minimum value.
 * @param max The maximum value.
 * @param center The center value.
 * @return The calibrated stick value.
 */
[[nodiscard]] static constexpr float getStickValue(float pos, float min, float max, float center)
{
    if(pos == center)
    {
        return 0.0f;
    }
    else if(pos > center)
    {
        return (pos - center) / (max - center + 1.0f);
    }
    else
    {
        return (pos - min) / (center - min + 1.0f) - 1.0f;
    }
}

/**
 * Convert GamePad data to JSON string used by UsendMii.
 * @param[in] pad_data Controllers data.
 * @return The JSON string.
 */
std::string pad_to_json(const PADData& pad_data)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    writer.SetMaxDecimalPlaces(10);

    writer.StartObject(); // Start root object

    // Wii Remotes
    if(pad_data.wpad[WPAD_CHAN_0] != nullptr ||
       pad_data.wpad[WPAD_CHAN_1] != nullptr ||
       pad_data.wpad[WPAD_CHAN_2] != nullptr ||
       pad_data.wpad[WPAD_CHAN_3] != nullptr)
    {
        writer.Key("wiiRemotes");
        writer.StartArray();
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

            writer.StartObject(); // Start wiiremote object
            writer.Key("order");
            writer.Uint(i + 1);
            writer.Key("hold");
            writer.Uint(holdwii);
            writer.Key("posX");
            writer.Int(pad_data.wpad[i]->ir.x);
            writer.Key("posY");
            writer.Int(pad_data.wpad[i]->ir.y);
            //writer.Key("angleX");
            //writer.Double(pad_data.wpad[i]->angle.x);
            //writer.Key("angleY");
            //writer.Double(pad_data.wpad[i]->angle.y);
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

                        auto js = pad_data.wpad[i]->exp.nunchuk.js;
                        auto x = getStickValue(js.pos.x, js.min.x, js.max.x, js.center.x);
                        auto y = getStickValue(js.pos.y, js.min.y, js.max.y, js.center.y);

                        writer.Key("extension");
                        writer.StartObject(); // Start extension object
                        writer.Key("type");
                        writer.String("nunchuk");
                        writer.Key("hold");
                        writer.Uint(holdnunchuk);
                        writer.Key("stickX");
                        writer.Double(x);
                        writer.Key("stickY");
                        writer.Double(y);
                        writer.EndObject(); // Start extension object
                    }
                    break;
                case EXP_CLASSIC:
                    { // Classic Controller
                        u32 holdclassic = pad_data.wpad[i]->btns_h >> 16;

                        auto ljs = pad_data.wpad[i]->exp.classic.ljs;
                        auto lx = getStickValue(ljs.pos.x, ljs.min.x, ljs.max.x, ljs.center.x);
                        auto ly = getStickValue(ljs.pos.y, ljs.min.y, ljs.max.y, ljs.center.y);

                        auto rjs = pad_data.wpad[i]->exp.classic.rjs;
                        auto rx = getStickValue(rjs.pos.x, rjs.min.x, rjs.max.x, rjs.center.x);
                        auto ry = getStickValue(rjs.pos.y, rjs.min.y, rjs.max.y, rjs.center.y);

                        writer.Key("extension");
                        writer.StartObject(); // Start extension object
                        writer.Key("type");
                        writer.String("classic");
                        writer.Key("hold");
                        writer.Uint(holdclassic);
                        writer.Key("lStickX");
                        writer.Double(lx);
                        writer.Key("lStickY");
                        writer.Double(ly);
                        writer.Key("rStickX");
                        writer.Double(rx);
                        writer.Key("rStickY");
                        writer.Double(ry);
                        writer.Key("lTrigger");
                        writer.Double(pad_data.wpad[i]->exp.classic.l_shoulder);
                        writer.Key("rTrigger");
                        writer.Double(pad_data.wpad[i]->exp.classic.r_shoulder);
                        writer.EndObject(); // Start extension object
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
            writer.EndObject(); // End wiiremote object
        }
        writer.EndArray();
    }

    // GameCube Controllers
    if(pad_data.pad[PAD_CHAN0] != nullptr ||
       pad_data.pad[PAD_CHAN1] != nullptr ||
       pad_data.pad[PAD_CHAN2] != nullptr ||
       pad_data.pad[PAD_CHAN3] != nullptr)
    {
        writer.Key("gameCubeControllers");
        writer.StartArray();
        for(int i = 0; i < 4; ++i)
        {
            if(pad_data.pad[i] == nullptr)
            {
                continue;
            }

            writer.StartObject(); // Start gameCubeController object
            writer.Key("order");
            writer.Uint(i + 1);
            writer.Key("hold");
            writer.Uint(pad_data.pad[i]->button);
            writer.Key("ctrlStickX");
            writer.Int(pad_data.pad[i]->stickX);
            writer.Key("ctrlStickY");
            writer.Int(pad_data.pad[i]->stickY);
            writer.Key("cStickX");
            writer.Int(pad_data.pad[i]->substickX);
            writer.Key("cStickY");
            writer.Int(pad_data.pad[i]->substickY);
            writer.Key("lTrigger");
            writer.Int(pad_data.pad[i]->triggerL);
            writer.Key("rTrigger");
            writer.Int(pad_data.pad[i]->triggerR);
            writer.EndObject(); // End gameCubeController object
        }
        writer.EndArray();
    }

    writer.EndObject(); // End root object

    // Convert to string
    return sb.GetString();
}
