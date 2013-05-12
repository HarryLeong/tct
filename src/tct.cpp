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
#include "tcout.h"
#include "utils.h"
#include "command.h"
#include "tct.h"
#include "file.h"

#define BOOST_LIB_DIAGNOSTIC
#include "boost/algorithm/string.hpp"

#include <ctime>
#include <vector>
#include <utility>
#include <string>
#include <cstring>
#include <utility>
#include <iostream>
#include <algorithm>
#include <thread>


namespace tct {

	int count_file(File const &fname,Files const &exts, bool check_ext, char *buf, int buflen, Command const &cmd)
	{
		if(check_ext) {
			if(!check_extensions(fname, exts)) {
				return 0;
			}
		}
		int err = 0;
		int nln = count_file_nlines(fname, &err, buf, buflen);
		if(err == 0) {
			if(cmd.show_file) {
				if(cmd.show_file_nlines) {
					tcout() << fname << ": " << nln << ln;
				} else {
					tcout() << fname << ln;
				}
			}
			return nln;
		}
		else{
			tcout() << "Error(file): " << fname << ln;
			return 0;
		}
	}

	struct ThreadWork
	{
		ThreadWork(Files::iterator begin,Files::iterator end, Files *exts, bool check_ext,Command const &cmd)
			: command(cmd)
		{
			nln = 0;
			this->begin = begin;
			this->end = end;
			this->exts = exts;
			this->check_ext = check_ext;
		}

		std::thread start() {
			return std::thread([this]() {
				int buflen = 128 *1024;
				char *buf = new char[buflen];
				for(File &fname : range(this->begin, this->end)) {
					this->nln += count_file(fname, *exts, check_ext, buf , buflen, command);
				}
				delete buf;
			});
		}

		int nln;
		Files::iterator begin;
		Files::iterator end;
		Files *exts;
		bool check_ext;
		Command const &command;
	};

	int count_files(Files &files,Files &exts, bool check_ext, int ntread, Command const &cmd)
	{
		if(ntread == 0) {
			ntread = (int)std::thread::hardware_concurrency();
			if(ntread == 0) {
				ntread = 1;
			}
		}
		std::vector<std::thread> threads;
		std::vector<ThreadWork> threadworks;

		auto iter = files.begin();
		int ntfiles = (int)files.size() / ntread;
		for(int i = 0;i < ntread; ++i) {
			if(i == ntread - 1) {
				threadworks.push_back(ThreadWork(iter,files.end(), &exts, check_ext, cmd));
			} else {
				threadworks.push_back(ThreadWork(iter,iter + ntfiles, &exts, check_ext, cmd));
			}
			iter += ntfiles;
			assert(iter <= files.end());
		}
		threads.reserve(threadworks.size());
		for(auto &work : threadworks) {
			threads.push_back(work.start());
		}
		for(auto &thread : threads) {
			thread.join();
		}
		int nln = 0;
		for(auto &work : threadworks) {
			nln += work.nln;
		}
		return nln;
	}

	int trim(Directories *pdirs)
	{
		using namespace boost::filesystem;
		using namespace boost::algorithm;
		Directories &dirs = *pdirs;

		typedef std::pair<std::string, int> DI;
		typedef std::pair<std::string, int> FI;
		std::list<DI> rdirs;
		std::list<DI> udirs;
		std::vector<int> edirs;

		int index = 0;
		for(auto &dir : dirs) {
			if(dir.recur) {
				rdirs.emplace_back(dir.name, index);
			} else {
				udirs.emplace_back(dir.name, index);
			}
			++index;
		}
		path ph;
		for(auto &dir :rdirs) {
			dir.first = uniform(absolute(path(dir.first)).string());
		}
		for(auto &dir :udirs) {
			dir.first = uniform(absolute(path(dir.first)).string());
		}
		
		rdirs.sort([](DI const &l, DI const &r) {
			return l.first > r.first;
		});

		for(auto iter = rdirs.begin();iter != rdirs.end(); ++iter) {
			auto iter2 = iter;
			++iter2;
			for(; iter2 != rdirs.end(); ) {
				auto tmp = iter2;
				++iter2;
				if(boost::algorithm::starts_with(tmp->first, iter->first)) {
					rdirs.erase(tmp);
					edirs.push_back(tmp->second);
					break;
				}
			}
		}

		for(auto &dir : rdirs) {
			for(auto iter = udirs.begin(); iter != udirs.end();) {
				auto tmp = iter;
				++iter;
				if(boost::algorithm::starts_with(tmp->first, dir.first)) {
					rdirs.erase(tmp);
					edirs.push_back(tmp->second);
					break;
				}
			}
		}

		std::sort(edirs.begin(), edirs.end());
		std::reverse(edirs.begin(), edirs.end());

		for(int i: edirs) {
			dirs.erase(dirs.begin() +i);
		}
		return 0;
	}

