#include "./bmp_file.hpp"

#include <fstream>
#include <cmath>
#include <stdexcept>
#include <future>
#include <thread>
#include <iostream>

void BMP::openBMP(const std::string &fileName) {
    std::ifstream file{fileName, std::ios::binary};

    /* Проверка файла на открытие */
    if (!file.is_open()) {
        throw std::runtime_error{"Ошибка открытия файла: " + fileName};
    }

    /* Чтение заголовка файла */
    file.read(reinterpret_cast<char *>(&m_fileHeader), sizeof(m_fileHeader));
    if (file.gcount() != sizeof(m_fileHeader)) {
        throw std::runtime_error{"Ошибка при чтении заголовка файла"};
    }

    /* Чтение информационного заголовка */
    file.read(reinterpret_cast<char *>(&m_infoHeader), sizeof(m_infoHeader));
    if (file.gcount() != sizeof(m_infoHeader)) {
        throw std::runtime_error{"Ошибка при чтении информации о файле"};
    }

    /* Проверка, что данный BMP файл является 24 или 32 битным */
    if (m_infoHeader.bitCount != 24 && m_infoHeader.bitCount != 32) {
        throw std::runtime_error{"Этот формат файла BMP не поддерживается. "
                                 "Используйте 24- или 32-битный формат"};
    }

    /* Перемещение указателя файла к началу координат файла BMP */
    file.seekg(m_fileHeader.offsetData, std::ios::beg);

    /* Вычисление шага строки */
    m_rowStride = (m_infoHeader.width * (m_infoHeader.bitCount / 8) + 3) & ~3;

    /* Выделение памяти под пиксельные данные */
    m_pixelData.resize(static_cast<size_t>(m_rowStride) *
                       static_cast<size_t>(m_infoHeader.height));

    /* Чтение данных о пикселях */
    file.read(reinterpret_cast<char *>(m_pixelData.data()), m_pixelData.size());

    if (static_cast<size_t>(file.gcount()) != m_pixelData.size()) {
        throw std::runtime_error("Ошибка чтения пикселей");
    }

    file.close();
}

