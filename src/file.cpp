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



#include "file.h"
#include <assert.h>
#include <experimental/filesystem>
using std::experimental::filesystem::absolute;
using std::experimental::filesystem::directory_iterator;
using std::experimental::filesystem::is_regular_file;

#ifdef sa
# define fopen fopen_

FILE *fopen_(char const *name, char const *mode)
{
	FILE *file = nullptr;
	auto err = fopen_s(&file, name, mode);
	if(err !=0 || file == nullptr) {
		return nullptr;
	}
	return file;
}

#endif

namespace tct {

	bool check_extensions(std::string const &name, std::vector<std::string> const &exts)
	{
		if(exts.empty()) {
			return true;
		}
		auto pos = name.rfind('.');
		if(pos == name.npos) {
			return contain(exts, std::string());
		} else {
			return contain(exts, name.substr(pos, std::string::npos));
		}
	}

	bool check_extensions(path const &file, std::vector<std::string> const &exts)
	{
		return check_extensions(file.string(), exts);
	}

	std::pair<int,int> count_file_nlines(std::string const &name, char *buf, int buflen)
	{
		assert(buf);
		assert(buflen > 8);
		int err = 0;
		int nln = 0;
		std::pair<int&, int &> result(err, nln);

		FILE *file = fopen(name.c_str(), "r");
		if(file == nullptr) {
			err = 1;
			return result;
		}
		AutoClose ac(file);

		bool last_is_newline = true;

		for(;;) {
			int const nread = fread(buf, 1, buflen, file);
			if(nread != buflen) {
				int ferr = ferror(file);
				if(ferr != 0) {
					err = 1;
					return result;
				}
			}
			for(int i = 0; i < nread; ++i) {
				if(buf[i] == '\n') {
					++nln;
				}
			}
			if(nread > 0) {
				last_is_newline = buf[nread - 1] == '\n';
			}

			if(nread != buflen) {
				break;
			}
		}
		if(!last_is_newline) {
			++nln;
		}
		return result;
	}

	void push_files(Files *pfiles, path const &ph, bool recur, push_files_args_t const &args, bool check)
	{
		using namespace std;
		if(check) {
			if(!exists(ph)) {
				args.onNonExsit(ph);
				return;
			} else if(!is_directory(ph)) {
				args.onNotDirectory(ph);
				return;
			}
		}

		directory_iterator  iter(ph);
		directory_iterator  end;

		for(iter; iter != end; ++iter) {
			path const &ph = iter->path();
			if(is_directory(ph)) {
				if(recur) {
					push_files(pfiles, ph, true, args, false);
					continue;
				}
			} else if(is_regular_file(ph)) {
				if(check_extensions(ph.string(), args.extensions())) {
					pfiles->push_back(ph.string());
				}
			}
		}
	}

	void push_files(Files *pfiles, Directories const &phs, push_files_args_t const &args, bool check)
	{
		for(Directory const &dir: phs) {
			push_files(pfiles, path(dir.name), dir.recur, args, check);
		}
	}


}
