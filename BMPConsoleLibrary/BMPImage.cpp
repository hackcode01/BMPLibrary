#include "BMPImage.hpp"

#include <iostream>
#include <thread>

Color BMPImage::getColor(int x, int y) const
{
    /* Возврат цветов файла */
    return m_colors[static_cast<std::vector<Color,
        std::allocator<Color>>::size_type>(y) * m_width + x];
}

void BMPImage::setColor(const Color& color, int x, int y) {
    /* Установка каждого цвета формата RGB изображения */
    m_colors[static_cast<std::vector<Color,
        std::allocator<Color>>::size_type>(y) * m_width + x].m_r = color.m_r;
    m_colors[static_cast<std::vector<Color,
        std::allocator<Color>>::size_type>(y) * m_width + x].m_g = color.m_g;
    m_colors[static_cast<std::vector<Color,
        std::allocator<Color>>::size_type>(y) * m_width + x].m_b = color.m_b;
}

void BMPImage::readImage(const char* path) {
    setlocale(LC_ALL, "Russian");

    /* Открытие потока для чтения файла */
    std::ifstream file{};
    file.open(path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Файл не удается открыть\n";
        return;
    }

    /* Установка размеров заголовка и информационного заголовка файла */
    const int fileHeaderSize = 14;
    const int informationHeaderSize = 40;

    unsigned char fileHeader[fileHeaderSize]{};
    file.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

    /* Проверка, является ли файл растровым изображением */
    if (fileHeader[0] != 'B' || fileHeader[1] != 'M') {
        std::cout << "Указанный путь не является растровым изображением\n";
        file.close();
        return;
    }

    unsigned char informationHeader[informationHeaderSize]{};
    file.read(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

    /* Определение размера, ширины и высоты файла */
    int fileSize = fileHeader[2] + (fileHeader[3] << 8) +
        (fileHeader[4] << 16) +
        (fileHeader[5] << 24);
    m_width = informationHeader[4] + (informationHeader[5] << 8) +
        (informationHeader[6] << 16) +
        (informationHeader[7] << 24);
    m_height = informationHeader[8] + (informationHeader[9] << 8) +
        (informationHeader[10] << 16) +
        (informationHeader[11] << 24);

    m_colors.resize(m_width * m_height);

    const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

    /* Чтение цветов изображения */
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            unsigned char color[3]{};
            file.read(reinterpret_cast<char*>(color), 3);

            m_colors[static_cast<std::vector<Color,
                std::allocator<Color>>::size_type>(y) * m_width + x].m_r =
                static_cast<float>(color[2]) / 255.0f;

            m_colors[static_cast<std::vector<Color,
                std::allocator<Color>>::size_type>(y) * m_width + x].m_g =
                static_cast<float>(color[1]) / 255.0f;

            m_colors[static_cast<std::vector<Color,
                std::allocator<Color>>::size_type>(y) * m_width + x].m_b =
                static_cast<float>(color[0]) / 255.0f;
        }

        file.ignore(paddingAmount);
    }

    file.close();

    std::cout << "Файл прочитан\n";
}

