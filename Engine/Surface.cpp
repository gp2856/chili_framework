#include "ChiliWin.h"
#include "Surface.h"
#include <fstream>
#include <cassert>
#include <cmath>

surface::surface(const std::string & filename)
{
	std::ifstream file(filename, std::ios::binary);

	// Read header info into windows structs
	BITMAPFILEHEADER bm_file_header;
	BITMAPINFOHEADER bm_info_header;
	file.read(reinterpret_cast<char*>(&bm_file_header), sizeof(bm_file_header));
	file.read(reinterpret_cast<char*>(&bm_info_header), sizeof(bm_info_header));

	// Assure we are only working with simple 8bpp RGB bitmap files
	assert(bm_info_header.biBitCount == 24 || bm_info_header.biBitCount == 32);
	assert(bm_info_header.biCompression == BI_RGB);

	width_ = bm_info_header.biWidth;
	height_ = bm_info_header.biHeight;

	pixels_ = new Color[width_*height_];

	// Seek to raw data
	file.seekg(bm_file_header.bfOffBits);

	if (bm_info_header.biBitCount == 24)
	{
		// Each row is padded to a multiple of 4 bytes
		// In a 24bpp bitmap, each pixel is represented by a 3-byte triplet
		// containing the values for Blue, Green, and Red respectively
		const int padding = (4 - (width_ * 3) % 4) % 4;

		// Check if height is negative/positive to detect if we are loading a top-down or bottom-up DIB
		if (height_ > 0)
		{
			// bottom-up
			for (int y = height_ - 1; y > 0; y--)
			{
				for (int x = 0; x < width_; x++)
				{
					// Precalculate RGB since bitmap color order is BGR
					const unsigned char b = file.get();
					const unsigned char g = file.get();
					const unsigned char r = file.get();
					// Create a new color object out of the next 3 bytes
					put_pixel(x, y, { Color(r,g,b) });
				}
				// Move forward padding bytes relative to current position
				file.seekg(padding, std::ios::cur);
			}
		}
		else
		{
			// top-down
			for (int y = 0; y < height_; y++)
			{
				for (int x = 0; x < width_; x++)
				{
					// Precalculate RGB since bitmap color order is BGR
					const unsigned char b = file.get();
					const unsigned char g = file.get();
					const unsigned char r = file.get();
					// Create a new color object out of the next 3 bytes
					put_pixel(x, y, { Color(r,g,b) });
				}
				// Move forward padding bytes relative to current position
				file.seekg(padding, std::ios::cur);
			}
		}
	}
	else if(bm_info_header.biBitCount == 32)
	{
		// Each row is padded to a multiple of 4 bytes
		// In a 32bpp bitmap, each pixel is represented by a DWORD
		// The least significant 8 bits contain the blue value, followed by green and red.
		const int padding = (4 - (width_ * 4) % 4) % 4;

		// Check if height is negative/positive to detect if we are loading a top-down or bottom-up DIB
		if (height_ > 0)
		{
			// bottom-up
			for (int y = height_ - 1; y > 0; y--)
			{
				for (int x = 0; x < width_; x++)
				{
					// Precalculate RGB since bitmap color order is BGR
					const unsigned char b = file.get();
					const unsigned char g = file.get();
					const unsigned char r = file.get();
					// Create a new color object out of the next 3 bytes
					put_pixel(x, y, { Color(r,g,b) });
					// In 32bpp, the high byte of each pixel is not used, so we need to seek past it
					file.seekg(sizeof(char), std::ios::cur);
				}
				// Move forward padding bytes relative to current position
				file.seekg(padding, std::ios::cur);
			}
		}
		else
		{
			// top-down
			for (int y = 0; y < height_; y++)
			{
				for (int x = 0; x < width_; x++)
				{
					// Precalculate RGB since bitmap color order is BGR
					const unsigned char b = file.get();
					const unsigned char g = file.get();
					const unsigned char r = file.get();
					// Create a new color object out of the next 3 bytes
					put_pixel(x, y, { Color(r,g,b) });
				}
				// Move forward padding bytes relative to current position
				file.seekg(padding, std::ios::cur);
			}
		}
	}

}

surface::surface(const int width, const int height)
	:
width_(width),
height_(height)
{
	pixels_ = new Color[width*height];
}

surface::surface(const surface & source)
	:
surface(source.get_width(), source.get_height())
{
	const int n_pixels = width_ * height_;
	for(int i = 0; i < n_pixels; i++)
	{
		pixels_[i] = source.get_pixels()[i];
	}
}

surface & surface::operator=(const surface & source)

{
	assert(source.get_pixels() != pixels_);
	width_ = source.get_width();
	height_ = source.get_height();

	delete[] pixels_;
	pixels_ = new Color[width_ * height_];

	const int n_pixels = width_ * height_;
	for (int i = 0; i < n_pixels; i++)
	{
		pixels_[i] = source.get_pixels()[i];
	}

	return *this;

}

surface::~surface()
{
	delete[] pixels_;
	pixels_ = nullptr;
}

int surface::get_width() const
{
	return width_;
}

int surface::get_height() const
{
	return height_;
}

Color surface::get_pixel(const int x, const int y) const
{
	assert(0 <= x && x <= width_);
	assert(0 <= y && y <= height_);
	return pixels_[x + y * width_];
}

Color * surface::get_pixels() const
{
	return pixels_;
}

void surface::put_pixel(const int x, const int y, const Color& c)
{
	assert(0 <= x && x <= width_);
	assert(0 <= y && y <= height_);
	pixels_[x + y * width_] = c;
}
