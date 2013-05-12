#include "bufout.h"

#include <iostream>
#include <cstdio>
#include <string>

namespace tct{

	bufout_t::bufout_t()
	{
		length = 0;
	}

	void bufout_t::append(char const *str, int length)
	{
#if 0
		if(length + this->length >= max_length) {
			flush();
			if(length >= max_length) {
				std::cout.write(str, length);
				return;
			}
		}
		memcpy(buffer + this->length, str, length);
		this->length += length;
#else
		//std::cout.write(str, length);
		std::cout << std::string(str, length);
#if !defined(NDEBUG)
		std::cout.flush();
#endif

#endif
	}


	void bufout_t::flush()
	{
		//std::cout.write(buffer, length);
		std::cout << std::string(buffer, length);
		length = 0;
	}

	bufout_t bufout;

}
