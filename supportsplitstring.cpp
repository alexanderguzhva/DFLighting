#include "supportsplitstring.h"

#include <sstream>

namespace dflighting
{

void SupportSplitString::Split(const std::string & s, char delim, std::vector<std::string> & elems)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim))
        elems.push_back(item);
}



}
