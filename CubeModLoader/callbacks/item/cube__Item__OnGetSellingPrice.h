#pragma once
#include <cmath>
#include <algorithm>
#include "../../CWSDK/cwsdk.h"

extern "C" int cube__Item__OnGetSellingPrice(cube::Item* item)
{
    if (!item) return 0;
	int buyingPrice = item->GetBuyingPrice();
	int sellingPrice = (int)std::max<float>(0.5f * std::sqrt((float)buyingPrice), 1.0f);

	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnGetItemSellingPricePriority == (GenericMod::Priority)priority) {
				dll->mod->OnGetItemSellingPrice(item, &sellingPrice);
			}
		}
	}

	return sellingPrice;
}

overwrite_function(0x10A390, cube__Item__OnGetSellingPrice)