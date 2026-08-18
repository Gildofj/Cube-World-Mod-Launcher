#pragma once
#include "../../CWSDK/cwsdk.h"

int GoldBagValue(cube::Item* item)
{
	int retval = 0;
	return retval;
}

extern "C" int cube__Item__OnGetGoldBagValue(cube::Item * item)
{
    if (!item || item->category != 31)
	{
		return 0;
	}

	int value = 100;

	for (uint8_t priority = 0; priority <= 4; priority += 1) {
		for (DLL* dll : modDLLs) {
			if (dll && dll->mod && dll->mod->OnItemGetGoldBagValuePriority == (GenericMod::Priority)priority) {
				dll->mod->OnItemGetGoldBagValue(item, &value);
			}
		}
	}

	return value;
}

overwrite_function(0x109070, cube__Item__OnGetGoldBagValue)