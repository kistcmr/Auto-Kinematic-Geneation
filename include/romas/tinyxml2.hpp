#ifndef _ROMAS_TINYXML2_HPP_
#define _ROMAS_TINYXML2_HPP_

#include <string>
#include "tinyxml2/tinyxml2.h"

namespace romas {
	inline std::string convert(const char* c)
	{
		if (c)	
			return std::string(c);
		else 	
			return std::string();
	};
}

#endif
