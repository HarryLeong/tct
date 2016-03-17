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

#ifndef ___tct_command
#define ___tct_command

#include "tct.h"
#include <string>
#include "opts.h"

namespace tct {

	struct Command
	{
		Command();
		opts::Opts::Error parse(int argc, char **argv);

		bool show_file;
		bool show_file_nlines;
		bool show_nlines;
		bool show_nfiles;
		bool show_time;	
		bool show_help;
		bool show_licence;
		int nthreads;
		Files files;
		Files extensions;
		Directories directories;

		std::string help;

	};

}

#endif
