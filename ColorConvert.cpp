

#include <d3dx9.h>
#include <stdio.h>

typedef float		FLOAT;


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif



// color is r, g, b, a and limits [0,1]
// Hue(H), Saturation(S), Intensity(V), limits H: [0,360), S,V: [0,1]


void RGB_To_Gray(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	v = pIn[0] * 0.299F + pIn[1] * 0.587F + pIn[2] * 0.114F;
	pOut[0] = pOut[1] = pOut[2] = v;
}

void Gray_To_RGB(FLOAT* pOut, const FLOAT* pIn)
{
	memcpy(pOut, pIn, sizeof(float)*3);
}


//	RGB to CMYK
//	k = 1
//	k = min(1-r, 1-g, 1-b)
//	if 1== k then c =m = y = 0;
//	c = (1 - r - k)/(1-k)
//	m = (1 - g - k)/(1-k)
//	y = (1 - b - k)/(1-k)

//	CMYK to RGB
//	r = (1 - c) * (1 - k)
//	g = (1 - m) * (1 - k)
//	b = (1 - y) * (1 - k)


GetKfromCMY

k = min(1-r, 1-g, 1-b) =>
k = 1 - max(r,g,b)

k = 1 - max( (1-c)*(1-k), (1-m)*(1-k), (1-y)*(1-k))
k = 1 - max( (1-c), (1-m), (1-y)) * (1-k)

t = max( (1-c), (1-m), (1-y))
k = 1 - t * (1-k)





void RGB_To_CMYK(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	c=0;
	FLOAT	m=0;
	FLOAT	y=0;
	FLOAT	k=1.f;

	FLOAT	r=pIn[0];
	FLOAT	g=pIn[1];
	FLOAT	b=pIn[2];

	c = 1.F - r;
	m = 1.F - g;
	y = 1.F - b;

	k = min( c, m);
	k = min( k, y);

	if(1.f > k)
	{
		c = (c - k)/(1-k);
		m = (m - k)/(1-k);
		y = (y - k)/(1-k);
	}

	pOut[0] = c;
	pOut[1] = m;
	pOut[2] = y;
//	pOut[3] = k;
}


FLOAT GetKfromCMY(const FLOAT* pIn)
{
	FLOAT k = 1;

	if(0.F == pIn[0] && 0.F == pIn[1] && 0.F == pIn[2])
		k = 1.f;
	else
	{
		FLOAT c = -(1.F - pIn[0]);
		FLOAT m = -(1.F - pIn[1]);
		FLOAT y = -(1.F - pIn[2]);

		k = max( c, m);
		k = max( k, y);

		k = 1.F - 1.F/(2 - k);
	}

	return k;
}

void CMRK_To_RGB(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT	r=0;
	FLOAT	g=0;
	FLOAT	b=0;

	FLOAT	c=pIn[0];
	FLOAT	m=pIn[1];
	FLOAT	y=pIn[2];
	FLOAT	k=GetKfromCMY(pIn);

	c = 1.F - c;
	m = 1.F - c;
	r = 1.F - c;
	k = 1.F - k;

	r = c * k;
	g = c * k;
	b = c * k;

	pOut[0] = r;
	pOut[1] = g;
	pOut[2] = b;
}


// H: [0,360), S,V: [0,1]
void RGB_To_HSV(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT r = pIn[0];
	FLOAT g = pIn[1];
	FLOAT b = pIn[2];
	FLOAT h = 0;
	FLOAT s = 0;
	FLOAT v = 0;

	FLOAT fMax, fMin;

	fMax = max(r,g);
	fMax = max(fMax,b);

	fMin = min(r,g);
	fMin = min(fMin,b);

	v = fMax;
	s = (fMax != 0.F) ? (fMax-fMin)/fMax : 0.F;


	if(s != 0.F)
	{
		FLOAT delta = fMax - fMin;

		if(r == fMax)
			h = (g - b) / delta;
		else if( g == fMax)
			h = 2.F + (b - r) / delta;
		else if( b == fMax)
			h = 4.F + (r - g) / delta;

		h *= 60.F;

		if(h < 0.F)
			h += 360.F;
	}

	pOut[0] = h;
	pOut[1] = s;
	pOut[2] = v;
}


