/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

//#include "olcConsoleGameEngine.h"
#include "olcConsoleGameEngineGL.h"

#define NUM_RAND_CHARS 20

#define WIDTH 80
#define HEIGHT 30

CHAR_INFO screen_buffer[WIDTH * HEIGHT];

wchar_t wchar_palette[64] = 
{
	L'"', L'*', L'.', L'+', L'<', L'>', L':', L'_',
	L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9', L'0'}; 

struct
{
	unsigned int pos;
	unsigned int white;
} column[512], rand_char[NUM_RAND_CHARS];

class OneLoneCoder_Sample : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Sample()
	{
		m_sAppName = L"Matrix Code";
	}

protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		int x = 0;
	
		for(; x < 256; x++)
		{
			column[x].pos   = rand() % HEIGHT;
			column[x].white = rand() & 1;
		}
		
		for(; x < 512; x++)
		{
			column[x].pos   = column[x - 256].pos - 2 - rand() % (HEIGHT/2);
		}
		
		for(x = 0; x < NUM_RAND_CHARS; x++)
		{	
			rand_char[x].pos = rand() % (WIDTH * HEIGHT);
			rand_char[x].white = 5;
		}
		
		for(x = 18; x < 64; x++) // use katakana
		{
			wchar_palette[x] = L'A' + x;
		}

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
	static float accum;

	int x;
	int y;
	
	accum += fElapsedTime;
	if(accum >= 0.05f)
	{
		accum -= 0.05f;
		
		for(x = 0; x < WIDTH; x++)
		{
			wchar_t pix = wchar_palette[rand() & 0x3F];

			y = column[x].pos % HEIGHT;
			screen_buffer[y * WIDTH + x].Char.UnicodeChar = pix;
			screen_buffer[y * WIDTH + x].Attributes = column[x].white? FG_WHITE : FG_GREEN;
			
			y = y > 0 ? y - 1 : HEIGHT - 1;
			screen_buffer[y * WIDTH + x].Attributes = FG_GREEN;
			
			column[x].pos++;
		}
		
		for(x = 0; x < WIDTH; x++)
		{
			y = column[256 + x].pos % HEIGHT;
			screen_buffer[y * WIDTH + x].Attributes = FG_DARK_GREEN;
			
			y = y > 0 ? y - 1 : HEIGHT - 1;
			screen_buffer[y * WIDTH + x].Attributes = FG_BLACK;
			
			column[256 + x].pos++;
		}
		
		x = 50;

		while(x--)
		{
			y = rand() % (WIDTH * HEIGHT);
			screen_buffer[y].Char.UnicodeChar = wchar_palette[rand() & 0x3F];
		}
		
		for(x = 0; x < NUM_RAND_CHARS; x++)
		{
			if(rand_char[x].white&1)
				screen_buffer[rand_char[x].pos].Char.UnicodeChar = wchar_palette[rand() & 0x3F];
			
			if(rand_char[x].white > 4) 
			{
				screen_buffer[rand_char[x].pos].Attributes = FG_GREEN;
			}
			else if(rand_char[x].white > 0) 
			{
				screen_buffer[rand_char[x].pos].Attributes = FG_DARK_GREEN;
			}
			else
			{
				screen_buffer[rand_char[x].pos].Attributes = FG_BLACK;

				rand_char[x].pos = rand() % (WIDTH * HEIGHT);
				rand_char[x].white = 10;
			}
			
			rand_char[x].white--;
		}
		
		for(y = 0; y < HEIGHT; y++)
		for(x = 0; x < WIDTH; x++)
		{
			wchar_t wc = screen_buffer[y * WIDTH + x].Char.UnicodeChar;
			WORD col = screen_buffer[y * WIDTH + x].Attributes;
			Draw(x, y, wc, col);
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

	if(game.ConstructConsole(WIDTH, HEIGHT, 8, 16))
	{
		game.Start();
	}

	return 0;
}
