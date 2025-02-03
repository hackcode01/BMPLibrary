#ifndef BMPIMAGE_HPP
#define BMPIMAGE_HPP

#include <Windows.h>
#include <fstream>
#include <vector>

/* Структура цвета для BMP файлов */
struct Color {
    /* Основной формат цвета RGB */
    float m_r{};
    float m_g{};
    float m_b{};

    /* Инициализация цветов при передаче параметров и по умолчанию */
    Color()
        : m_r{0.0f}, m_g{0.0f}, m_b{0.0f} {}

    Color(float r, float g, float b)
        : m_r{r}, m_g{g}, m_b{b} {}
};

/* Класс BMP файла */
class BMPImage {
public:
    BMPImage() {}

    /* Инициализация ширины, высоты и цвета в файле */
    BMPImage(int width, int height)
        : m_width{width}, m_height{height}
        , m_colors{ std::vector<Color>(width * height) } {}

    /* Получение цвета файла */
    Color getColor(int x, int y) const;
    
    /* Установление цвета файла */
    void setColor(const Color& color, int x, int y);

    /* чтение изображения BMP файла */
    void readImage(const char* path);
    
    /* Сохранение нового BMP файла */
    void exportImage(const char* path) const;

    /* Открытие BMP файла для чтения данных */
    bool openBMP(const std::string& fileName);

    /* Вывод цвета в символах в терминал */
    void displayBMP(char blackSymbol, char whiteSymbol);
    
    /* Закрытие BMP файла */
    void closeBMP();

private:
    /* Заголовок BMP файла */
    BITMAPFILEHEADER m_fileHeader{};

    /* Информационный заголовок BMP файла */
    BITMAPINFOHEADER m_infoHeader{};

    /* Данные изображения */
    unsigned char* m_imageData{};

    /* Поток для чтения BMP файла */
    std::ifstream m_file{};

    /* Ширина и высота файла */
    int m_width{};
    int m_height{};

    /* Цвета изображения BMP файла */
    std::vector<Color> m_colors{};
};

#endif
