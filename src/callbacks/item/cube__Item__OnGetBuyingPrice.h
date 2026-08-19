#pragma once
#include <cmath>
#include <algorithm>
#include "cwsdk.h"

extern "C" void cube__Item__GetBuyingPrice(float base_cost, cube::Item* item, int* price)
{
    if (!item || !price) return;
	*price = std::max<int>(static_cast<int>(base_cost * std::pow(2.0f, (float)item->rarity)), 1);

	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnGetItemBuyingPricePriority == (GenericMod::Priority)priority) {
				dll->mod->OnGetItemBuyingPrice(item, price);
			}
		}
	}
}

extern "C" void ASM_cube__Item__GetBuyingPrice();

void setup_cube__Item__GetBuyingPrice() {
    WriteFarJMP(CWOffset(0x109E0E), (void*)&ASM_cube__Item__GetBuyingPrice);
}