#include "melody.hpp"
#include <cmath>

using namespace melody;

void Matrix::identity() {
	a = 1;
	b = 0;
	c = 0;
	d = 1;
	tx = 0;
	ty = 0;
}

void Matrix::translate(float dx, float dy) {
	tx += dx;
	ty += dy;
}

void Matrix::scale(float sx, float sy) {
	a *= sx;
	b *= sy;
	c *= sx;
	d *= sy;
	tx *= sx;
	ty *= sy;
}

void Matrix::rotate(float theta) {
	float _cos = std::cos(theta);
	float _sin = std::sin(theta);

	float a1 = a * _cos - b * _sin;
	b = a * _sin + b * _cos;
	a = a1;

	float c1 = c * _cos - d * _sin;
	d = c * _sin + d * _cos;
	c = c1;

	float tx1 = tx * _cos - ty * _sin;
	ty = tx * _sin + ty * _cos;
	tx = tx1;
}