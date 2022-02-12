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
#include "zlib.h"

int main(int argc, char** argv) {
    // Trivial check for arguments.
    if (argc < 2) {
        tar::printhelp();
        return 9;
    }

    // GENERAL VARIABLES
    bool toFile = false;
    bool toJSON = false;
    uintmax_t global_sizeofallitems{};
    uintmax_t global_sizeofallfiles{};
    std::map<std::string, uintmax_t> globaltypecount{
            {tarconstant::typeFile,  0},
            {tarconstant::typeDir,   0},
            {tarconstant::typeSym,   0},
            {tarconstant::typeHard,  0},
            {tarconstant::typeOther, 0}
    };

    // Load command line parameters into vector, read them and set variables
    std::vector<std::string> cmdparam{};
    for (int i = 1; i < argc; i++){
        cmdparam.emplace_back(argv[i]);
    }

    std::vector<std::string> archiveFilename {};
    for (auto i : cmdparam) {
        if (i[0] != '-' && i.size() > 1) {
            if(!tar::fileOpen(i)){
                tar::printhelp();
                std::cout << "Error opening file " << i <<" - closing down" << '\n';
                return 9;
            }
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

    // Core loop - opening files, check if compressed or not and parse accordingly.

    for (auto &archiveName : archiveFilename) {
        //count the types of all items
        std::map<std::string, uintmax_t> typecount{
                {tarconstant::typeFile,  0},
                {tarconstant::typeDir,   0},
                {tarconstant::typeSym,   0},
                {tarconstant::typeHard,  0},
                {tarconstant::typeOther, 0}
        };

        uint64_t sizeof_allitems{}; // total size of all files in the archive
        bool isGz = tar::validGzip(archiveName);

        if (isGz) {
            if (!tar::gzValidTar(archiveName)) {
                std::cout << "!!!!!!!" << '\n';
                std::cout << archiveName << " is not a valid tar file for tarstats-pp" << '\n';
                std::cout << "!!!!!!!" << '\n' << '\n';
            }
            else {
                gzFile gzIn = gzopen(archiveName.c_str(), "r");
                gzbuffer(gzIn, 8192);
                while (!gzeof(gzIn)) {

                    char headbuffer[512] = {0};
                    uint64_t unzippedbytes = gzread(gzIn, headbuffer, sizeof(headbuffer));
                    if (unzippedbytes == 0)
                        break;

                    // tar file ends with 2 512byte blocks of 0. As no block should ever be 0 unless at the end,
                    // we check only once.
                    if (tar::eof(headbuffer)) {
                        break;
                    }

                    // Read type of item
                    std::string itemtype = tar::getitemtype(headbuffer[tarconstant::itemtypeByte]);
                    typecount[itemtype] += 1;
                    globaltypecount[itemtype] += 1;

                    // read itemsize and add to total
                    sizeof_allitems += tar::getitemsize(headbuffer);
                    global_sizeofallitems += tar::getitemsize(headbuffer);

                    // ignore file content. we want to get to the next header. item types != FILE == 0 bytes
                    uintmax_t help = (tar::getitemsize(headbuffer) / tarconstant::blocksize)
                            * tarconstant::blocksize + tarconstant::blocksize;
                    char dump[help] = {0};

                    if (tar::getitemsize(headbuffer) != 0) {
                        gzread(gzIn, dump, (tar::getitemsize(headbuffer) / tarconstant::blocksize)
                                              * tarconstant::blocksize + tarconstant::blocksize);
                    }
                }
                gzclose(gzIn);
                global_sizeofallfiles += std::filesystem::file_size(archiveName);

                if(toJSON){
                    if(!toFile) {
                        tar::jsonconsolestats(typecount, std::filesystem::file_size(archiveName),
                                              sizeof_allitems, archiveName, "gzarchive");
                    }
                    if(toFile) {
                        tar::jsonfilestats(typecount, std::filesystem::file_size(archiveName),
                                              sizeof_allitems, archiveName, "gzarchive");
                    }
                }
                if(!toJSON) {
                    if(toFile)
                        tar::txtfilestats(typecount, std::filesystem::file_size(archiveName),
                                          sizeof_allitems, archiveName);
                    if(!toFile)
                        tar::consolestats(typecount, std::filesystem::file_size(archiveName),
                                          sizeof_allitems);
                }
            }
        }

        if (!isGz) {
            //Open tar File.
            std::ifstream file(archiveName, std::ios::binary);

            if (!tar::validTar(file)) {
                std::cout << "!!!!!!!" << '\n';
                std::cout << archiveName << " is not a valid tar file for tarstats-pp" << '\n';
                std::cout << "!!!!!!!" << '\n' << '\n';
            } else {
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
                    globaltypecount[itemtype] += 1;

                    // read itemsize and add to total
                    sizeof_allitems += tar::getitemsize(headbuffer);
                    global_sizeofallitems += tar::getitemsize(headbuffer);

                    // ignore file content. we want to get to the next header. item types != FILE have no content blocks (0 byte)
                    if (tar::getitemsize(headbuffer) != 0) {
                        file.ignore((tar::getitemsize(headbuffer) / tarconstant::blocksize)
                                    * tarconstant::blocksize + tarconstant::blocksize);
                    }

                    // empty the headerbuffer
                    delete[] headbuffer;
                }
                file.close();
                global_sizeofallfiles += std::filesystem::file_size(archiveName);

                if(toJSON){
                    if(!toFile) {
                        tar::jsonconsolestats(typecount, std::filesystem::file_size(archiveName),
                                              sizeof_allitems, archiveName, "archive");
                    }
                    if(toFile) {
                        tar::jsonfilestats(typecount, std::filesystem::file_size(archiveName),
                                           sizeof_allitems, archiveName, "archive");
                    }
                }
                if(!toJSON) {
                    if(toFile)
                        tar::txtfilestats(typecount, std::filesystem::file_size(archiveName),
                                                sizeof_allitems, archiveName);
                    if(!toFile)
                        tar::consolestats(typecount, std::filesystem::file_size(archiveName),
                                          sizeof_allitems);
                }
            }
        }
    }
    if(toFile && toJSON)
        tar::jsonglobalfile(globaltypecount, global_sizeofallfiles,
                            global_sizeofallitems, archiveFilename.size());

    tar::consoleglobalstats(globaltypecount, global_sizeofallfiles,
                            global_sizeofallitems, archiveFilename.size());
    return 0;
}
