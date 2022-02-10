/*
 * by Marcel Nowicki (github@epenis.de | github.com/blindi0815)
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include "tarconst.h"
#include "tarfunc.h"

int main(int argc, char** argv) {

    // GENERAL VARIABLES
    //count the types of all items
    std::map<std::string, uint> typecount{
        {tarconstant::typeFile, 0}, {tarconstant::typeDir, 0}, {tarconstant::typeSym, 0},
        {tarconstant::typeHard, 0}, {tarconstant::typeOther, 0}
    };

    uint64_t sizeof_allfiles{}; // total size of all files in the archive

    // Trivial check for arguments. Errorprone and has to be changed.
    if (argc < 2) {
        std::cout << tarconstant::helptext << '\n';
        return 9;
    }
    // Getting name from argument lists on startup. Trivial and errorprone. Placeholder for now.
    std::string archiveFilename(argv[1]);

    //Open tar File.
    std::ifstream file(archiveFilename, std::ios::binary);
    if(!file) {
        std::cout << "Error opening file!" << '\n' << '\n';
        std::cout << tarconstant::helptext << '\n';
        return 9;
    }

    if (!tar::validTar(file)) {
        std::cout << archiveFilename << " is not a valid tar file for tarstats-pp" << '\n';
        return 9;
    }

    while (file) {
        //Read header of next item in tar archive
        char *headbuffer = new char[tarconstant::blocksize];
        file.read(headbuffer, tarconstant::blocksize);

        // tar file ends with 2 512byte blocks of 0. As no block should ever be 0 unless at the end, we check only once.
        if (tar::eof(headbuffer)) {
            break;
        }

        // Read type of item
        std::string itemtype = tar::getitemtype(headbuffer[tarconstant::itemtypeByte]);
        typecount[itemtype] += 1;

        // read itemsize and add to total
        sizeof_allfiles += tar::getitemsize(headbuffer);

        // ignore file content. we want to get to the next header. item types != FILE have no content blocks (0 byte)
        if (tar::getitemsize(headbuffer)!=0) {
            file.ignore((tar::getitemsize(headbuffer) / tarconstant::blocksize)
                        * tarconstant::blocksize + tarconstant::blocksize);
        }

        // empty the headerbuffer
        delete[] headbuffer;
    }
    file.close();
    std::cout << "Archive size:         " << std::filesystem::file_size(archiveFilename) << " Bytes"<< '\n';
    std::cout << "Size of all items:    " << sizeof_allfiles << " Bytes" << '\n' << '\n';
    for (auto &i : typecount) {
        std::cout << i.first <<": " << i.second << '\n';
    }

    return 0;
}
