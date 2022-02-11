/* this file contains function definitions for parsing a tar file.
 *
 * by Marcel Nowicki (github@epenis.de | github.com/blindi0815)
 */

#include "tarfunc.h"
#include "tarconst.h"
#include <string>
#include <cmath>
#include <iostream>
#include <map>
#include "zlib.h"

// checks if a valid modern tar file - ustar
bool tar::validTar(std::istream &file) {
    file.seekg(tarconstant::mgcfieldByte.first);
    char* buffer = new char [tarconstant::mgcfieldByte.second];
    file.read(buffer, tarconstant::mgcfieldByte.second);
    std::string magicfield(&buffer[0], 5);
    delete[] buffer;
    file.seekg(0);
    return magicfield == tarconstant::mgctar ? true : false;
}

// check if tar in gzfile is valid
bool tar::gzValidTar(std::string filename) {
    auto gzIn = ::gzopen(filename.c_str(), "r");
    gzbuffer(gzIn, 8192);
    char headbuffer[512] = {0};
    gzread( gzIn, headbuffer, sizeof(headbuffer));
    std::string magicfield (&headbuffer[tarconstant::mgcfieldByte.first], 5);
    gzclose(gzIn);
    if (magicfield == tarconstant::mgctar)
        return true;
    return false;
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

// writes stats to console in default style
void tar::consolestats (std::map<std::string, uintmax_t> &typecount, uintmax_t tarfilesize, uintmax_t sizeofall) {
    std::cout << "Archive size:         " << tarfilesize << " Bytes"<< '\n';
    std::cout << "Size of all items:    " << sizeofall << " Bytes" << '\n' << '\n';
    for (auto &i : typecount) {
        std::cout << i.first <<": " << i.second << '\n';
    }
}

// writes default console output to txt file
void tar::txtfilestats (std::map<std::string, uintmax_t> &typecount, uintmax_t tarfilesize, uintmax_t sizeofall,
                   std::string archiveName) {
    std::string txtname = archiveName + ".txt";
    std::ofstream txtfile(txtname);
    txtfile << "Archive size:         " << tarfilesize << " Bytes"<< std::endl;
    txtfile << "Size of all items:    " << sizeofall << " Bytes" << std::endl << std::endl;
    for (auto &i : typecount) {
        txtfile << i.first <<": " << i.second << std::endl;
    }
    txtfile.close();
    std::cout << "Stats written to " << txtname << '\n' << '\n';
}

// print out helpertext
void tar::printhelp(){
    std::cout << tarconstant::helptext << '\n' << '\n';
}

// check if valid GNU ZIP file via magic byte
bool tar::validGzip(std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    char byte;
    file.seekg(1);
    file.read((&byte), 1);
    file.close();
    if (byte == tarconstant::mgcgzip)
        return true;

    return false;
}

// check if file can be opened
bool tar::fileOpen(std::string &filename) {
    std::ifstream file(filename);
    if(file) {
        file.close();
        return true;
    }

    file.close();
    return false;
}