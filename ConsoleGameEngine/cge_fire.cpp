/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

//#include "olcConsoleGameEngine.h"
#include "olcConsoleGameEngineGL.h"

enum
{
	PIXEL_100 = 0x2588,
	PIXEL_075 = 0x2593,
	PIXEL_050 = 0x2592,
	PIXEL_025 = 0x2591
};

#define WIDTH 80
#define HEIGHT 60

int fire_buffer[WIDTH * HEIGHT];

int palette[][2] =
{
	{PIXEL_100, FG_BLACK},
	{PIXEL_025, FG_RED | BG_BLACK},
	{PIXEL_050, FG_RED | BG_BLACK},
	{PIXEL_075, FG_RED | BG_BLACK},
	{PIXEL_100, FG_RED},
	{PIXEL_025, FG_YELLOW | BG_RED},
	{PIXEL_050, FG_YELLOW | BG_RED},
	{PIXEL_075, FG_YELLOW | BG_RED},
	{PIXEL_100, FG_YELLOW},
	{PIXEL_025, FG_WHITE | BG_YELLOW},
	{PIXEL_050, FG_WHITE | BG_YELLOW},
	{PIXEL_075, FG_WHITE | BG_YELLOW},
	{PIXEL_100, FG_WHITE}
};

class OneLoneCoder_Sample : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Sample()
	{
		m_sAppName = L"Fire!";
	}

	
protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{


		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		static float off;
		int x, y;
		
		off += fElapsedTime;
		if(off >= (1.0f/30.0f))
		{
			off -= 1.0f/30.0f;
			
			for(x = 0; x < WIDTH; x++)
				fire_buffer[(HEIGHT - 1) * WIDTH + x] = rand() & 0xFF;
			
			for(y = 0; y < (HEIGHT - 1); y++)
			for(x = 0; x < WIDTH; x++)
			{
				fire_buffer[y * WIDTH + x] = 
					((fire_buffer[((y + 1) % HEIGHT) * WIDTH + ((x - 1 + WIDTH) % WIDTH)]
					+ fire_buffer[((y + 1) % HEIGHT) * WIDTH + ((x) % WIDTH)]
					+ fire_buffer[((y + 1) % HEIGHT) * WIDTH + ((x + 1) % WIDTH)]
					+ fire_buffer[((y + 2) % HEIGHT) * WIDTH + ((x) % WIDTH)])
					* 32) / 132;
			}
			
			for(y = 0; y < HEIGHT; y++)
			for(x = 0; x < WIDTH; x++)
			{
				int col = fire_buffer[y * WIDTH + x] / 14;
				col = col < 0 ? 0 : col > 13 ? 13 : col;
				
				Draw(x, y, palette[col][0], palette[col][1]);
			}
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

	if(game.ConstructConsole(WIDTH, HEIGHT, 8, 8))
	{
		game.Start();
	}

	return 0;
}
