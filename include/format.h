#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);  // TODO: See src/format.cpp
std::string LeadingZero(int value);
std::string KbToMb(std::string value);
};  // namespace Format

#endif