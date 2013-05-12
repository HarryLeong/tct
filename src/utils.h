#ifndef ___tct_utils
#define ___tct_utils

#include <stdio.h>

namespace tct {
	
	bool isAlpha(char c);
	
	struct AutoClose
	{
		~AutoClose();
		AutoClose(FILE *file)
		{ 
			this->file = nullptr;
			setFile(file);
		}
		AutoClose()
		{
			this->file = nullptr;
			setFile(file); 
		}
		void setFile(FILE *file)
		{ this->file = file; }
	private:
		FILE *file;
	};

	template< class I>
	struct Range {
		Range(I b, I e)
		{ _begin = b, _end = e; }
		
		I begin()
		{ return _begin; }
		
		I end()
		{ return _end; }
	private:
		I _begin;
		I _end;
	};

	template< class I>
	Range<I> range(I begin, I end)
	{
		return Range<I>(begin, end);
	}

	template<class R, class T >
	bool contain(R const &range, T const &val)
	{
		return std::find(begin(range) ,end(range), val) != end(range);
	}

}

#endif
