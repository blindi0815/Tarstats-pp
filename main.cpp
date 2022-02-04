#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream datei("test.tar", std::ios::binary);
    if(!datei)
        std::cout << "Fehler beim Oeffnen der Datei" << std::endl;

    int buffersize = 512;
    char* buffer = new char[buffersize];

    datei.read(buffer, buffersize);
    std::string filename{};
    for (int i = 0; i <= 99; i++)
    {
        if (buffer[i]!= '\0')
            filename.push_back(buffer[i]);
    }
    std::cout << filename << std::endl;

    std::string filename2(&buffer[0], 100);
    std::cout << filename2 << std::endl;

    return 0;
}
