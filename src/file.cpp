#include "file.h"

#ifdef _MSC_VER
# define fopen fopen_

FILE *fopen_(char const *name,char const *mode)
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

	int count_file_nlines(std::string const &name, int *perr, char *buf, int buflen)
	{
		assert(perr);
		assert(buf);
		assert(buflen > 8);
		int &err = *perr;
		err = 0;

		int nln = 0;

		FILE *file = fopen(name.c_str(),"r");
		if(file == nullptr) {
			err = 1;
			return 0;
		}
		AutoClose ac(file);

		bool last_is_newline = true;

		for(;;) {
			int const nread = fread(buf, 1, buflen, file);
			if(nread != buflen) {
				int ferr = ferror(file);
				if(ferr != 0) {
					err = 1;
					return 0;
				}
			}
			for(int i = 0;i < nread; ++i) {
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
		return nln;
	}

	void push_files(Files *pfiles, path const &ph, bool recur, push_files_t const &args, bool check)
	{
		using namespace boost::filesystem;
		using namespace std;
		Files &files = *pfiles;
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
			path const &ph = (*iter).path();
			if(is_directory(ph)) {
				if(recur) {
					push_files(&files, ph, true, args, false);
					continue;
				}
			} else if(is_regular(ph)) {
				if(check_extensions(ph.string(), args.extensions())) {
					files.push_back(ph.string());
				}
			}
		}
	}

	void push_files(Files *pfiles, Directories const &phs, push_files_t const &args, bool check)
	{
		for(Directory const &dir: phs) {
			push_files(pfiles, path(dir.name), dir.recur, args, check);
		}
	}


}
