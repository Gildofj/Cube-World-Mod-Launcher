#pragma once

#define overwrite_function(offset, function)\
void setup_##function() {\
	WriteFarJMP(Offset(base, offset), (void*)&function);\
}

#define setup_function(function) setup_##function()
