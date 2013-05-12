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


#ifndef ___tct_bufout
#define ___tct_bufout

namespace tct {
	//actully it be slowly,...God!!
	struct bufout_t
	{
		bufout_t();
		static const int max_length = 64;
		void append(char const *str, int length);
		void flush();
		int length;
		char buffer[max_length + 1];
	};
	extern bufout_t bufout;
}

#endif
