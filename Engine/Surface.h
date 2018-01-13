#pragma once
#include <string>
#include "Colors.h"

class surface
{
public:
	surface(const int width, const int height);
	surface(const surface& source);
	surface& operator=(const surface& source);
	~surface();

	int get_width() const;
	int get_height() const;
	Color get_pixel(int x, int y) const;
	Color *get_pixels() const;
	void put_pixel(const int x, const int y, const Color c);
private:
	Color * pixels_ = nullptr;
	int width_;
	int height_;
};