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
		int nthreads;
		Files files;
		Files extensions;
		Directories directories;

		std::string help;

	};

}

#endif
