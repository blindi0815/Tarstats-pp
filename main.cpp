#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <filesystem>

// global itemtype definition
std::string const typeFile = "FILES";
std::string const typeDir = "DIRECTORIES";
std::string const typeSym = "SYMLINKS";
std::string const typeHard = "HARDLINKS";
std::string const typeOther = "OTHER";

// checks if a valid modern tar file - ustar
bool validTar (std::istream & file) {
    file.seekg(257);
    char* buffer = new char [6];
    file.read(buffer, 6);
    std::string magicfield(&buffer[0], 5);
    delete[] buffer;
    file.seekg(0);
    return magicfield == "ustar" ? true: false;
}

// checks if a 512byte block consist only of 0 or \0
bool eof (const char* buf) {
    std::string test(&buf[0]);
    std::erase(test, '\0');
    std::erase(test, '0');
    if (test.size() == 0)
        return 1;
    return 0;
}

// gets size of an item in bytes. assumes octal encoding.
uint64_t getitemsize(const char* buf){
    std::string asciisize(&buf[124], 11); // at offset 124 because first byte marks octal or base256 coding
    uint64_t itemsize{};
    uint8_t power = 10;
    for (auto i : asciisize) {
        uint8_t value = i - '0';
        itemsize += static_cast<uint64_t >(value * std::pow(8, power));
        power--;
    }
    return itemsize;
}

// gets type of an item
std::string getitemtype(char &n) {
    // read itemtype
    switch (n){
        case '0': case '\0':
            return typeFile;
        case '1':
            return typeHard;
        case '2':
            return typeSym;
        case '5':
            return typeDir;
        default:
            return typeOther;
    }
}



int main(int argc, char** argv) {

    // GENERAL VARIABLES
    //count the types of all items
    std::map<std::string, uint> typecount{
        {typeFile, 0}, {typeDir, 0}, {typeSym, 0},
        {typeHard, 0}, {typeOther, 0}
    };

    uint64_t sizeof_allfiles{}; // total size of all files in the archive

    std::string helptext
    {"usage: tarstats-pp [-h] [-j] [-f] tarfile.\n"
     "A tool to calculate basics statistics on tarball. Shamelessly inspired by github.com/isotopp/tarstats!\n\n"
     "mandatory argument:\n"
     "tarfile   a tarfile to calculate stats on\n\n"
     "optional arguments\n"
     "-h        this helptext\n"
     "-j        stats in JSON format printed to stdout\n"
     "-f        print stats to file <tarfilename>.txt or .json if -j invoked"};

    // Trivial check for arguments. Errorprone and has to be changed.
    if (argc < 2) {
        std::cout << helptext << '\n';
        return 9;
    }
    // Getting name from argument lists on startup. Trivial and errorprone. Placeholder for now.
    std::string archiveFilename(argv[1]);

    //Open tar File.
    std::ifstream file(archiveFilename, std::ios::binary);
    if(!file) {
        std::cout << "Error opening file!" << '\n' << '\n';
        std::cout << helptext << '\n';
        return 9;
    }

    if (!validTar(file)) {
        std::cout << archiveFilename << " is not a valid tar file for tarstats-pp" << '\n';
        return 9;
    }

    // Tar spec is working with continous 512 byte size blocks. Header is 512 bytes.
    int buffersize = 512;
    while (file) {
        //Read header of first item in tar archive
        char *headbuffer = new char[buffersize];
        file.read(headbuffer, buffersize);

        // tar file ends with 2 512byte blocks of 0. As no block should ever be 0 unless at the end, we check only once.
        if (eof(headbuffer)) {
            break;
        }

        // Read type of item
        std::string itemtype{};
        itemtype = getitemtype(headbuffer[156]);
        typecount[itemtype] += 1;


        // read itemsize and add to total
        sizeof_allfiles += getitemsize(headbuffer);

        // ignore file content. we want to get to the next header. item types != FILE have no content blocks (0 byte)
        if (getitemsize(headbuffer)!=0) {
            file.ignore((getitemsize(headbuffer) / 512) * 512 + 512);
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
