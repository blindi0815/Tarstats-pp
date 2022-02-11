/* this headerfile contains global constant variables needed during parsing of a tar file
 *
 * by Marcel Nowicki (github@epenis.de | github.com/blindi0815)
 */

#ifndef TARSTATS___TARCONST_H
#define TARSTATS___TARCONST_H

#include <string>
#include <utility>

namespace tarconstant {
    // global itemtype definition
    inline std::string const typeFile = "FILES";
    inline std::string const typeDir = "DIRECTORIES";
    inline std::string const typeSym = "SYMLINKS";
    inline std::string const typeHard = "HARDLINKS";
    inline std::string const typeOther = "OTHER";

    // valid magicfield tar
    inline std::string const mgctar = "ustar";
    inline constexpr char mgcgzip = '\213';

    // Byte offsets
    inline uint16_t blocksize = 512; // tar consists of continuous 512 Byte data blocks
    inline uint8_t itemtypeByte = 156;
    inline std::pair<uint16_t, uint8_t> mgcfieldByte {257,6};      // { offset , length }
    inline std::pair<uint8_t , uint8_t> itemoctlnByte {124,11};    // at offset 124 because first byte
                                                                        // marks octal or base256 coding
    inline std::string helptext
            {"usage: tarstats-pp [-h] [-j] [-f] tarfile.\n"
             "A tool to calculate basics statistics on tarball. Shamelessly inspired by github.com/isotopp/tarstats!\n\n"
             "mandatory argument:\n"
             "tarfile   a tarfile to calculate stats on\n\n"
             "optional arguments\n"
             "-h        this helptext\n"
             "-j        stats in JSON format printed to stdout\n"
             "-f        print stats to file <tarfilename>.txt or .json if -j invoked\n\n"
             "Find more information as well as full source at github.com/blindi0815/Tarstats-pp."
             "This software is as is under a GPL 3.0 license.\n\n"
             "Written by Marcel Nowicki - github.com/blindi0815"};
}

#endif //TARSTATS___TARCONST_H
