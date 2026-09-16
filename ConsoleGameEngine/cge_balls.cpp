/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

//#include "olcConsoleGameEngine.h"
#include "olcConsoleGameEngineGL.h"
#include <algorithm>

#ifndef BALLS_2D
#define BALLS_2D 1
#endif

#define TOTAL_BALLS 100
#define WIDTH 320
#define HEIGHT 240

struct vec3 {
	float x, y, z;
	
	float len() { return sqrt(x*x+y*y+z*z); }
	float dot(vec3 &v) { return (x*v.x+y*v.y+z*v.z); }
	
	inline vec3 operator+(vec3 &v) { return vec3(x+v.x, y+v.y, z+v.z); }
	inline vec3 operator-(vec3 &v) { return vec3(x-v.x, y-v.y, z-v.z); }

	inline vec3& operator+=(vec3 &v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
	inline vec3& operator-=(vec3 &v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
	
	inline vec3 operator+(float k) { return vec3(x+k, y+k, z+k); }
	inline vec3 operator-(float k) { return vec3(x-k, y-k, z-k); }
	inline vec3 operator*(float k) { return vec3(x*k, y*k, z*k); }
	inline vec3 operator/(float k) { return vec3(x/k, y/k, z/k); }

	inline vec3& operator+=(float k) { x+=k; y+=k; z+=k; return *this; }
	inline vec3& operator-=(float k) { x-=k; y-=k; z-=k; return *this; }
	inline vec3& operator*=(float k) { x*=k; y*=k; z*=k; return *this; }
	inline vec3& operator/=(float k) { x/=k; y/=k; z/=k; return *this; }
	
	vec3() : x(0), y(0), z(0) {}
	vec3(float a, float b, float c) : x(a), y(b), z(c) {}
};

struct sball {
	struct vec3 pos, vel;
	float radius;
	int color;

	float edge_r, edge_b;
};

bool ball_sort_z(struct sball &i, struct sball &j) { return (i.pos.z > j.pos.z); }

struct sball ball[TOTAL_BALLS];
int ball_idx[TOTAL_BALLS];
int ball_pair_checked[TOTAL_BALLS][TOTAL_BALLS];

void edge_collision(float *p, float *v, float radius, float max)
{
	max -= radius;
	
	if(*p >= max) 	{
		*p  = 2.0f * max - *p;
		*v *= -1;
	} else if(*p <= radius) {
		*p  = 2.0f * radius - *p;
		*v *= -1;
	}
}

float signf(float f) 
{
	return f < 0 ? -1.0f : 1.0f;
}

class OneLoneCoder_Sample : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Sample()
	{
		m_sAppName = L"Balls";
	}

	
protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		srand(GetTickCount());
		
		for(int i = 0; i < TOTAL_BALLS; i++) {
			ball[i].pos.x = (rand()%(WIDTH-20)) + 10.0f;
			ball[i].pos.y = (rand()%(HEIGHT-20)) + 10.0f;
			ball[i].pos.z = BALLS_2D ? 31.0f : (rand()%50) + 10.0f;

			ball[i].vel.x = (rand()&0x3F) * 1.0f - 32.0f;
			ball[i].vel.y = (rand()&0x3F) * 1.0f - 32.0f;
			ball[i].vel.z = BALLS_2D ? 0 : (rand()&0x3F) * 1.0f - 32.0f;
			
			ball[i].radius = (rand()&7) + 4.0f;
			ball[i].color  = (rand()%6) + 1;
			ball[i].color |= ball[i].color << 4;
			
			ball[i].edge_r = WIDTH - ball[i].radius;
			ball[i].edge_b = HEIGHT - ball[i].radius;
		}
		
		return true;
	}
	
