/* this file contains function definitions for parsing a tar file.
 *
 * by Marcel Nowicki (github@epenis.de | github.com/blindi0815)
 */

#include "tarfunc.h"
#include "tarconst.h"
#include <string>
#include <cmath>

// checks if a valid modern tar file - ustar
bool tar::validTar(std::istream &file) {
    file.seekg(tarconstant::mgcfieldByte.first);
    char* buffer = new char [tarconstant::mgcfieldByte.second];
    file.read(buffer, tarconstant::mgcfieldByte.second);
    std::string magicfield(&buffer[0], 5);
    delete[] buffer;
    file.seekg(0);
    return magicfield == tarconstant::mgcfield ? true: false;
}

// checks if a 512byte block consist only of 0 or \0
bool tar::eof (const char* buf) {
    std::string test(&buf[0]);
    std::erase(test, '\0');
    std::erase(test, '0');
    if (test.empty())
        return true;
    return false;
}

// gets size of an item in bytes. assumes octal encoding.
uint64_t tar::getitemsize(const char* buf){
    std::string asciisize(&buf[tarconstant::itemoctlnByte.first], tarconstant::itemoctlnByte.second);
    uint64_t itemsize{};
    uint8_t power = tarconstant::itemoctlnByte.second - 1;
    for (auto i : asciisize) {
        uint8_t value = i - '0';
        itemsize += static_cast<uint64_t >(value * std::pow(8, power));
        power--;
    }
    return itemsize;
}

// gets type of an item
std::string tar::getitemtype(char &n) {
    // read itemtype
    switch (n){
        case '0': case '\0':
            return tarconstant::typeFile;
        case '1':
            return tarconstant::typeHard;
        case '2':
            return tarconstant::typeSym;
        case '5':
            return tarconstant::typeDir;
        default:
            return tarconstant::typeOther;
    }
}