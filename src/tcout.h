#ifndef ___tct_tcout
#define ___tct_tcout

#include <string>
#include <cstdio>

#if _MSC_VER
#define snprintf _snprintf_s
#endif

namespace tct {

	struct tcout_t
	{
	private:
		friend tcout_t tcout();
		tcout_t();
	public:
		tcout_t(tcout_t &&r);
		~tcout_t();

		tcout_t &operator<<(int i)
		{
			char buf[20];
			int length = snprintf(buf, 20,"%d", i);
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

		void flush();

		std::string sbuf;
	};

	tcout_t tcout();
	tcout_t &ln(tcout_t &);

}

#endif
