/*
 * MIT License
 *
 * Copyright (c) 2020 Vladislav Markelov 
 * https://twitter.com/MeetMarkelov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GLOBALMERCATOR_HPP
#define GLOBALMERCATOR_HPP

#include <cmath>

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#define TileSize 256.
#define InitRes (2. * M_PI * 6378137. / TileSize)
#define OriginShift (2. * M_PI * 6378137. / 2.0)

class GlobalMercator
{
public:

	static inline void PixelsToTile(const double px, const double py, int& tx, int &ty)
	{
		tx = ceil(px / TileSize) - 1.;
		ty = ceil(py / TileSize) - 1.;
	}

	static inline void WGS84ToMeters(const double Lat, const double Lon, double &mx, double &my)
	{
		mx = Lon * OriginShift / 180.;
		my = log(tan((90. + Lat) * M_PI / 360.)) / (M_PI / 180.);
		
		my = my * OriginShift / 180.;
	}

	static inline double Resolution(const int Zoom)
	{
		return InitRes / pow(2., Zoom);
	}

	static inline void MetersToPixels(const double mx, const double my, double &px, double &py, const int Zoom)
	{
		const double res = Resolution( Zoom );
		px = (mx + OriginShift) / res;
		py = (my + OriginShift) / res;
	}

	static inline void MetersToWGS84(const double mx, const double my, double &Lat, double &Lon)
	{
		Lon = (mx / OriginShift) * 180.;
		Lat = (my / OriginShift) * 180.;

		Lat = 180. / M_PI * (2. * atan( exp( Lat * M_PI / 180.)) - M_PI / 2.);
	}

	static inline void PixelsToMeters(const double px, const double py, double &mx, double &my, const int Zoom)
	{
		const double res = Resolution( Zoom );
		mx = px * res - OriginShift;
		my = py * res - OriginShift;
	}

	static inline void TileBounds(const int tx, const int ty, double &MinX, double &MinY, double &MaxX, double &MaxY, const int Zoom)
	{
		PixelsToMeters(double(tx) * TileSize,		double(ty) * TileSize,		MinX, MinY, Zoom);
		PixelsToMeters(double(tx + 1) * TileSize,	double(ty + 1) * TileSize,	MaxX, MaxY, Zoom);
	}

	static inline void WGS84Bounds(const int tx, const int ty, double &MinLat, double &MinLon, double &MaxLat, double &MaxLon, const int Zoom)
	{
		double MinX, MinY, MaxX, MaxY;
		TileBounds( tx, ty, MinX, MinY, MaxX, MaxY, Zoom);
		MetersToWGS84(MinX, MinY, MinLat, MinLon);
		MetersToWGS84(MaxX, MaxY, MaxLat, MaxLon);
	}

	static inline void TileTMSToGoogle(const int tx, const int ty, int &GoogleX, int &GoogleY, const int Zoom)
	{
		GoogleX = tx;
		GoogleY = (pow(2.,Zoom) - 1.) - ty;
	}

	static inline void WGS84ToTile(const double Lat, const double Lon, int& tx, int &ty, const int Zoom)
	{
		double mx, my;
		double px, py;
		WGS84ToMeters(Lat,Lon,mx,my);
		MetersToPixels(mx,my,px,py,Zoom);
		PixelsToTile(px, py, tx, ty);
	}
};

#endif
