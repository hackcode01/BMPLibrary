#ifndef BMPIMAGE_HPP
#define BMPIMAGE_HPP

#include <Windows.h>
#include <fstream>
#include <vector>

/* ��������� ����� ��� BMP ������ */
struct Color {
    /* �������� ������ ����� RGB */
    float m_r{};
    float m_g{};
    float m_b{};

    /* ������������� ������ ��� �������� ���������� � �� ��������� */
    Color()
        : m_r{0.0f}, m_g{0.0f}, m_b{0.0f} {}

    Color(float r, float g, float b)
        : m_r{r}, m_g{g}, m_b{b} {}
};

/* ����� BMP ����� */
class BMPImage {
public:
    BMPImage() {}

    /* ������������� ������, ������ � ����� � ����� */
    BMPImage(int width, int height)
        : m_width{width}, m_height{height}
        , m_colors{ std::vector<Color>(width * height) } {}

    /* ��������� ����� ����� */
    Color getColor(int x, int y) const;
    
    /* ������������ ����� ����� */
    void setColor(const Color& color, int x, int y);

    /* ������ ����������� BMP ����� */
    void readImage(const char* path);
    
    /* ���������� ������ BMP ����� */
    void exportImage(const char* path) const;

    /* �������� BMP ����� ��� ������ ������ */
    bool openBMP(const std::string& fileName);

    /* ����� ����� � �������� � �������� */
    void displayBMP(char blackSymbol, char whiteSymbol);
    
    /* �������� BMP ����� */
    void closeBMP();

private:
    /* ��������� BMP ����� */
    BITMAPFILEHEADER m_fileHeader{};

    /* �������������� ��������� BMP ����� */
    BITMAPINFOHEADER m_infoHeader{};

    /* ������ ����������� */
    unsigned char* m_imageData{};

    /* ����� ��� ������ BMP ����� */
    std::ifstream m_file{};

    /* ������ � ������ ����� */
    int m_width{};
    int m_height{};

    /* ����� ����������� BMP ����� */
    std::vector<Color> m_colors{};
};

#endif
