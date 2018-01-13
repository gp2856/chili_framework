#include "Surface.h"

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
	return pixels_[x + y * width_];
}

Color * surface::get_pixels() const
{
	return pixels_;
}

void surface::put_pixel(const int x, const int y, const Color c)
{
	pixels_[x + y * width_] = c;
}
