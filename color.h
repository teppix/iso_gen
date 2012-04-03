#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>

#define COLOR(r,g,b) ((((r & 0xff) << 16 ) | ((g & 0xff) << 8)) | (b&0xff))
typedef int32_t Color;

#endif