void BMP::drawLine(int32_t x_1, int32_t y_1, int32_t x_2, int32_t y_2,
                   uint8_t r, uint8_t g, uint8_t b) {
    /* Проверка, больше ли угол 45 градусов */
    bool steep = std::abs(y_2 - y_1) > std::abs(x_2 - x_1);

    /* Если угол больше 45 градусов, замена кординат x на y */
    if (steep) {
        std::swap(x_1, y_1);
        std::swap(x_2, y_2);
    }

    /* Проверка, что линия рисуется слева направо */
    if (x_1 > x_2) {
        std::swap(x_1, x_2);
        std::swap(y_1, y_2);
    }

    /* Разница по x */
    int32_t dx = x_2 - x_1;

    /* Разница по y */
    int32_t dy = std::abs(y_2 - y_1);

    /* Определение начальной ошибки, чтобы определить, когда нужно сдвигаться по оси y */
    int32_t error = dx / 2;

    /* Шаг по y, если линия рисуется вверх, то 1, если рисуется вниз, то -1 */
    int32_t ystep = (y_1 < y_2) ? 1 : -1;
    int32_t y = y_1;

    /* Перебор всех пикселей по оси x */
    for (int32_t x = x_1; x <= x_2; ++x) {
        /* Если угол больше 45 градусов, переворачиваем координаты, если меньше, не переворачиваем */
        if (steep) {
            setPixel(y, x, r, g, b);
        } else {
            setPixel(x, y, r, g, b);
        }

        error -= dy;

        /* Если значение ошибки стало отрицательным, то увеличиваем значение оси y */
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

void BMP::drawCross(uint8_t r, uint8_t g, uint8_t b) {
    /* Рисование первой диагональной линии от (0, 0) до (width-1, height-1) */
    drawLine(0, 0, m_infoHeader.width - 1, m_infoHeader.height - 1, r, g, b);

    /* Рисование второй диагональной линии от (width-1, 0) до (0, height-1) */
    drawLine(m_infoHeader.width - 1, 0, 0, m_infoHeader.height - 1, r, g, b);
}

void BMP::saveBMP(const std::string &fileName) {
    std::ofstream file(fileName, std::ios::binary);

    /* Проверка файла на открытие */
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка при открытии файла для записи");
    }

    /* Запись заголовка файла */
    file.write(reinterpret_cast<const char *>(&m_fileHeader), sizeof(m_fileHeader));
    /* Запись информационного заголовка */
    file.write(reinterpret_cast<const char *>(&m_infoHeader), sizeof(m_infoHeader));
    /* Запись всех пикселей в файл */
    file.write(reinterpret_cast<const char *>(m_pixelData.data()), m_pixelData.size());

    file.close();
}

bool BMP::hasMoreThanTwoColors() const {
    for (int32_t y = 0; y < m_infoHeader.height; ++y) {
        for (int32_t x = 0; x < m_infoHeader.width; ++x) {
            /* Получение индекса каждого пикселя */
            int32_t index = getPixelIndex(x, y);

            /* Вычисление каждого цвета пикселя */
            uint8_t blue = m_pixelData[index];
            uint8_t green = m_pixelData[static_cast<std::vector<uint8_t,
                std::allocator<uint8_t>>::size_type>(index) + 1];
            uint8_t red = m_pixelData[static_cast<std::vector<uint8_t,
                std::allocator<uint8_t>>::size_type>(index) + 2];

            /* Если пиксель содержит другие цвета, кроме черного или белого,
             * изображение не является черно-белым
             */
            if (!(red == 255 && green == 255 && blue == 255) &&
                !(red == 0 && green == 0 && blue == 0)) {
                return true;
            }
        }
    }

    return false;
}

void BMP::convertToBlackAndWhite() {
    /* Лямбда-функция для обработки диапазона строк */
    auto convertRow = [this](int32_t startRow, int32_t endRow,
        std::vector<uint8_t> &newPixelData) {

        for (int32_t y = startRow; y < endRow; ++y) {
            for (int32_t x = 0; x < m_infoHeader.width; ++x) {
                int32_t index = (y * m_rowStride) + (x * (m_infoHeader.bitCount / 8));

                /* Вычисление каждого цвета пикселя */
                uint8_t blue = m_pixelData[index];
                uint8_t green = m_pixelData[static_cast<std::vector<uint8_t,
                    std::allocator<uint8_t>>::size_type>(index) + 1];
                uint8_t red = m_pixelData[static_cast<std::vector<uint8_t,
                    std::allocator<uint8_t>>::size_type>(index) + 2];

                /* Вычисление яркости */
                double brightness = 0.2126 * red + 0.7152 * green + 0.0722 * blue;

                /* Установка пикселя в черный или белый цвет */
                if (brightness < m_brightnessFactor) {
                    newPixelData[index] = 0;
                    newPixelData[static_cast<std::vector<uint8_t,
                        std::allocator<uint8_t>>::size_type>(index) + 1] = 0;
                    newPixelData[static_cast<std::vector<uint8_t,
                        std::allocator<uint8_t>>::size_type>(index) + 2] = 0;
                } else {
                    newPixelData[index] = 255;
                    newPixelData[static_cast<std::vector<uint8_t,
                        std::allocator<uint8_t>>::size_type>(index) + 1] = 255;
                    newPixelData[static_cast<std::vector<uint8_t,
                        std::allocator<uint8_t>>::size_type>(index) + 2] = 255;
                }
            }
        }
    };

    /* Создание копии пиксельных данных для преобразования */
    std::vector<uint8_t> newPixelData = m_pixelData;

    /* Определение количества потоков на основе доступных ядер процессора */
    uint32_t countThreads = std::thread::hardware_concurrency();
    if (countThreads == 0) {
        countThreads = 1;
    }

    /* Вычисление количества строк на поток */
    int32_t rowsPerThread = m_infoHeader.height / countThreads;

    /* Вектор асинхронных задач */
    std::vector<std::future<void>> futures;

    /* Запуск потоков для обработки строк */
    for (uint32_t i = 0; i < countThreads; ++i) {
        int32_t startRow = i * rowsPerThread;
        int32_t endRow = (i == countThreads - 1) ?
            m_infoHeader.height :
            startRow + rowsPerThread;

        futures.push_back(std::async(std::launch::async,
            convertRow, startRow, endRow,std::ref(newPixelData)));
    }

    /* Ожидание завершения всех потоков */
    for (auto &future : futures) {
        future.get();
    }

    /* Обновление пиксельных данных преобразованными значениями */
    m_pixelData = std::move(newPixelData);
}

void BMP::displayBMP() {
    if (hasMoreThanTwoColors()) {
        std::cout << "Изображение содержит более двух цветов, "
                      "преобразуйте его в черно-белое...\n";
        convertToBlackAndWhite();
    }

    for (int32_t y = m_infoHeader.height - 1; y >= 0; y -= 1) {
        for (int32_t x = 0; x < m_infoHeader.width; ++x) {
            int32_t index = getPixelIndex(x, y);

            /* Вычисление каждого цвета пикселя */
            uint8_t blue = m_pixelData[index];
            uint8_t green = m_pixelData[static_cast<std::vector<uint8_t,
                std::allocator<uint8_t>>::size_type>(index) + 1];
            uint8_t red = m_pixelData[static_cast<std::vector<uint8_t,
                std::allocator<uint8_t>>::size_type>(index) + 2];

            /* Вывод символа для белого или черного пикселя */
            std::cout << ((red == 255 && green == 255 && blue == 255) ? WHITE : BLACK);
        }

        std::cout << '\n';
    }
}

void BMP::setPixel(const int32_t x, const int32_t y, const uint8_t r, const uint8_t g, const uint8_t b) {
    /* Проверка, что координаты не выходят за пределы изображения */
    if (x < 0 || x >= m_infoHeader.width || y < 0 || y >= m_infoHeader.height) {
        return;
    }

    /* Установка каждого цвета для пикселя */
    int32_t index = getPixelIndex(x, y);
    m_pixelData[index] = b;
    m_pixelData[index + 1] = g;
    m_pixelData[index + 2] = r;
}

inline int32_t BMP::getPixelIndex(int32_t x, int32_t y) const {
    return (y * m_rowStride) + (x * (m_infoHeader.bitCount / 8));
}
