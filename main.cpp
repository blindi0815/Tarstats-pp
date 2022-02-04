#include <iostream>
#include <fstream>
#include <string>

int main() {
    if (std::ifstream datei{"test.tar", std::ios::binary}) {
        int buffersize = 512;
        char *buffer;

        datei.read(buffer, buffersize);


        datei.close();

        std::cout << buffer[0] << buffer[1] << std::endl;
    }

    else
        std::cout << " Fehler ! " << std::endl;
    return 0;
}