void HSV_To_RGB(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT r = 0;
	FLOAT g = 0;
	FLOAT b = 0;
	FLOAT h = pIn[0];
	FLOAT s = pIn[1];
	FLOAT v = pIn[2];


	// gray
	if(s == 0.F)
	{
		if(h == 0.F)
			r = g = b = v;
		else
			r = g = b = 0.F;
	}

	else
	{
		FLOAT f, p, q, t;
		int i;

		if(h == 360.F)
			h = 0.F;

		h /= 60.F;
		i = (int)h;
		f = h - i;
		p = v * (1.F - s);
		q = v * (1.F - (s * f));
		t = v * (1.F - (s * (1.F - f)));

		switch(i)
		{
		case 0: r = v; g = t; b = p; break;
		case 1: r = q; g = v; b = p; break;
		case 2: r = p; g = v; b = t; break;
		case 3: r = p; g = q; b = v; break;
		case 4: r = t; g = p; b = v; break;
		case 5: r = v; g = p; b = q; break;
		}
	}

	pOut[0] = r;
	pOut[1] = g;
	pOut[2] = b;
}


void RGB_To_HLS(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT r = pIn[0];
	FLOAT g = pIn[1];
	FLOAT b = pIn[2];
	FLOAT h = 0;
	FLOAT l = 0;
	FLOAT s = 0;

	FLOAT v;
	FLOAT m;
	FLOAT vm;
	FLOAT r2, g2, b2;

	v = max(r,g);
	v = max(v,b);
	m = min(r,g);
	m = min(m,b);

	if ((s = (m + v) / 2.F) <= 0.F)
		return;
	if ((l = vm = v - m) > 0.F)
		l /= (s <= 0.5F) ? (v + m ) : (2.F - v - m) ;
	else
		return;

	r2 = (v - r) / vm;
	g2 = (v - g) / vm;
	b2 = (v - b) / vm;

	if (r == v)
		h = (g == m ? 5.F + b2 : 1.F - g2);
	else if (g == v)
		h = (b == m ? 1.F + r2 : 3.F - b2);
	else
		h = (r == m ? 3.F + g2 : 5.F - r2);

	h *= 60.F;


	pOut[0] = h;
	pOut[1] = l;
	pOut[2] = s;
}


void HLS_To_RGB(FLOAT* pOut, const FLOAT* pIn)
{
	FLOAT r = 0;
	FLOAT g = 0;
	FLOAT b = 0;
	FLOAT h = pIn[0];
	FLOAT l = pIn[1];
	FLOAT s = pIn[2];

	FLOAT v;

	v = (s <= 0.5F) ? (s * (1.F + l)) : (s + l - s * l);
	if (v <= 0)
		r = g = b = 0.F;
	else {
		FLOAT m;
		FLOAT sv;
		int sextant;
		FLOAT fract, vsf, mid1, mid2;

		m = s + s - v;
		sv = (v - m ) / v;
		h /= 60.F;
		sextant = (int)h;
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;

		switch (sextant)
		{
		case 0: r = v;    g = mid1; b = m; break;
		case 1: r = mid2; g = v;    b = m; break;
		case 2: r = m;    g = v;    b = mid1; break;
		case 3: r = m;    g = mid2; b = v; break;
		case 4: r = mid1; g = m;    b = v; break;
		case 5: r = v;    g = m;    b = mid2; break;
		}
	}


	pOut[0] = r;
	pOut[1] = g;
	pOut[2] = b;
}



void main()
{
	D3DXCOLOR	dst;
	D3DXCOLOR src;

	src = D3DXCOLOR(0.60f, 0.2f, 0.3f, 1.f);

	RGB_To_HSV((float*)&dst, (float*)&src);
	printf( "RGB(0.60, 0.20, 0.30) -> HSV(%.2f, %.2f, %.2f)\n", dst.r, dst.g, dst.b);


	src = D3DXCOLOR(345.f, 0.67f, 0.6f, 1.f);
	HSV_To_RGB((float*)&dst, (float*)&src);
	printf( "HLS(345.0, 0.67, 0.60) -> RGB(%.2f, %.2f, %.2f)\n", dst.r, dst.g, dst.b);

	src = D3DXCOLOR(0.3f, 0.4f, 0.8f, 1.f);
	RGB_To_HLS((float*)&dst, (float*)&src);
	printf( "RGB(0.30, 0.40, 0.80) -> HSL(%.2f, %.2f, %.2f)\n", dst.r, dst.g, dst.b);

	src = D3DXCOLOR(228.f, 0.55f, 0.56f, 1.f);
	HLS_To_RGB((float*)&dst, (float*)&src);
	printf( "HSL(228.0, 0.55, 0.56) -> RGB(%.2f, %.2f, %.2f)\n", dst.r, dst.g, dst.b);
}

