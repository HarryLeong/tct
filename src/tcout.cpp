#include "tcout.h"
#include "bufout.h"

#include <utility>
#include <mutex>

namespace tct {

	static std::mutex mutex;

	tcout_t tcout()
	{
		return tcout_t();
	}

	tcout_t::tcout_t(tcout_t &&r) : sbuf(std::move(r.sbuf))
	{
	}
	tcout_t::tcout_t()
	{
	}

	tcout_t::~tcout_t()
	{
		if(!sbuf.empty()) {
			std::lock_guard<std::mutex> guard(mutex);
			bufout.append(sbuf.data(), sbuf.length());
		}
	}

	void tcout_t::flush()
	{
		std::lock_guard<std::mutex> guard(mutex);
		bufout.flush();
	}

	
	tcout_t &ln(tcout_t & stream)
	{
		stream << "\n";
		return stream;
	}	

}
