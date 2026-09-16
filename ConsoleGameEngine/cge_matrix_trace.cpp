/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

//#include "olcConsoleGameEngine.h"
#include "olcConsoleGameEngineGL.h"

#define WIDTH 80
#define HEIGHT 32

CHAR_INFO screen_buffer[WIDTH * HEIGHT];

wchar_t phone[22] = L" (3 1 3)5 5 5 1 2 3 4";

int found[10];
int check_number = 30;

int bright[256];

class OneLoneCoder_Sample : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Sample()
	{
		m_sAppName = L"Matrix Trace";
	}

	
protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		int x;
		
		srand(GetTickCount());
		
		for(x = 0; x < 10; x++)
		{
			int idx = x * 2 + 2;
			phone[idx] = L'0' + (rand() % 10);
		}
		
		phone[8]  = L'5';
		phone[10] = L'5';
		phone[12] = L'5';
		
		for(x = 0; x < 256; x++)
			bright[x] = rand();

		return true;
	}
	
	void set_pixel(int x, int y, wchar_t wc, WORD col)
	{
		screen_buffer[y * WIDTH + x].Char.UnicodeChar = wc;
		screen_buffer[y * WIDTH + x].Attributes = col;
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
			
			for(y = 3; y < HEIGHT; y++)
			for(x = 2; x < WIDTH; x += 2)
			{
				wchar_t number;
				int idx = (x/2 - 1)%10;

				if(found[idx]) continue;
				
				number = L'0' + rand() % 10;
				set_pixel(x, y, number, FG_DARK_GREEN);
			}
			
			for(x = 2; x < WIDTH; x += 2)
			{
				int idx = (x/2 - 1)%10;
				if(found[idx]) continue;
				
				y = (bright[x] % (HEIGHT - 3)) + 3;
				screen_buffer[y * WIDTH + x].Attributes = FG_GREEN;
				
				y = ((bright[x]-1) % (HEIGHT - 3)) + 3;
				screen_buffer[y * WIDTH + x].Attributes = FG_DARK_GREEN;
				
				bright[x]++;
			}
			
			if(check_number)
			{
				check_number--;
			}
			else
			{
				int num = rand() % 10;

				if(!found[num])
				{
					found[num] = 1;
					check_number = 10 + (rand() & 0x3F);
					
					x = num * 2 + 2;
					
					for(y = 3; y < HEIGHT; y++)
					for(x = 2; x < WIDTH; x += 2)
					{
						int idx = (x/2 - 1)%10;
						
						if(idx == num)
							set_pixel(x, y, L' ', FG_BLACK);
					}
					
					switch(num)
					{
						case 0: set_pixel( 1, 1, phone[ 1], FG_DARK_GREEN);
								set_pixel( 2, 1, phone[ 2], FG_DARK_GREEN); break;
						case 1: set_pixel( 4, 1, phone[ 4], FG_DARK_GREEN); break;
						case 2: set_pixel( 6, 1, phone[ 6], FG_DARK_GREEN);
								set_pixel( 7, 1, phone[ 7], FG_DARK_GREEN); break;
						case 3: set_pixel( 8, 1, phone[ 8], FG_DARK_GREEN); break;
						case 4: set_pixel(10, 1, phone[10], FG_DARK_GREEN); break;
						case 5: set_pixel(12, 1, phone[12], FG_DARK_GREEN); break;
						case 6: set_pixel(14, 1, phone[14], FG_DARK_GREEN); break;
						case 7: set_pixel(16, 1, phone[16], FG_DARK_GREEN); break;
						case 8: set_pixel(18, 1, phone[18], FG_DARK_GREEN); break;
						case 9: set_pixel(20, 1, phone[20], FG_DARK_GREEN); break;
					}
				}
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
