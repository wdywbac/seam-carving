#include "Image.h"

Image::Image(std::vector<std::vector<Image::Pixel>> table)
        : m_table(std::move(table))
{}

Image::Pixel::Pixel(int red, int green, int blue)
        : m_red(red)
        , m_green(green)
        , m_blue(blue)
{}

Image::Pixel::Pixel()
        : m_red(0), m_green(0), m_blue(0) {}

Image::Pixel Image::GetPixel(size_t columnId, size_t rowId) const
{
    return m_table[columnId][rowId];
}

size_t Image::height() const
{
    if (m_table.empty())
        return 0;
    return m_table[0].size();
}

size_t Image::width() const
{
    return m_table.size();
}