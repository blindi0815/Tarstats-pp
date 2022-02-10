/*
 * by Marcel Nowicki (github@epenis.de | github.com/blindi0815)
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include "tarconst.h"
#include "tarfunc.h"

int main(int argc, char** argv) {
    // Trivial check for arguments.
    if (argc < 2) {
        tar::printhelp();
        return 9;
    }

    // GENERAL VARIABLES

    // Load command line parameters into vector, read them and set variables
    std::vector<std::string> cmdparam{};
    for (int i = 1; i < argc; i++){
        cmdparam.emplace_back(argv[i]);
    }
    bool toFile = false;
    bool toJSON = false;

    std::vector<std::string> archiveFilename {};
    for (auto i : cmdparam) {
        if (i[0] != '-' && i.size() > 1) {
            archiveFilename.push_back(i);
        }
        if (i[0] == '-') {
        for (auto j : i) {
            switch (j) {
                case 'j':
                    toJSON = true;
                    break;
                case 'f':
                    toFile = true;
                    break;
                case 'h':
                    tar::printhelp();
                    return 9;
                case '-':
                    break;
                default:
                    std::cout << "Invalid optional argument '" << j << "'!" << '\n' << '\n';
                    tar::printhelp();
                    return 9;
            }
        }
        }
    }

    for (auto &archiveName : archiveFilename) {
        //count the types of all items
        std::map<std::string, uintmax_t> typecount{
                {tarconstant::typeFile, 0}, {tarconstant::typeDir, 0}, {tarconstant::typeSym, 0},
                {tarconstant::typeHard, 0}, {tarconstant::typeOther, 0}
        };

        uint64_t sizeof_allfiles{}; // total size of all files in the archive

        //Open tar File.
        std::ifstream file(archiveName, std::ios::binary);
        if (!file) {
            std::cout << "Error opening file " << archiveName << "!" << '\n' << '\n';
            tar::printhelp();
            return 9;
        }

        if (!tar::validTar(file)) {
            std::cout << "!!!!!!!" << '\n';
            std::cout << archiveName << " is not a valid tar file for tarstats-pp" << '\n';
            std::cout << "!!!!!!!" << '\n' << '\n';
        }
        else {
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
                if (tar::getitemsize(headbuffer) != 0) {
                    file.ignore((tar::getitemsize(headbuffer) / tarconstant::blocksize)
                                * tarconstant::blocksize + tarconstant::blocksize);
                }

                // empty the headerbuffer
                delete[] headbuffer;
            }
            file.close();

            std::cout << toJSON << '\n';
            tar::consolestats(typecount, std::filesystem::file_size(archiveName), sizeof_allfiles);
            if (toFile) {
                tar::txtfilestats(typecount, std::filesystem::file_size(archiveName), sizeof_allfiles,
                                  archiveName);
            }
        }
    }

    return 0;
}
