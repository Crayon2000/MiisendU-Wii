#ifndef _VPADTOJSON_H_
#define _VPADTOJSON_H_

#include <wiiuse/wpad.h>
#include <ogc/pad.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Struct to hold all controllers data.
 */
typedef struct {
    WPADData* wpad[4]; /**< Wii Remotes. */
    PADStatus* pad[PAD_CHANMAX]; /**< GameCube Controller. */
} PADData;

void pad_to_json(PADData pad_data, char* out, uint32_t out_size);

#ifdef __cplusplus
}
#endif

#endif /* _VPADTOJSON_H_ */
