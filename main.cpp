#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream datei("test.tar", std::ios::binary);
    if(!datei)
        std::cout << "Fehler beim Oeffnen der Datei" << std::endl;

    int headersize = 512;
    char* headbuffer = new char[headersize];

    datei.read(headbuffer, headersize);
    std::string filename{};
    for (int i = 0; i <= 99; i++)
    {
        if (headbuffer[i] != '\0')
            filename.push_back(headbuffer[i]);
    }
    std::cout << filename << std::endl;

    std::string filename2(&headbuffer[0], 100);
    std::cout << filename2 << std::endl;

    return 0;
}
