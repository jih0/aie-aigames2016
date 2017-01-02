#include "aiUtilities.h"

#include <math.h>

// returns true if the ray intersects the circle
bool rayCircleIntersection(float px, float py,	// ray start
	float dx, float dy,	// ray direction
	float cx, float cy, float r,	// circle position and radius
	float& ix, float& iy,	// intersection point
	float* t) {	// distance along normalised ray direction to intersection

	// normalise direction
	float temp = dx * dx + dy * dy;
	if (temp == 0)
		return false;

	temp = sqrtf(temp);

	dx /= temp;
	dy /= temp;

	// get vector from line start to circle centre
	float ex = cx - px;
	float ey = cy - py;

	// get squared length of e
	float e2 = ex * ex + ey * ey;

	// get squared radius
	float r2 = r * r;

	// determine if starting inside circle
	if (e2 < r2) {
		// if inside then reverse test direction
		dx *= -1;
		dy *= -1;
	}

	// project sphere centre onto d to get edge of a triangle
	float a = ex * dx + ey * dy;

	// squared edge length
	float a2 = a * a;

	// use pythagoras to determine intersection
	float f2 = r2 - e2 + a2;
	// f2 is now the amount of penetration into the circle squared

	// if negative then no penetration
	if (f2 < 0)
		return false;

	// calculate distance in direction d from p that the intersection occurs
	temp = a - sqrtf(f2);

	ix = dx * temp + px;
	iy = dy * temp + py;

	if (t != nullptr)
		*t = temp;

	return true;
}