	int trim(std::vector<std::string> *exts)
	{
		std::vector<std::string> copy;
		for(auto ext : *exts) {
			if(!contain(copy, ext)) {
				copy.push_back(ext);
			}
		}
		(*exts) = std::move(copy);
		return 0;
	}

	int work(Command &cmd)
	{
		int err = 0;

		err = trim(&cmd.extensions);
		if(err != 0) {
			return err;
		}

		err = trim(&cmd.directories);
		if(err != 0) {
			return err; 
		}

		using namespace boost;
		using namespace std;
		int nlines = 0;
		int nfiles = 0;
		clock_t start_time = clock();
		try {

			Files fs;

			push_files_t args;
			args.pextensions = &cmd.extensions;
			args.onNonExsit = [](path const &ph) {
				tcout() << "Error: Non-exsit file/direcotry: " << ph.string() << ln;
			};
			args.onNotDirectory = [](path const &ph) {
				tcout() << "Error: Not Directory: " << ph.string() << ln;
			};
			push_files(&fs, cmd.directories, args);

			Files absfs;
			Files subfiles;
			absfs.reserve(fs.size());
			for(File &file: fs) {
				absfs.push_back(uniform(absolute(path(file)).string()));
			}
			for(File &subfile : cmd.files) {
				std::string tmp = uniform(absolute(path(subfile)).string());
				auto iter = std::find(absfs.begin(), absfs.end(), tmp);
				if(iter == absfs.end()) {
					subfiles.push_back(subfile);
				}
			}

			fs.insert(fs.end(), subfiles.begin(), subfiles.end());

			nlines += count_files(fs, cmd.extensions, false, cmd.nthreads, cmd);

			nfiles = (int)fs.size();
		} catch(...) {
			tcout() << "ERROR(interanl): Internal error and exit!" << ln;
			err = 1;
		}
		if(cmd.show_nfiles) {
			tcout() << "count files: " << nfiles << ln;
		}
		if(cmd.show_nlines) {
			tcout() << "count lines: " << nlines << ln;
		}
		auto time = clock() - start_time;
		if(cmd.show_time) {
			tcout() << "time: " << time << ln;
		}
		tcout().flush();
		return err;

	}

}

const char *licence = "Copyright (C) 2013  Harry Leong(https://github.com/HarryLeong/tct)\n"
	"\n"
	"This program is free software: you can redistribute it and/or modify"
	"it under the terms of the GNU General Public License as published by"
	"the Free Software Foundation, either version 3 of the License, or"
	"(at your option) any later version.\n"
	"\n"
	"This program is distributed in the hope that it will be useful,"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
	"GNU General Public License for more details.\n"
	"\n"
	"You should have received a copy of the GNU General Public License"
	"along with this program.  If not, see <http://www.gnu.org/licenses/>.\n"
	;

int main(int argc, char *argv[])
{
	tct::Command command;
	auto err = command.parse(argc, argv);
	if(err.code() != 0) {
		tct::tcout() << err.string() << tct::ln;
		tct::tcout() << "use 'tct -h' for help." << tct::ln;
		return 1;
	}
	if(command.show_help || argc == 1) {
		tct::tcout() << command.help << tct::ln;
	}
	if(command.show_licence) {
		tct::tcout() << licence << tct::ln;
	}
	return work(command);

}
