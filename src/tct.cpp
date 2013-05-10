#define BOOST_LIB_DIAGNOSTIC
#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"
#include "gflags/gflags.h"

#include <ctime>
#include <vector>
#include <utility>
#include <string>
#include <cstring>
#include <cstdio>
#include <utility>
#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>

// fread is more effictive than the ReadFile API on Windows!!!
#if defined(_MSC_VER) && 0 

#include <Windows.h>
void *fopen_(char const *name, char const*)
{
	 HANDLE h = (HANDLE)CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	 if(h == INVALID_HANDLE_VALUE) {
		 return nullptr;
	 }
	 return h;
}
int fclose_(void *file)
{
	return CloseHandle(file);
}

int fread_(void *buf,int,int length,void *file)
{
	DWORD read = 0;
	ReadFile(file, buf, length, &read ,NULL);
	return (int)read;
}

int ferror_(void *) {
	return GetLastError();
}

#define fopen fopen_
#define fclose fclose_
#define fread fread_
#define ferror ferror_
#define FILE void

#endif

//actully it be slowly,...God!!
struct bufout_t
{
	static const int max_length = 64;
	bufout_t()
	{
		length = 0;
	}

	void append(char const *str, int length)
	{
#if 0
		if(length + this->length >= max_length) {
			flush();
			if(length >= max_length) {
				std::cout.write(str, length);
				return;
			}
		}
		memcpy(buffer + this->length, str, length);
		this->length += length;
#else
		//std::cout.write(str, length);
		std::cout << std::string(str, length);
#if !defined(NDEBUG)
		std::cout.flush();
#endif

#endif
	}


	void flush()
	{
		//std::cout.write(buffer, length);
		std::cout << std::string(buffer, length);
		length = 0;
	}
	int length;
	char buffer[max_length + 1];
} bufout;


struct tcout_t
{
	tcout_t()
	{
	}
	tcout_t(tcout_t &&r) : sbuf(move(r.sbuf))
	{
	}
	~tcout_t()
	{
		if(!sbuf.empty()) {
			std::lock_guard<std::mutex> guard(mutex);
			bufout.append(sbuf.data(), sbuf.length());
		}
	}

	tcout_t &operator<<(int i)
	{
		char buf[20];
		int length = sprintf(buf, "%d", i);
		sbuf.append(buf, length);
		return *this;
	}
	tcout_t &operator<<(std::string const &str)
	{
		sbuf.append(str.data(), str.length());
		return *this;
	}
	tcout_t &operator<<(char const *str)
	{
		sbuf.append(str, strlen(str));
		return *this;
	}
	tcout_t &operator<<(char ch)
	{
		sbuf.append(&ch, 1);
		return *this;
	}

	tcout_t &operator<<(tcout_t &(*op)(tcout_t &) )
	{
		return (*op)(*this);
	}

	void flush()
	{
		std::lock_guard<std::mutex> guard(mutex);
		bufout.flush();
	}

	std::string sbuf;
	static std::mutex mutex;
};

std::mutex tcout_t::mutex;

tcout_t tcout()
{
	return tcout_t();
}

inline tcout_t &ln(tcout_t & _Ostr)
{	// insert newline and flush byte stream
	_Ostr << "\n";
	return (_Ostr);
}


template< class I>
struct Range {
	Range(I b, I e) { _begin = b, _end = e; }
	I begin() { return _begin; }
	I end() { return _end; }
	I _begin;
	I _end;
};

template< class I>
Range<I> range(I begin, I end)
{
	return Range<I>(begin, end);
}


bool isAlpha(char c)
{
	return isascii(c) && isalpha(c);
}

struct auto_close {
	FILE *file;
	~auto_close() {
		if(file != nullptr) {
			fclose(file);
		}
	}
};

