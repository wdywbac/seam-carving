#pragma once
#include <iostream>

#include <vector>

struct Image
{
    struct Pixel
    {
        Pixel(int red, int green, int blue);

        Pixel();

        int m_red;
        int m_green;
        int m_blue;
    };

    size_t width() const;
    size_t height() const;

    Image(std::vector<std::vector<Pixel>> table);

    Pixel GetPixel(size_t columnId, size_t rowId) const;

    std::vector<std::vector<Pixel>> m_table;
};