	void physics_update(float dt)
	{
		memset(ball_pair_checked, 0, TOTAL_BALLS * TOTAL_BALLS * 4);
		
		for(int i = 0; i < TOTAL_BALLS; i++) {
			vec3 &p1 = ball[i].pos;
			vec3 &v1 = ball[i].vel;
			
			p1 += v1 * dt;
			
			edge_collision(&p1.x, &v1.x, ball[i].radius, (float)WIDTH);
			edge_collision(&p1.y, &v1.y, ball[i].radius, (float)HEIGHT);
			edge_collision(&p1.z, &v1.z, ball[i].radius,  70.0f);
			
			ball_pair_checked[i][i] = 1;

			for(int j = 0; j < TOTAL_BALLS; j++) {
				if(ball_pair_checked[i][j]) continue;
				
				vec3 &p2 = ball[j].pos;
				vec3 &v2 = ball[j].vel;
				
				float pdist = ball[i].radius + ball[j].radius;
				
				vec3 dir = p2 - p1;
				float dirlen = dir.len();
				
				if(dirlen <= pdist) {
					dir /= dirlen;
					
					float diff = (pdist - dirlen) * 0.5f;
					float v1k = v1.dot(dir);
					float v2k = v2.dot(dir);
					
					p1 += dir * (-diff);
					p2 += dir * (diff);
					
					float m1 = ball[i].radius;
					float m2 = ball[j].radius;
					
					if(BALLS_2D) {
						m1 *= m1;
						m2 *= m2;
					} else {
						m1 *= m1 * m1;
						m2 *= m2 * m2;
					}

					float v1f = (v1k * (m1 - m2) + 2.0f * m2 * v2k) / (m1 + m2);
					float v2f = (v2k * (m2 - m1) + 2.0f * m1 * v1k) / (m1 + m2);

					v1 += dir * (v1f - v1k);
					v2 += dir * (v2f - v2k);
				}

				ball_pair_checked[i][j] = 1;
			}
		}
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if(m_keys[VK_ESCAPE].bPressed) return false;
		
		Fill(0, 0, WIDTH, HEIGHT, 0x2588, 0x00);
		
		static float time_accum;
		
		while(time_accum < fElapsedTime) {
			physics_update(1.0f/120.0f);
			time_accum += 1.0f/120.0f;
		}
		
		time_accum -= fElapsedTime;
		
		if(!BALLS_2D)
		std::sort(ball, &ball[TOTAL_BALLS], ball_sort_z);
	
		for(int i = 0; i < TOTAL_BALLS; i++) {
			wchar_t c   = PIXEL_SOLID;
			short color = ball[i].color;
			int size = (int)ball[i].radius;
			int posx = BALLS_2D ? (int)ball[i].pos.x : (int)((ball[i].pos.x - WIDTH*0.5f) / (1.0f + ball[i].pos.z * 0.01f) + WIDTH*0.5f);
			int posy = BALLS_2D ? (int)ball[i].pos.y : (int)((ball[i].pos.y - HEIGHT*0.5f) / (1.0f + ball[i].pos.z * 0.01f) + HEIGHT*0.5f);
			
			switch((int)(ball[i].pos.z * 0.1f)) {
				case 0: c = PIXEL_QUARTER;       color |= 0xF0; size += 3; break;
				case 1: c = PIXEL_HALF;          color |= 0xF0; size += 2; break;
				case 2: c = PIXEL_THREEQUARTERS; color |= 0xF0; size += 1; break;
				case 3: break;
				case 4: c = PIXEL_THREEQUARTERS; color &= 0x0F; size -= 1; break;
				case 5: c = PIXEL_HALF;          color &= 0x0F; size -= 2; break;
				case 6: c = PIXEL_QUARTER;       color &= 0x0F; size -= 3; break;
			}
			
			FillCircle(posx, posy, size, c, color | 0x88);
			DrawCircle(posx, posy, size, c, color);
		}
		
		return true;
	}
	
	// Called by olcConsoleGameEngine, optional
	virtual bool OnUserDestroy(float fElapsedTime)
	{
		return true;
	}
	
};

int main(void)
{
	OneLoneCoder_Sample game;
	
	if(game.ConstructConsole(WIDTH, HEIGHT, 2, 2)) {
		game.Start();
	}

	return 0;
}