int compute_file_nlines(std::string const &filename,int *perr, char *buf, int buflen)
{
	assert(perr);
	assert(buf);
	assert(buflen > 8);
	int &err = *perr;
	err = 0;

	int nln = 0;

	FILE *file = fopen(filename.c_str(),"r");
	if(file == nullptr) {
		err = 1;
		return 0;
	}
	auto_close ac;
	ac.file = file;
	for(;;) {
		int const nread = fread(buf, 1, (buflen - 1), file);
		if(nread != (buflen - 1)) {
			int ferr = ferror(file);
			if(ferr != 0) {
				err = 1;
				return 0;
			}
		}
		buf[nread] = '\0';
		for(char *chpos = buf; ; ++nln) {
			chpos = strchr(chpos,'\n');
			if(chpos == nullptr) {
				break;
			}
			++chpos;
		}

		if(nread != (buflen - 1)) {
			break;
		}
	}
	return nln;
}

typedef std::string File;
typedef std::vector<File> Files;
typedef std::vector<int> Filenlines;
typedef std::pair<std::string,bool> Directory;
typedef std::vector<Directory > Directories;

namespace fs {
	using namespace boost::filesystem;
	typedef path Path;
}


struct Command {
	Command();
	bool show_file;
	bool show_file_nlines;
	
	bool show_nlines;
	bool show_nfiles;
	bool show_time;

	Files files;
	Files extensions;
	Directories directories;

	int nfiles;
	int nlines;
	int time;
	int num_thread;

	int work();
	int lint();
};

extern Command command;


bool check_extension(std::string const &filename,Files const &exts)
{
	if(exts.empty()) {
		return true;
	}
	size_t pos = filename.rfind('.');
	if(pos == filename.npos) {
		std::string str;
		return std::find(exts.begin(), exts.end(), str) != exts.end();
	} else {
		std::string str = filename.substr(pos, filename.size());
		return std::find(exts.begin(), exts.end(), str) != exts.end();
	}
}

bool check_extension(fs::path const &path,Files const &exts) {
	return check_extension(path.string(), exts);
}

int count_file(File const &fname,Files const &exts, bool check_ext, char *buf, int buflen)
{
	if(check_ext) {
		if(!check_extension(fname, exts)) {
			return 0;
		}
	}
	int err = 0;
	int nln = compute_file_nlines(fname, &err, buf, buflen);
	if(err == 0) {
		if(command.show_file) {
			if(command.show_file_nlines) {
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
#if 0
int count_files(Files &files,Files &exts, bool check_ext)
{
	int nln = 0;
	int buflen = 128 *1024;
	char *buf = new char[buflen];
	for(File &fname : files) {
		nln += count_file(fname, exts, check_ext, buf , buflen);
	}
	delete buf;
	return nln;
}
#else
struct ThreadWork
{
	ThreadWork(Files::iterator begin,Files::iterator end, Files *exts, bool check_ext)
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
				this->nln += count_file(fname, *exts, check_ext, buf , buflen);
			}
			delete buf;
		});
	}

	int nln;
	Files::iterator begin;
	Files::iterator end;
	Files *exts;
	bool check_ext;
};

