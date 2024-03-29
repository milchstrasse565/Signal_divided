#ifndef __BSP_AHT20_H
#define __BSP_AHT20_H

#include "main.h"


void temphum_init(void);
void AHT20_Read_CTdata(uint32_t *ct);

float aht20_GetTemp(uint32_t *ct);

float aht20_GetHum(uint32_t *ct);


#endif			//__BSP_AHT20_H