void BMPImage::exportImage(const char* path) const {
    setlocale(LC_ALL, "Russian");

    /* Открытие потока для записи файла */
    std::ofstream file{};
    file.open(path, std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Файл не удалось открыть";
        return;
    }

    /* Указание отступов, размера заголовка, размера информационного заголовка,
     * размера файла
     */
    unsigned char bmpPadding[3] = { 0, 0, 0 };
    const int paddingAmount = ((4 - (m_width * 3) % 4) % 4);

    const int fileHeaderSize = 14;
    const int informationHeaderSize = 40;
    const int fileSize = fileHeaderSize + informationHeaderSize + m_width *
        m_height * 3 + paddingAmount * m_width;

    /* Создание заголовка файла и указание значений его элементов */
    char fileHeader[fileHeaderSize]{};

    /* Тип файла */
    fileHeader[0] = 'B';
    fileHeader[1] = 'M';

    /* Размер файла */
    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize >> 8;
    fileHeader[4] = fileSize >> 16;
    fileHeader[5] = fileSize >> 24;

    /* Первый резерв - не используется */
    fileHeader[6] = 0;
    fileHeader[7] = 0;

    /* Второй резерв - не используется */
    fileHeader[8] = 0;
    fileHeader[9] = 0;

    /* Смещение пиксельных данных */
    fileHeader[10] = fileHeaderSize * informationHeaderSize;
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;

    /* Создание информационого заголовка файла и указание значений его элементов */
    char informationHeader[informationHeaderSize]{};

    /* Размер загловка */
    informationHeader[0] = informationHeaderSize;
    informationHeader[1] = 0;
    informationHeader[2] = 0;
    informationHeader[3] = 0;

    /* Ширина изображения */
    informationHeader[4] = m_width;
    informationHeader[5] = m_width >> 8;
    informationHeader[6] = m_width >> 16;
    informationHeader[7] = m_width >> 24;

    /* Высота изоброжения */
    informationHeader[8] = m_height;
    informationHeader[9] = m_height >> 8;
    informationHeader[10] = m_height >> 16;
    informationHeader[11] = m_height >> 24;

    /* Плоскости */
    informationHeader[12] = 1;
    informationHeader[13] = 0;

    /* Бит на пиксель - RGB */
    informationHeader[14] = 24;
    informationHeader[15] = 0;

    /* Сжатие - без сжатия */
    informationHeader[16] = 0;
    informationHeader[17] = 0;
    informationHeader[18] = 0;
    informationHeader[19] = 0;

    /* Размер изображения - без сжатия */
    informationHeader[20] = 0;
    informationHeader[21] = 0;
    informationHeader[22] = 0;
    informationHeader[23] = 0;

    /* X пикселей на метр */
    informationHeader[24] = 0;
    informationHeader[25] = 0;
    informationHeader[26] = 0;
    informationHeader[27] = 0;

    /* Y пикселей на метр */
    informationHeader[28] = 0;
    informationHeader[29] = 0;
    informationHeader[30] = 0;
    informationHeader[31] = 0;

    /* Количество цветов (Цветовая палитра не используется) */
    informationHeader[32] = 0;
    informationHeader[33] = 0;
    informationHeader[34] = 0;
    informationHeader[35] = 0;

    /* Важные цвета - в основном игнорируются */
    informationHeader[36] = 0;
    informationHeader[37] = 0;
    informationHeader[38] = 0;
    informationHeader[39] = 0;

    /* Запись заголовка и информационного заголовка в новый файл */
    file.write(reinterpret_cast<char*>(fileHeader), fileHeaderSize);
    file.write(reinterpret_cast<char*>(informationHeader), informationHeaderSize);

    /* Запись цветов и отступов в файл */
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            unsigned char r = static_cast<unsigned char>(getColor(x, y).m_r * 255.0f);
            unsigned char g = static_cast<unsigned char>(getColor(x, y).m_g * 255.0f);
            unsigned char b = static_cast<unsigned char>(getColor(x, y).m_b * 255.0f);

            unsigned char color[] = { b, g, r };

            file.write(reinterpret_cast<char*>(color), 3);
        }

        file.write(reinterpret_cast<char*>(bmpPadding), paddingAmount);
    }

    file.close();

    std::cout << "Файл создан!\n";
}

bool BMPImage::openBMP(const std::string& fileName) {
    m_file.open(fileName, std::ios::in | std::ios::binary);

    if (!m_file.is_open()) {
        return false;
    }

    m_file.read((char*)&m_fileHeader, sizeof(BITMAPFILEHEADER));
    m_file.read((char*)&m_infoHeader, sizeof(BITMAPINFOHEADER));

    if (m_fileHeader.bfType != 0x4D42 ||
        (m_infoHeader.biBitCount != 24 && m_infoHeader.biBitCount != 32)) {
        m_file.close();
        return false;
    }

    m_imageData = new unsigned char[m_infoHeader.biSizeImage];
    m_file.seekg(m_fileHeader.bfOffBits, std::ios::beg);
    m_file.read((char*)m_imageData, m_infoHeader.biSizeImage);

    return true;
}

void BMPImage::displayBMP(char blackSymbol, char whiteSymbol) {
    setlocale(LC_ALL, "Russian");

    for (int y = 0; y < m_infoHeader.biHeight; ++y) {
        for (int x = 0; x < m_infoHeader.biWidth; ++x) {
            int index = (y * m_infoHeader.biWidth + x) * 3;

            /* Проверка, является цвет черным или белым */
            if (m_imageData[index] == 0 && m_imageData[index + 1] == 0 &&
                m_imageData[index + 2] == 0) {
                std::cout << blackSymbol;
            } else {
                std::cout << whiteSymbol;
            }
        }

        std::cout << '\n';
    }
}

void BMPImage::closeBMP() {
    /* Очистка выделенной памяти и закрытие файла */
    delete[] m_imageData;

    m_file.close();
}
