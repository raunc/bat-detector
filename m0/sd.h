#ifndef SD_H_
#define SD_H_

#include <SD.h>

void SDPrepare(File&, uint8_t chipSelect);
bool SDFlush(File &, uint32_t);
bool SDNewOpen(File&);
void SDReCheck(File&, uint8_t chipSelect, uint32_t, uint32_t);

#endif
