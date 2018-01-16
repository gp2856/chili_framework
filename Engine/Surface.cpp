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

	const bool is_32_b = bm_info_header.biBitCount == 32;
	width_ = bm_info_header.biWidth;

	int y_start;
	int y_end;
	int dy;
	if (bm_info_header.biHeight < 0)
	{
		height_ = -bm_info_header.biHeight;
		y_start = 0;
		y_end = height_;
		dy = 1;
	}
	else
	{
		height_ = bm_info_header.biHeight;
		y_start = height_ - 1;
		y_end = -1;
		dy = -1;
	}

	pixels_ = new Color[width_*height_];

	// Seek to raw data
	file.seekg(bm_file_header.bfOffBits);

	
	// Padding is only with respect to 24bpp
	// In a 24bpp bitmap, each pixel is represented by a 3-byte triplet
	// containing the values for Blue, Green, and Red respectively
	const int padding = (4 - (width_ * 3) % 4) % 4;

		
	for (int y = y_start; y != y_end; y += dy)
	{
		for (int x = 0; x < width_; x++)
		{
			// Precalculate RGB since bitmap color order is BGR
			const unsigned char b = file.get();
			const unsigned char g = file.get();
			const unsigned char r = file.get();
			// Create a new color object out of the next 3 bytes
			put_pixel(x, y, { Color(r,g,b) });

			if(is_32_b)
			{
				file.seekg(1, std::ios::cur);
			}
		}
		// Move forward padding bytes relative to current position
		if (!is_32_b)
		{
			file.seekg(padding, std::ios::cur);
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

RectI surface::get_rect() const
{
	return { 0,width_,0,height_ };
}
