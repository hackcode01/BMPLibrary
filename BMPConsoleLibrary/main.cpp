#include "BMPImage.hpp"

#include <iostream>

int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");

    const int width = 640;
    const int height = 480;
    
    BMPImage image{ width, height };
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            image.setColor(Color((float)x / (float)width, 1.0f - ((float)x / (float)width),
                (float)y / (float)height), x, y);
        }
    }
    
    image.exportImage("image.bmp");
    
    BMPImage copyBmp{ 0, 0 };
    copyBmp.readImage("image.bmp");
    copyBmp.exportImage("copyBmp.bmp");

    std::cout << "\n\n";

    BMPImage imageBlackWhite;

    if (!imageBlackWhite.openBMP(argv[1])) {
        std::cout << "Ошибка при открытии файла!\n";
        return -2;
    }

    return 0;
}
