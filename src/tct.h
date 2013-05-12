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

#ifndef ___tct_tct
#define ___tct_tct

#include <string>
#include <utility>
#include <vector>

namespace tct {

	typedef std::string File;
	typedef std::vector<File> Files;
	struct Directory
	{
	private:
		Directory(Directory const &r);
	public:
		
		template<class String>
		Directory(String &&str,bool recur) : name(std::forward<String>(str)) {
			this->recur = recur;
		}

		Directory dup() {
			return Directory(name, recur);
		}

		Directory(Directory &&r) :
			name(move(r.name))
		{ recur = r.recur; }

		bool recur;
		File name;
		bool operator<(Directory const &r)
		{
			if(recur < r.recur) return true;
			if(recur == r.recur) return name < r.name;
			return false;
		}
		bool operator==(Directory const &r)
		{
			return recur == r.recur
				&& name == r.name;
		}
	};
	typedef std::vector<Directory> Directories;

}

#endif
