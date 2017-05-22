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
#include "command.h"
#include "tct.h"
#include "file.h"

#include "utils.h"

#include <assert.h>
#include <ctime>
#include <vector>
#include <utility>
#include <string>
#include <cstring>
#include <utility>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

namespace tct {

	int count_file(File const &fname, char *buf, int buflen, Command const &cmd)
	{
		int err = 0;
		int nln = 0;
		std::pair<int&, int&>(err, nln) = count_file_nlines(fname, buf, buflen);
		if (err != 0) {
			printf("error: %s", fname.c_str());
		}
		if (cmd.show_file) {
			if (cmd.show_file_nlines) {
				printf("%s: %d\n", fname.c_str(), nln);
			}
			else {
				printf("%s\n", fname.c_str());
			}
		}
		return nln;
	}

	struct ThreadWork
	{
		ThreadWork(Files files, Files *exts, Command const &cmd)
			: command(cmd)
		{
			nln = 0;
			fFiles = std::move(files);
		}

		std::thread start() {
			return std::thread([this]() {
				int buflen = 128 *1024;
				std::unique_ptr<char[]> buf(new char[buflen]);
				for(File &fname : fFiles) {
					this->nln += count_file(fname, buf.get() , buflen, command);
				}
			});
		}

		int nln;
		Files fFiles;
		Command const &command;
	};
	static_assert(!std::is_move_assignable_v<ThreadWork>, "");
	static_assert(!std::is_copy_assignable_v<ThreadWork>, "");
	static_assert(std::is_move_constructible_v<ThreadWork>, "");

	static_assert(!std::is_copy_assignable_v<std::thread>, "");
	static_assert(!std::is_copy_constructible_v<std::thread>, "");

	int count_files(Files &files,Files &exts, int *pntread, Command const &cmd)
	{
		auto &ntread = *pntread;
		if(ntread == 0) {
			ntread = 1;
		}
		std::vector<std::thread> threads;
		std::vector<ThreadWork> threadworks;

		auto iter = files.begin();
		int ntfiles = (int)files.size() / ntread;
		for(int i = 0;i < ntread; ++i) {
			if(i == ntread - 1) {
				Files files(iter, files.end());
				threadworks.push_back(ThreadWork(std::move(files), &exts, cmd));
			} else {
				Files files(iter, iter + ntfiles);
				threadworks.push_back(ThreadWork(std::move(files), &exts, cmd));
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
				if(starts_with(tmp->first, iter->first)) {
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
				if(starts_with(tmp->first, dir.first)) {
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

		using namespace std;
		int nlines = 0;
		int nfiles = 0;

		auto start_time = std::chrono::steady_clock::now();
		try {

			Files fs;

			push_files_args_t args;
			args.pextensions = &cmd.extensions;
			args.onNonExsit = [](path const &ph) {
				printf("Error: Non-exsit file/direcotry: %s", ph.string().c_str());
			};
			args.onNotDirectory = [](path const &ph) {
				printf("Error: Not Directory: %s", ph.string().c_str());
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

			nlines += count_files(fs, cmd.extensions, &cmd.nthreads, cmd);

			nfiles = (int)fs.size();
		} catch(std::exception const &e) {
			printf("internal error: %s!\n", e.what());
			err = 1;
		}

		if (cmd.show_nfiles) {
			printf("files: %d\n", nfiles);
		}
		if (cmd.show_nlines) {
			printf("lines: %d\n", nlines);
		}
		if (cmd.show_time) {
			auto time = std::chrono::steady_clock::now() - start_time;
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(time);
			printf("time: %d ms\n", (int)ms.count());
		}
		if (cmd.show_nthreads) {
			printf("threads: %d\n", (int)cmd.nthreads);
		}
		return err;

	}

}

const char *licence = "Copyright (C) 2016  Harry Leong (https://github.com/HarryLeong/tct)\n"
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
		printf("%s\n Use 'tct -h' for help.\n", err.string().c_str());
		return 1;
	}
	if(command.show_help || argc == 1) {
		printf("%s\n", command.help.c_str());
	}
	if(command.show_licence) {
		printf("%s\n", licence);
	}
	return work(command);

}
