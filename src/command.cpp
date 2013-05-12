#include "command.h"
#include "opts.h"
#include "file.h"

namespace tct {
	
	opts::Opts::Error Command::parse(int argc, char **argv)
	{
		opts::Opts opts;

		opts.add("thread-count",
			"t",
			"The number of threads we run.",
			int(),
			[&](int const &val, std::string *err) {
				if(val < 0 || val > 128) {
					err->append("the number of thread should be in range[0, 128]");
					return false;
				}
				nthreads = val;
				return true;
		},"the number of threads we run.if 0 is setted, we will determine a value by the"
		  "machine.(although counting files is an io work, but experiment shows there is speed-up"
		  "in mutilple threads)"
		  );

		opts.add("directory",
			"d",
			"Search files in the directory",
			std::string(),
			[&](std::string const &val, std::string *) {
			directories.push_back(Directory(val, false));
			return true;
		});

		opts.add("extension",
			"e",
			"Files that was included by searching direcotry will be filterred by the extension name",
			std::string(),
			[&](std::string const &val, std::string *) {
				std::string ext = val;;
				if(!extension_uniform(&ext)) {
					return false;
				}
				extensions.push_back(ext);
				return true;
		},
			"Files that was included by searching direcotry will be filterred by the extension name."
			"If no extension name is ever setted, that means every file is O.K."
			"If at least one extension name is ever setted, that the file must have one of those(that) extension name(s)."
			"If '.' is setted, that means the file must have no extension name."
			);

		opts.add("directory-recur",
			"r",
			"Search files in the directory and its subdirectories",
			std::string(),
			[&](std::string const &val, std::string *) {
				directories.push_back(Directory(val, true));
				return true;
		});

		opts.add("show-time",
			"",
			"Show the time",
			bool(),
			[&](bool val, std::string *) {
				show_time = val;
				return true;
		});

		opts.add("show-file",
			"",
			"Show the information for each file",
			bool(),
			[&](bool val, std::string *) {
				show_file = val;
				return true;
		});

		opts.add("show-file-nlines",
			"",
			"The information of each file include number of lines",
			bool(),
			[&](bool val, std::string *) {
				show_file_nlines = val;
				return true;
		});

		opts.add("show-nfiles",
			"",
			"Show number of all files",
			bool(),
			[&](bool val, std::string *) {
				show_nfiles = val;
				return true;
		});
		opts.add("show-nlines",
			"",
			"Show number of lines of all files",
			bool(),
			[&](bool val, std::string *) {
				show_nlines = val;
				return true;
		});
		opts.add("help",
			"h",
			"Show help",
			bool(),
			[&](bool val, std::string *) {
				show_help = val;
				return true;
		});
		opts.toolname("tct");
		opts.description("This tool count this number of files or lines of all files.\n");
		help = opts.help();
		Files files;
		std::vector<char *> reset;
		auto err = opts.parse(argc, argv, &reset);
		if(err.code() != 0) {
			return std::move(err);
		}
		files.assign(reset.begin() + 1, reset.end());
		this->files = std::move(files);
		return std::move(err);

		}

		Command::Command()
		{
			show_file = false;
			show_file_nlines = true;
			show_nlines = true;
			show_nfiles = true;
			show_time = false;	
			nthreads = 0;
		}

}

