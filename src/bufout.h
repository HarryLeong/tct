#ifndef ___tct_bufout
#define ___tct_bufout

namespace tct {
	//actully it be slowly,...God!!
	struct bufout_t
	{
		bufout_t();
		static const int max_length = 64;
		void append(char const *str, int length);
		void flush();
		int length;
		char buffer[max_length + 1];
	};
	extern bufout_t bufout;
}

#endif
