#include "utils.h"
#include "tcout.h"

#include <exception>
#include <stdexcept>
#include <cstdio>
#include <cwchar>

namespace tct {

	bool isAlpha(char c)
	{
		return isascii(c) && isalpha(c);
	}

	AutoClose::~AutoClose()
	{
		if(file != nullptr) {
			int err = fclose(file);
			if(err != 0) {
				tcout() << "failed to close file!"
					<< ln;
			}
		}
	}

}
