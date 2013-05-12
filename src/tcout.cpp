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


#include "tcout.h"
#include "bufout.h"

#include <utility>
#include <mutex>

namespace tct {

	static std::mutex mutex;

	tcout_t tcout()
	{
		return tcout_t();
	}

	tcout_t::tcout_t(tcout_t &&r) : sbuf(std::move(r.sbuf))
	{
	}
	tcout_t::tcout_t()
	{
	}

	tcout_t::~tcout_t()
	{
		if(!sbuf.empty()) {
			std::lock_guard<std::mutex> guard(mutex);
			bufout.append(sbuf.data(), sbuf.length());
		}
	}

	void tcout_t::flush()
	{
		std::lock_guard<std::mutex> guard(mutex);
		bufout.flush();
	}

	
	tcout_t &ln(tcout_t & stream)
	{
		stream << "\n";
		return stream;
	}	

}
