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


#ifndef ___tct_tcout
#define ___tct_tcout

#include <string>
#include <cstdio>

#if _MSC_VER
#define snprintf _snprintf_s
#endif

namespace tct {

	struct tcout_t
	{
	private:
		friend tcout_t tcout();
		tcout_t();
	public:
		tcout_t(tcout_t &&r);
		~tcout_t();

		tcout_t &operator<<(int i)
		{
			char buf[20];
			int length = snprintf(buf, 20,"%d", i);
			sbuf.append(buf, length);
			return *this;
		}

		tcout_t &operator<<(std::string const &str)
		{
			sbuf.append(str.data(), str.length());
			return *this;
		}

		tcout_t &operator<<(char const *str)
		{
			sbuf.append(str, strlen(str));
			return *this;
		}

		tcout_t &operator<<(char ch)
		{
			sbuf.append(&ch, 1);
			return *this;
		}

		tcout_t &operator<<(tcout_t &(*op)(tcout_t &) )
		{
			return (*op)(*this);
		}

		void flush();

		std::string sbuf;
	};

	tcout_t tcout();
	tcout_t &ln(tcout_t &);

}

#endif
