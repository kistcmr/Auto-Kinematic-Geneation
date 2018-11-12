/**

 @file ustring.hpp
 @brief 문자열 처리 함수
 @author 나성권
            
*/


#ifndef __ROMAS_USTRING_HPP__
#define __ROMAS_USTRING_HPP__


#include <sstream>
#include <string>
#include <vector>
//#include <cstdarg>
//#include <cstdio>
#include <cmath>

namespace romas {

	/// 문자열을 토큰으로 분리한다.
	/// @param line 문자열
	/// @param delim 문자열을 분리하는 기준이 되는 문자
	/// @param tokens 문자열이 분리된 결과
	inline void tokenize(const std::string& line, const std::string delim, std::vector<std::string>& tokens) {

		static size_t start;
		static size_t end;
		static size_t stop;

		tokens.clear();
		start = line.find_first_not_of(delim);
		end = line.length();

		while ((start >= 0) && (start < end)) {
			stop = line.find_first_of(delim, start);
			if ((stop < 0) || (stop > end)) stop = end;
			tokens.push_back(line.substr(start, stop - start));
			start = line.find_first_not_of(delim, stop + 1);
		}
	}

	/// 형변환. Source에 현재의 형식을 지정하고, Target에 변환될 형식을 지정한다.
	/// @param src 형변환을 하고자 하는 변수
	/// @return 형변환된 변수
	template<typename Target, typename Source> Target
		lexical_cast(const Source& src) {

		std::stringstream ss;
		Target result;
		ss << src;
		ss >> result;

		return result;
	}

	inline void toQuaternion(double roll, double pitch, double yaw, double& w, double& x, double& y, double& z)
	{
		double t0 = std::cos(yaw * 0.5);
		double t1 = std::sin(yaw * 0.5);
		double t2 = std::cos(roll * 0.5);
		double t3 = std::sin(roll * 0.5);
		double t4 = std::cos(pitch * 0.5);
		double t5 = std::sin(pitch * 0.5);

		w = t0 * t2 * t4 + t1 * t3 * t5;
		x = t0 * t3 * t4 - t1 * t2 * t5;
		y = t0 * t2 * t5 + t1 * t3 * t4;
		z = t1 * t2 * t4 - t0 * t3 * t5;
		return;
	}

}


#endif 
