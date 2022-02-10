/* this header file contains function declarations used parsing a tar file
 *
 * by Marcel Nowicki (github@epenis.de | github.com/blindi0815)
 */

#include <fstream>


#ifndef TARSTATS___TARFUNC_H
#define TARSTATS___TARFUNC_H
namespace tar {
// checks if a valid modern tar file - ustar
    bool validTar(std::istream &file);

// checks if a 512byte block consist only of 0 or \0
    bool eof(const char *buf);

// gets size of an item in bytes. assumes octal encoding.
    uint64_t getitemsize(const char *buf);

// gets type of an item
    std::string getitemtype(char &n);
}


#endif //TARSTATS___TARFUNC_H
