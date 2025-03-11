#include "./bmp/bmp_file.hpp"

#include <Windows.h>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    BMP bmp{};
    try {
        bmp.openBMP(argv[1]);
        std::cout << "Оригинальное изображение:\n";
        bmp.displayBMP();

        bmp.drawCross(0, 255, 0);

        std::this_thread::sleep_for(std::chrono::seconds{5});

        std::cout << "\nИзображение с крестом:\n";
        bmp.displayBMP();

        std::string saveFileName;
        std::cout << "Введите имя для нового BMP-файла: ";
        std::getline(std::cin, saveFileName, '\n');
        bmp.saveBMP(saveFileName);
        std::cout << "Изображение сохранено в " << saveFileName << '\n';
    } catch (const std::exception& exception) {
        std::cerr << "Ошибка: " << exception.what() << '\n';
        return 1;
    }

    return 0;
}
