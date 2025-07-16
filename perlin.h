#ifndef PERLIN_H
#define PERLIN_H
#include<cmath>

struct vector2f{
  float x;
  float y;

  vector2f(float _x, float _y):x (_x),y(_y){}



  };



namespace perlin {

	static float dot(vector2f a, vector2f b){
		return a.x * b.x + a.y * b.y;
	}

 static  const int permutation[256] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
   };

	static vector2f getConstVector(int v) {
		switch (v % 8) {
			case 0: return vector2f(1, 0);
			case 1: return vector2f(-1, 0);
			case 2: return vector2f(0, 1);
			case 3: return vector2f(0, -1);
			case 4: return vector2f(1, 1);
			case 5: return vector2f(-1, 1);
			case 6: return vector2f(-1, -1);
			case 7: return vector2f(1, -1);
			default: return vector2f(0, 0);

		}
	}

   static float fade(float t)
    {
    	return 6*t*t*t*t*t - 15*t*t*t*t + 10 * t*t*t;
    }

  	static float lerp(float t, float a , float b){
		return a + t * (b - a);
      }


   static float perlin2D(float x, float y) {

     int X = (int)floor(x)&255;
     int Y = (int)floor(y)&255;

     float xf = x - floor(x);
     float yf = y - floor(y);

	vector2f leftBottom = vector2f(xf, yf);
    vector2f rightBottom = vector2f(xf-1.0f, yf);
    vector2f leftTop = vector2f(xf, yf-1.0f);
    vector2f rightTop = vector2f(xf-1.0f, yf-1.0f);

    int valueLeftBottom = permutation[permutation[X] + Y];
    int valueRightBottom = permutation[permutation[X+1] + Y];
  	int valueLeftTop = permutation[permutation[X] + Y+1];
    int valueRightTop = permutation[permutation[X+1] + Y+1];

    float dotLeftBottom = dot(leftBottom, getConstVector(valueLeftBottom));
    float dotRightBottom = dot(rightBottom, getConstVector(valueRightBottom));
    float dotLeftTop = dot(leftTop, getConstVector(valueLeftTop));
    float dotRightTop = dot(rightTop, getConstVector(valueRightTop));

	float u = fade(xf);
    float v = fade(yf);

    return lerp(u,
               lerp(v, dotLeftBottom, dotLeftTop),
               lerp(v, dotRightBottom, dotRightTop));

   }



	static float fractalBrownianMotion2D( float x , float y) {
  		const int octaves = 6;
  		float frequency = 0.8f;
  		float amplitude = 2.5f;
  		float lacunarity = 0.5f;
  		float gain = 0.5f;

  		float sum = 0.0f;

  		for (int i = 0; i < octaves; ++i) {
  			sum += amplitude * perlin2D(x * frequency, y * frequency);
  			frequency *= lacunarity;
  			amplitude *= gain;
  		}

  		return sum;
  	}


};



#endif //PERLIN_H
