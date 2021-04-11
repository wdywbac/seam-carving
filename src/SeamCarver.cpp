#include <cmath>
#include "SeamCarver.h"

SeamCarver::SeamCarver(Image image)
        : m_image(std::move(image))
{}

const Image& SeamCarver::GetImage() const
{
    return m_image;
}

size_t SeamCarver::GetImageWidth() const
{
    return m_image.width();
}

size_t SeamCarver::GetImageHeight() const
{
    return m_image.height();
}

double SeamCarver::GetPixelEnergy(size_t columnId, size_t rowId) const
{
    std::vector<std::vector<double>> energy = CalculatePixelEnergy();
    return energy[columnId][rowId];
}

std::vector<std::vector<double>> SeamCarver::CalculatePixelEnergy() const
{
    size_t height = GetImageHeight();
    size_t width = GetImageWidth();

    int abscissa_energy = 0;
    int ordinate_energy = 0;
    double energy = 0;


    std::vector<std::vector<double>> energy_table(width);
    for (size_t i = 0; i < width; i++)
    {
        energy_table[i].resize(height);
    }

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            abscissa_energy = GetEnergy(m_image.m_table[(x + 1) % width][y],
                    m_image.m_table[(x - 1 + width) % width][y]);
            ordinate_energy = GetEnergy(m_image.m_table[x][(y + 1) % height],
                    m_image.m_table[x][(y - 1 + height) % height]);

            energy = abscissa_energy + ordinate_energy;
            energy_table[x][y] = sqrt(energy);
        }
    }

    return energy_table;
}

