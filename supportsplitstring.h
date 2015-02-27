#ifndef SUPPORTSPLITSTRING_H
#define SUPPORTSPLITSTRING_H

#include <vector>
#include <string>

namespace dflighting
{

class SupportSplitString
{
public:
    static void Split(const std::string & s, char delim, std::vector<std::string> & elems);
};


}

#endif // SUPPORTSPLITSTRING_H
