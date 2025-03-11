#ifndef __BMP_FILE_HPP__
#define __BMP_FILE_HPP__

#include <cstdint>
#include <vector>
#include <string>

/* Выравнивание структуры по 1 байту */
#pragma pack(push, 1)

/* Структура для хранения заголовка BMP файла */
struct BMPFileHeader {
    /* Тип файла */
    uint16_t fileType{};

    /* Размер файла в байтах */
    uint32_t fileSize{};

    /* Первое зарезеривированные поле */
    uint16_t reserved_1{};

    /* Второе зарезеривированные поле */
    uint16_t reserved_2{};

    /* Смещение от начала файла до начала данных пикселей */
    uint32_t offsetData{};
};

/* Структура для хранения информационного заголовка BMP файла */
struct BMPInfoHeader {
    /* Размер структуры */
    uint32_t size;

    /* Ширина изображения в пикселях */
    int32_t width;

    /* Высота изображения в пикселях */
    int32_t height;

    /* Количество плоскостей */
    uint16_t planes;

    /* Количество бит на пиксель */
    uint16_t bitCount;

    /* Тип сжатия */
    uint32_t compression{};

    /* Размер данных изображения в байтах */
    uint32_t imageSize{};

    /* Горизонтальное разрешение в пикселях на метр */
    int32_t xPixelsPerMeter{};

    /* Вертикальное разрешение в пикселях на метр */
    int32_t yPixelsPerMeter{};

    /* Количество используемых цветов */
    uint32_t colorsUsed{};
};

/* Возвращение к стандартному выравниваю компилятора */
#pragma pack(pop)

/* Класс для работы с BMP файлами */
class BMP {

public:
    BMP() = default;

    /* Очищение пикселей */
    ~BMP() {
        m_pixelData.clear();
    }

    /* Метод по открытию файла BMP */
    void openBMP(const std::string& fileName);

    /* Метод по рисованию линий на изображении */
    void drawLine(int32_t x_1, int32_t y1, int32_t x2, int32_t y2,
                  uint8_t r, uint8_t g, uint8_t b);

    /* Метод для рисования двух перекрестных линий на изобажении */
    void drawCross(uint8_t r, uint8_t g, uint8_t b);

    /* Метод для сохранения нового изображения */
    void saveBMP(const std::string& fileName);

    /* Метод для проверки содержания в изображении более двух цветов */
    bool hasMoreThanTwoColors() const;

    /* Метод для конвертирования исходного изображения в черно-белое */
    void convertToBlackAndWhite();

    /* Отображение изображения в консоли */
    void displayBMP();

private:
    /* Поле для хранения заголовка файла */
    BMPFileHeader m_fileHeader{};

    /* Поле для хранения информационного заголовка */
    BMPInfoHeader m_infoHeader{};

    /* Поле для хранения данных пикселей изображения */
    std::vector<uint8_t> m_pixelData{};

    /* Шаг строки в байтах */
    int32_t m_rowStride{ 0 };

    /* Cимвол для отображения белого цвета */
    constexpr static char WHITE{ '*' };

    /* Символ для хранения черного цвета */
    constexpr static char BLACK{ '#' };

    /* Пороговое значение яркости для преобразования в черно-белое */
    constexpr static uint8_t m_brightnessFactor{ 128 };

    /* Метод по назначению пикселю цвета */
    void setPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b);

    /* Метод по вычислению индекса пикселя в векторе пикселей по координатам */
    inline int32_t getPixelIndex(int32_t x, int32_t y) const;
};

#endif
