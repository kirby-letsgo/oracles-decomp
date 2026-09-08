#pragma once
#include "core/gb.h"

void clearMemory(GB *gb);
void fillMemory(GB *gb);
void clearMemoryBc(GB *gb);
void fillMemoryBc(GB *gb);
void copyMemoryReverse(GB *gb);
void copyMemory(GB *gb);
void copyMemoryBcReverse(GB *gb);
void copyMemoryBc(GB *gb);
void addAToDe(GB *gb);
void addAToBc(GB *gb);
void addDoubleIndexToDe(GB *gb);
void addDoubleIndexToBc(GB *gb);
void getNumSetBits(GB *gb);
void addDecimalToHlRef(GB *gb);
void subDecimalFromHlRef(GB *gb);
void multiplyAByC(GB *gb);
void multiplyABy16(GB *gb);
void multiplyABy8(GB *gb);
void multiplyABy4(GB *gb);
void s8ToS16(GB *gb);
void compareHlToBc(GB *gb);
void getHighestSetBit(GB *gb);
void getLowestSetBit(GB *gb);
void checkFlag(GB *gb);
void setFlag(GB *gb);
void unsetFlag(GB *gb);
void decHlRef16WithCap(GB *gb);
void incHlRefWithCap(GB *gb);
void incHlRef16WithCap(GB *gb);
void hexToDec(GB *gb);
void getRandomNumber(GB *gb);
void getRandomNumber_noPreserveVars(GB *gb);
void getRandomIndexFromProbabilityDistribution(GB *gb);
