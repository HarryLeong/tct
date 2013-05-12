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
