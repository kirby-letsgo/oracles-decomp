#ifndef BUS_H
#define BUS_H
#include "gb.h"
uint8_t bus_read(GB *gb, uint16_t addr);
void bus_write(GB *gb, uint16_t addr, uint8_t v);
void bus_hdma_chunk(GB *gb);
#endif
