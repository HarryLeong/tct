//This is a part of tct, tct is a tool for counting text file.
//Copyright (C) 2016  Harry Leong(https://github.com/HarryLeong/tct)

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


#include "utils.h"

#include <exception>
#include <stdexcept>
#include <cstdio>
#include <cwchar>

namespace tct {

	bool isAlpha(char c)
	{
		return isascii(c) && isalpha(c);
	}
	bool starts_with(std::string const &a, std::string const &b) {
		auto p = a.c_str();
		auto q = b.c_str();
		for (; *p == *q && *q != '\0'; ++p, ++q) {
		}
		return *q == '\0';
	}

	AutoClose::~AutoClose()
	{
		if(fFile != nullptr) {
			if(fclose(fFile) != 0) {
				printf("error: failed to close file!");
			}
		}
	}

}
