#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>




int main(int argc, char** argv) {

    // Trivial check for arguments. Errorprone and has to be changed.
    if (argc < 2) {
        std::cout << "Please enter filename" << std::endl;
        return 9;
    }
    // Getting name from argument lists on startup. Trivial and errorprone. Placeholder for now.
    std::string archiveFilename(argv[1]);

    //Open tar File.
    std::ifstream datei(archiveFilename, std::ios::binary);
    if(!datei)
        std::cout << "Error opening file" << std::endl;

    // Tar spec is working with continous 512 byte size blocks. Header is 512 bytes.
    int buffersize = 512;

    //Read header of first item in tar archive
    char* headbuffer = new char[buffersize];
    datei.read(headbuffer, buffersize);

    // Read name of the next item
    std::string itemname(&headbuffer[0], 100);
    std::erase(itemname, '\0');

    // Read type of item
    std::string itemtype{};
    switch (headbuffer[156]){
        case '0': case '\0':
            itemtype = "FILE";
            break;
        case '1':
            itemtype = "HARDLINK";
            break;
        case '2':
            itemtype = "SYMLINK";
            break;
        case '5':
            itemtype = "DIRECTORY";
            break;
        default:
            itemtype = "OTHER";
            break;
    }




    // Read size of the next file
    long double itemsize{};
    int power = 10;
    std::string asciifilesize(&headbuffer[124], 11);
    for (auto i : asciifilesize) {
        int value = i - '0';
        itemsize += value * std::pow(8, power);
        power--;
    }

    // uint64_t locationtonextfile{};
    /* find out how often 512 bytes have to be read until the next header begins. Itemcontents are filled with '0' to
     * fill the next 512 byte block
     */
    int filereadcount = (itemsize / buffersize) + 1;
    char* itembuffer = new char[buffersize];

    while (filereadcount >= 0){
        datei.read(itembuffer, buffersize);
        filereadcount--;
    }
    delete[] itembuffer;
    delete[] headbuffer;


    headbuffer = new char[buffersize];
    datei.read(headbuffer, buffersize);
    std::string itemname2 (&headbuffer[0], 100);
    std::erase(itemname2, '\0');

    std::string itemtype2{};
    switch (headbuffer[156]){
        case '0': case '\0':
            itemtype2 = "FILE";
            break;
        case '1':
            itemtype2 = "HARDLINK";
            break;
        case '2':
            itemtype2 = "SYMLINK";
            break;
        case '5':
            itemtype2 = "DIRECTORY";
            break;
        default:
            itemtype2 = "OTHER";
            break;
    }


    //Printing to stdout
    std::cout << itemname << std::endl;
    std::cout << itemtype << std::endl;
    std::cout << itemsize << " Bytes" << std::endl;

    //Second item
    std::cout << itemname2 << std::endl;
    std::cout << itemtype2 << std::endl;


    return 0;
}
