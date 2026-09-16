/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

//#include "olcConsoleGameEngine.h"
#include "olcConsoleGameEngineGL.h"
#include "cge_sokomap.inc"

class OneLoneCoder_Sample : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Sample()
	{
		m_sAppName = L"Sokoban";
		
		wall   = olcSprite(2, 2);
		box    = olcSprite(2, 2);
		box2   = olcSprite(2, 2);
		socket = olcSprite(2, 2);
		player = olcSprite(2, 2);
		
		wall.SetGlyph(0, 0, 0x2588); wall.SetColour(0, 0, 0xDD);
		wall.SetGlyph(1, 0, 0x2588); wall.SetColour(1, 0, 0xDD);
		wall.SetGlyph(0, 1, 0x2588); wall.SetColour(0, 1, 0xDD);
		wall.SetGlyph(1, 1, 0x2588); wall.SetColour(1, 1, 0xDD);
		
		box.SetGlyph(0, 0, 0x2554); box.SetColour(0, 0, 0xBF);
		box.SetGlyph(1, 0, 0x2557); box.SetColour(1, 0, 0xBF);
		box.SetGlyph(0, 1, 0x255A); box.SetColour(0, 1, 0xBF);
		box.SetGlyph(1, 1, 0x255D); box.SetColour(1, 1, 0xBF);
		
		box2.SetGlyph(0, 0, 0x2554); box2.SetColour(0, 0, 0xB0);
		box2.SetGlyph(1, 0, 0x2557); box2.SetColour(1, 0, 0xB0);
		box2.SetGlyph(0, 1, 0x255A); box2.SetColour(0, 1, 0xB0);
		box2.SetGlyph(1, 1, 0x255D); box2.SetColour(1, 1, 0xB0);
		
		socket.SetGlyph(0, 0, 0x2F); socket.SetColour(0, 0, 0xB0);
		socket.SetGlyph(1, 0, 0x5C); socket.SetColour(1, 0, 0xB0);
		socket.SetGlyph(0, 1, 0x5C); socket.SetColour(0, 1, 0xB0);
		socket.SetGlyph(1, 1, 0x2F); socket.SetColour(1, 1, 0xB0);
		
		player.SetGlyph(0, 0, 0x250C); player.SetColour(0, 0, 0xB0);
		player.SetGlyph(1, 0, 0x2510); player.SetColour(1, 0, 0xB0);
		player.SetGlyph(0, 1, 0x2514); player.SetColour(0, 1, 0xB0);
		player.SetGlyph(1, 1, 0x2518); player.SetColour(1, 1, 0xB0);
	}

	
protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		mapid = 0;
		change_map(0);
		return true;
	}
	
	void change_map(int diff)
	{
		mapid = (mapid + diff) % 90;
		if(mapid < 0) mapid += 90;
		curmap = map[mapid];
		
		px = curmap.sx;
		py = curmap.sy;
		curmap.data[py][px] = ' ';
		
		boxes = 0;
		
		for(int y = 0; y < curmap.height; y++)
		for(int x = 0; x < curmap.width; x++)
		{
			if(curmap.data[y][x] == '$') boxes++;
		}
		
		moves = pushes = time_h = time_m = time_s = 0;
	}
	
	void move_player(int diffx, int diffy)
	{
		int posx = px + diffx;
		int posy = py + diffy;
		
		char block = curmap.data[posy][posx];
		
		if(block == '#') // wall
		{
			return;
		}
		if(block == '$' || block == '+') // pusbox
		{
			int npx = posx + diffx;
			int npy = posy + diffy;
			
			char nblock = curmap.data[npy][npx];
			
			if(nblock == ' ')
			{
				curmap.data[posy][posx] = block == '+' ? '.' : ' ';
				curmap.data[npy][npx] = '$';

				px = posx;
				py = posy;
				moves++;
				pushes++;
				
				if(block == '+') boxes++;
			}
			else if(nblock == '.')
			{
				curmap.data[posy][posx] = block == '+' ? '.' : ' ';
				curmap.data[npy][npx] = '+';
				
				px = posx;
				py = posy;
				moves++;
				pushes++;
				
				if(block == '$') boxes--;
			}
			
			if(boxes == 0) change_map(1);
		}
		else
		{
			px = posx;
			py = posy;
			moves++;
		}
	}
	
	void check_input(void)
	{
		if(m_keys[VK_PRIOR].bPressed) change_map(-1);
		if(m_keys[VK_NEXT].bPressed) change_map(1);
		
		if(m_keys[VK_LEFT].bPressed) move_player(-1, 0);
		if(m_keys[VK_RIGHT].bPressed) move_player(1, 0);
		if(m_keys[VK_UP].bPressed) move_player(0, -1);
		if(m_keys[VK_DOWN].bPressed) move_player(0, 1);
	}
	
	void update_timer(float dt)
	{
		static float accum_time;
		
		accum_time += dt;
		if(accum_time > 1.0f)
		{
			accum_time -= 1.0f;
			time_s++;
			
			if(time_s >= 60)
			{
				time_s -= 60;
				time_m++;
				
				if(time_m >= 60)
				{
					time_m -= 60;
					time_h++;
					if(time_h > 99) time_h = 99;
				}
			}
		}
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		
		check_input();
		update_timer(fElapsedTime);
		
		Fill(0, 0, 80, 80, 0x2588, 0xBB);
		
		int offy = (34 - curmap.height * 2) / 2;
		int offx = (40 - curmap.width * 2) / 2;
		
		for(int y = 0; y < curmap.height; y++)
		for(int x = 0; x < curmap.width; x++)
		{
			char tile = curmap.data[y][x];
			
			switch(tile)
			{
				case '#': DrawSprite(x*2 + offx, y*2 + offy, &wall); break;
				case '$': DrawSprite(x*2 + offx, y*2 + offy, &box); break;
				case '+': DrawSprite(x*2 + offx, y*2 + offy, &box2); break;
				case '.': DrawSprite(x*2 + offx, y*2 + offy, &socket); break;
			}
		}
		
		DrawSprite(px*2 + offx, py*2 + offy, &player);
		
		//20x16
		
		wsprintf(statusbar, L"%02d| moves:%04d pushes:%04d time:%02d:%02d:%02d", 
			mapid + 1, moves, pushes, time_h, time_m, time_s);
			
		DrawString(0, 34, statusbar, 0xF0);
		Draw( 2, 34, 0x2502, 0xFD);
		Draw( 3, 34, 0x2588, 0xFF);
		Draw(14, 34, 0x2588, 0xFF);
		Draw(26, 34, 0x2588, 0xFF);
		return true;
	}
	
	// Called by olcConsoleGameEngine, optional
	virtual bool OnUserDestroy(float fElapsedTime)
	{
		return true;
	}
	
private:
	olcSprite wall, box, box2, socket, player;

	int moves, pushes, time_h, time_m, time_s;
	int mapid, px, py, boxes;
	sokomap curmap;
	wchar_t statusbar[64];
};

int main(void)
{
	OneLoneCoder_Sample game;
	
	//16x12
	//320 x 280
	if(game.ConstructConsole(40, 35, 8, 8))
	{
		game.Start();
	}

	return 0;
}