int SeamCarver::GetEnergy(Image::Pixel first, Image::Pixel second) const
{
    int red = first.m_red - second.m_red;
    int blue = first.m_blue - second.m_blue;
    int green = first.m_green - second.m_green;

    return red * red + green * green + blue * blue;
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const
{
    std::vector<std::vector<double>> energy = CalculatePixelEnergy();

    Seam seam;
    size_t height = GetImageHeight();
    size_t width = GetImageWidth();
    seam.resize(height);

    std::vector<std::vector<double>> dp(width, std::vector<double> (height, 0));
    for (size_t columnId = 0; columnId < width; columnId++)
    {
        dp[columnId][0] = energy[columnId][0];
    }

    for (size_t rowId = 1; rowId < height; rowId++)
    {
        for (size_t columnId = 0; columnId < width; columnId++)
        {
            dp[columnId][rowId] = energy[columnId][rowId];
            if (columnId == 0)
            {
                dp[columnId][rowId] += std::min(dp[columnId][rowId - 1], dp[columnId + 1][rowId - 1]);
            }
            else if (columnId == width - 1)
            {
                dp[columnId][rowId] += std::min(dp[columnId][rowId - 1], dp[columnId - 1][rowId - 1]);
            }
            else
            {
                dp[columnId][rowId] += std::min(std::min(dp[columnId - 1][rowId - 1], dp[columnId][rowId - 1]), dp[columnId + 1][rowId - 1]);
            }
        }
    }

    double min_value = std::numeric_limits<double>::max();
    size_t min_ordinate = 0;
    for (size_t columnId = 0; columnId < width; columnId++)
    {
        if (dp[columnId][height - 1] < min_value)
        {
            min_value = dp[columnId][height - 1];
            min_ordinate = columnId;
        }
    }

    seam[height - 1] = min_ordinate;

    size_t rowId = height - 1;
    while (rowId > 0) {
        rowId--;
        if (min_ordinate == 0)
        {
            if (dp[min_ordinate][rowId] <= dp[min_ordinate + 1][rowId])
            {
                seam[rowId] = min_ordinate;
            }
            else
            {
                seam[rowId] = min_ordinate + 1;
            }
        }
        else if (min_ordinate == width - 1)
        {
            if (dp[min_ordinate][rowId] <= dp[min_ordinate - 1][rowId])
            {
                seam[rowId] = min_ordinate;
            }
            else
            {
                seam[rowId] = min_ordinate - 1;
            }
        }
        else
        {
            if (dp[min_ordinate - 1][rowId] <= dp[min_ordinate][rowId] && dp[min_ordinate - 1][rowId] <= dp[min_ordinate + 1][rowId])
            {
                seam[rowId] = min_ordinate - 1;
            }
            else if (dp[min_ordinate][rowId] <= dp[min_ordinate - 1][rowId] && dp[min_ordinate][rowId] <= dp[min_ordinate + 1][rowId])
            {
                seam[rowId] = min_ordinate;
            }
            else
            {
                seam[rowId] = min_ordinate + 1;
            }
        }
        min_ordinate = seam[rowId];
    }

    return seam;
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const
{
    std::vector<std::vector<double>> energy = CalculatePixelEnergy();

    Seam seam;
    size_t height = GetImageHeight();
    size_t width = GetImageWidth();
    seam.resize(width);

    std::vector<std::vector<double>> dp(width, std::vector<double> (height, 0));
    for (size_t rowId = 0; rowId < height; rowId++)
    {
        dp[0][rowId] = energy[0][rowId];
    }

    for (size_t columnId = 1; columnId < width; columnId++)
    {
        for (size_t rowId = 0; rowId < height; rowId++)
        {
            dp[columnId][rowId] = energy[columnId][rowId];
            if (rowId == 0)
            {
                dp[columnId][rowId] += std::min(dp[columnId - 1][rowId], dp[columnId - 1][rowId + 1]);
            }
            else if (rowId == height - 1)
            {
                dp[columnId][rowId] += std::min(dp[columnId - 1][rowId], dp[columnId - 1][rowId - 1]);
            }
            else
            {
                dp[columnId][rowId] += std::min(std::min(dp[columnId - 1][rowId - 1], dp[columnId - 1][rowId]), dp[columnId - 1][rowId + 1]);
            }
        }
    }

    double min_value = std::numeric_limits<double>::max();
    size_t min_abscissa = 0;
    for (size_t rowId = 0; rowId < height; rowId++)
    {
        if (dp[width - 1][rowId] < min_value)
        {
            min_value = dp[width - 1][rowId];
            min_abscissa = rowId;
        }
    }

    seam[width - 1] = min_abscissa;

    size_t columnId = width - 1;
    while (columnId > 0) {
        columnId--;
        if (min_abscissa == 0)
        {
            if (dp[columnId][min_abscissa] <= dp[columnId][min_abscissa + 1])
            {
                seam[columnId] = min_abscissa;
            }
            else
            {
                seam[columnId] = min_abscissa + 1;
            }
        }
        else if (min_abscissa == width - 1)
        {
            if (dp[columnId][min_abscissa] <= dp[columnId][min_abscissa - 1])
            {
                seam[columnId] = min_abscissa;
            }
            else
            {
                seam[columnId] = min_abscissa - 1;
            }
        }
        else
        {
            if (dp[columnId][min_abscissa - 1] <= dp[columnId][min_abscissa] && dp[columnId][min_abscissa - 1] <= dp[columnId][min_abscissa + 1])
            {
                seam[columnId] = min_abscissa - 1;
            }
            else if (dp[columnId][min_abscissa] <= dp[columnId][min_abscissa - 1] && dp[columnId][min_abscissa] <= dp[columnId][min_abscissa + 1])
            {
                seam[columnId] = min_abscissa;
            }
            else
            {
                seam[columnId] = min_abscissa + 1;
            }
        }
        min_abscissa = seam[columnId];
    }

    return seam;
}

void SeamCarver::RemoveHorizontalSeam(const Seam& seam)
{
    size_t height = GetImageHeight();
    size_t width = GetImageWidth();

    for (size_t columnId = 0; columnId < width; columnId++)
    {
        for (size_t rowId = 0; rowId < height - 1; rowId++)
        {
            if (seam[columnId] <= rowId)
            {
                m_image.m_table[columnId][rowId] = m_image.m_table[columnId][rowId + 1];
            }
        }
    }

    for (size_t i = 0; i < width; i++)
    {
        m_image.m_table[i].resize(height - 1);
    }
}

void SeamCarver::RemoveVerticalSeam(const Seam& seam)
{
    size_t height = GetImageHeight();
    size_t width = GetImageWidth();

    for (size_t rowId = 0; rowId < height; rowId++)
    {
        for (size_t columnId = 0; columnId < width - 1; columnId++)
        {
            if (seam[rowId] <= columnId)
            {
                m_image.m_table[columnId][rowId] = m_image.m_table[columnId + 1][rowId];
            }
        }
    }

    m_image.m_table.resize(width - 1);

}