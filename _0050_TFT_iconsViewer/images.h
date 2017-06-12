#include <stdint.h>

typedef struct 
{
  uint8_t imageType;
  uint32_t address;
  uint16_t width;
  uint16_t height;  
} ImageDirectoryEntry_t;

enum imageType_t { RGB, COLOR565 };

extern const uint8_t images[];
extern ImageDirectoryEntry_t ImageDirectory[];
