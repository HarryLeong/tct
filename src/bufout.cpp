//This is a part of tct, tct is a tool for counting text file.
//Copyright (C) 2013  Harry Leong(https://github.com/HarryLeong/tct)

//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
