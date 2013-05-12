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

#ifndef ___tct_filesystem
#define ___tct_filesystem

#include "boost/filesystem.hpp"
#include "tct.h"
#include "utils.h"

#include <string>
#include <vector>
#include <functional>

namespace tct {

	using boost::filesystem::path;
	using boost::filesystem::absolute;	
	using boost::filesystem::is_regular;

	template< class String >
	std::string uniform(String &&path)
	{
		std::string tmp = std::forward<String>(path);
		for(char &ch : tmp) {
			if(ch == '\\' || ch == '/') {
				ch = (char)path::preferred_separator;
			}
		}
		return std::move(tmp);
	}

	inline bool extension_uniform(std::string *pext)
	{
		auto &ext = *pext;
		auto pos = ext.find('.');
		if(pos == ext.npos) {
			ext.insert(ext.begin(),'.');
			return true;
		} else if(pos == 0) {
			return true;
		}
		return false;
	}

	bool check_extensions(std::string const &name, std::vector<std::string> const &exts);
	bool check_extensions(path const &ph, std::vector<std::string> const &exts);

	int count_file_nlines(std::string const &filename,int *perr, char *buf, int buflen);

	struct push_files_t
	{
		push_files_t()
		{ pextensions = nullptr; }
		std::vector<std::string> const &extensions() const
		{ 
			assert(pextensions != nullptr);
			return *pextensions;
		}
		std::vector<std::string> *pextensions;
		std::function<void(path const &ph)> onNonExsit;
		std::function<void(path const &ph)> onNotDirectory;
	};

	void push_files(Files *files, path const &ph, bool recur, push_files_t const &args, bool check = true);
	void push_files(Files *files, Directories const &phs, push_files_t const &args, bool check = true);

}

#endif