int count_files(Files &files,Files &exts, bool check_ext, int ntread)
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
			threadworks.push_back(ThreadWork(iter,files.end(), &exts, check_ext));
		} else {
			threadworks.push_back(ThreadWork(iter,iter + ntfiles, &exts, check_ext));
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

#endif

void push_files(fs::path &dir, bool recur, bool check_dir, Files const &exts, Files *files)
{
	using namespace boost;
	using namespace std;
	int nfiles = 0;
	int nlines = 0;
	if(check_dir) {
		if(!fs::exists(dir)) {
			tcout() << "not exist file/directory: " << dir.string() << ln;
			return;
		} else if(!fs::is_directory(dir)) {
			tcout() << "not directory: " << dir.string() << ln;
			return;
		}
	}
	fs::directory_iterator  enditer;
	fs::directory_iterator  iter(dir);

	for(iter; iter != enditer; ++iter) {
		fs::Path path = (*iter).path();
		if(fs::is_directory(path)) {
			if(recur) {
				push_files(path, recur, false, exts, files);
				continue;
			}
		} else if(fs::is_regular(path)) {
			if(check_extension(path, exts)) {
				files->push_back(path.string());
			}
		}
	}
}

void push_files(Directories &dirs,Files &exts, Files *files)
{
	for(Directory &dir: dirs) {
		fs::Path path(dir.first);
		push_files(path, dir.second, true, exts, files);
	}
}

struct DirectoryIndex
{
	Directory direcotry;
	int index;
};

int Command::lint()
{
	using namespace boost::filesystem;
	using namespace boost::algorithm;
	using namespace std;
	vector<DirectoryIndex> copy;
	int aindex = 0;
	for(auto &dir : directories) {
		DirectoryIndex dir_;
		dir_.direcotry = dir;
		dir_.index = aindex;
		copy.push_back(move(dir_));
		++aindex;
	}
	for(auto &dir : copy) {
		path ph = dir.direcotry.first;
		ph = absolute(ph);
		dir.direcotry.first = ph.string();
	}
	sort(copy.begin(),copy.end(),[](DirectoryIndex const &l, DirectoryIndex const &r) {
		if((int)l.direcotry.second < (int)r.direcotry.second) {
			return true;
		} else if((int)l.direcotry.second == (int)r.direcotry.second) {
			return l.direcotry.first < r.direcotry.first;
		} else {
			return false;
		}
	});
	auto iter = find_if(copy.begin(), copy.end(), [](DirectoryIndex const &l) {
		return l.direcotry.second == true;
	});
	vector<int> indexs;
	int index = &(*iter) - copy.data();
	for(int i = 0; i < index; ++i) {
		for(int j = i + 1; j < (int)copy.size();) {
			if(starts_with(copy[j].direcotry.first, copy[i].direcotry.first)) {
				indexs.push_back(copy[j].index);
				copy.erase(copy.begin() + j);
				if(j < index) {
					--index;
				}
			} else {
				++j;
			}
		}
	}
	for(int i = index; i < (int)copy.size(); ++i) {
		for(int j = i + 1; j < (int)copy.size();) {
			if(copy[j].direcotry.first == copy[i].direcotry.first) {
				indexs.push_back(copy[j].index);
				copy.erase(copy.begin() + j);
			} else {
				++j;
			}
		}
	}
	sort(indexs.begin(),indexs.end(),[](int l, int r){
		return !(l < r);
	});
	for(int index : indexs) {
		tcout() << "warning:\'" << directories[index].first << "\' is repeated! we ignore this!" << ln;
		directories.erase(directories.begin() + index);
	}
	for(auto &file : files) {
		if(check_extension(file, extensions)) {
			tcout() << "warning:\'" << file << "\' is repeated! we ignore this!" << ln;
			directories.erase(directories.begin() + index);
		}
	}

	return 0;
}

int Command::work()
{
	int errlint = lint();
	if(errlint != 0) {
		return errlint; 
	}
	using namespace boost;
	using namespace std;
	int errc = 0;
	clock_t start_time = clock();
	try {
		nlines = 0;
		
		Files fs;
		push_files(directories, extensions, &fs);
		fs.reserve(fs.size() + files.size());
		fs.insert(fs.end(), files.begin(), files.end());
		nlines += count_files(fs, extensions, false, num_thread);
		
		nfiles = (int)fs.size();
	} catch(...) {
		tcout() << "ERROR(interanl): internal error and exit!" << ln;
		errc = 1;
	}
	tcout() << '\n';
	if(show_nfiles) {
		tcout() << "files count: " << nfiles << ln;
	}
	if(show_nlines) {
		tcout() << "lines count: " << nlines << ln;
	}
	time = clock() - start_time;
	if(show_time) {
		tcout() << "time:" << time << ln;
	}
	tcout().flush();
	return errc;
}


// Command.cpp

Command::Command()
{
}

Command command;

DEFINE_bool(show_time, false, "show the time");
DEFINE_bool(show_nfiles, true, "show the number of files");
DEFINE_bool(show_nlines, true, "show the number of lines of all files");
DEFINE_bool(show_file, false, "show each file name");
DEFINE_bool(show_file_nlines, true, "show the the number of nlines of each file(if show the file name)");

DEFINE_string(extension,"<non>", "filter by the file extension name"
			  "use 'extension=\"\"' to specifiy the empty file"
			  " extension name(exclude files specified by file name directly).");
DEFINE_string(directory,"<non>", "search files in the directory");
DEFINE_string(directory_recur,"<non>", "search files in the directory and its subdirectories");

DEFINE_string(e,"<non>", "short for extension");
DEFINE_string(d,"<non>", "short for directory");
DEFINE_string(dr,"<non>", "short for directory_recur");

DEFINE_int32(nt,0,"short for num_thread");
DEFINE_int32(num_thread,0,"the number of threads we run.if 0 is setted, we will determine a value by the\
						  machine.(although counting files is an io work, but experiment shows there is speed-up\
						  in mutilple threads)");

std::string usg = "the program count the number of files, lines and etc.\n"
	"examples of usage:\n"
	"srcct -extension=\"\" -e=cpp -d=\"../d1\" -dr=\"d2\" f1.h f2.cpp\n"
	"this will count files with no extension or 'cpp' extension in direcotry '../d1' or directories\n"
	"'d2'and its subdirectories,finally,count the files,'f1.h' and 'f2.cpp'\n";


bool check_e(const char *flag, std::string const &val)
{
		if(val == "<non>") {
			return true;
		}
		size_t i = 0;
		if(val[0] == '.') {
			++i;
		}
		for(;i < val.size() && isAlpha(val[i]); ++i) {
		}
		if(i != val.size()) {
			return false;
		}
		if(val.size() == 0 || (val[0] == '.' && val.size() == 1)) {
			command.extensions.push_back("");
			return true;
		}
		std::string const *str = &val;
		std::string tmp;
		if(val[0] != '.') {
			tmp = ".";
			tmp += val;
			str = &tmp;
		}
		if(str == &val) {
			tmp = val;
		}
		command.extensions.push_back((std::string&&)tmp);
		return true;
}

bool check_d(const char *flag, std::string const &val)
{
		if(val == "<non>") {
			return true;
		}
		command.directories.push_back(Directory(val,false));
		return true;
}


bool check_dr(const char *flag, std::string const &val)
{
		if(val == "<non>") {
			return true;
		}
		command.directories.push_back(Directory(val,true));
		return true;
}

bool check_nt(const char *flag, google::int32 val)
{
	if(val < 0 || val > 128) {
		tcout() << "number of threads should be in range [0,128]." << ln;
		return false;
	}
	FLAGS_num_thread = val;
	FLAGS_nt = val;
	return true;
}


struct Init {
	Init()
	{
		google::RegisterFlagValidator(&FLAGS_extension, check_e);
		google::RegisterFlagValidator(&FLAGS_e, check_e);
		google::RegisterFlagValidator(&FLAGS_directory,check_d);
		google::RegisterFlagValidator(&FLAGS_d, check_d);
		google::RegisterFlagValidator(&FLAGS_directory_recur,check_dr);
		google::RegisterFlagValidator(&FLAGS_dr, check_dr);
		google::RegisterFlagValidator(&FLAGS_nt, check_nt);
		google::RegisterFlagValidator(&FLAGS_num_thread, check_nt);
	}
} init;
#if 1
int main(int argc,char *argv[])
{
	int nargc = argc + 3;
	char **nargv = new char*[nargc];
	std::unique_ptr<char*> uqu(nargv);
	std::copy(argv, argv + argc, nargv);
	nargv[argc + 0] = "-e=<non>";
	nargv[argc + 1] = "-d=<non>";
	nargv[argc + 2] = "-dr=<non>";
	google::SetUsageMessage(usg);
	google::SetVersionString("0.9");
	int first = google::ParseCommandLineFlags(&nargc, &nargv, true);
	assert(1 == first);
	command.show_time = FLAGS_show_time;
	command.show_nfiles = FLAGS_show_nfiles;
	command.show_nlines = FLAGS_show_nlines;
	command.show_file = FLAGS_show_file;
	command.show_file_nlines = FLAGS_show_file_nlines;
	command.num_thread = FLAGS_num_thread;
	for(int i = 1; i < nargc; ++i) {
		command.files.push_back(nargv[i]);
	}
	return command.work();
}
#endif

/*
int main()
{
	std::string buf;
	buf += "sdfsfsdfsfsfsdfsdfsfwefjeowvh powegsuohbsp ngg";
	clock_t time = clock();
	for(int i = 0; i < 1000; ++i) {
#if 1
		std::cout
#else
		bufout
#endif
			<< buf << std::endl;
	}
	std::cout << (clock() - time) << "\n";
	system("pause");
}
*/