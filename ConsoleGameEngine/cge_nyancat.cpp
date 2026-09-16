/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

//#include "olcConsoleGameEngine.h"
#include "olcConsoleGameEngineGL.h"

uint8_t *nyan_song;

WAVEHDR  whdr[2];
HWAVEOUT wave_out;

class OneLoneCoder_Sample : public olcConsoleGameEngine
{
public:
	OneLoneCoder_Sample()
	{
		m_sAppName = L"Nyan Cat";
	}

	
protected:
	// Called by olcConsoleGameEngine
	virtual bool OnUserCreate()
	{
		// x 15 = 66.66666 ms
		
		star[0].ffspeed = 15.0f;
		star[1].ffspeed = 15.0f;
		star[2].ffspeed = 15.0f;
		star[3].ffspeed = 15.0f;
		star[4].ffspeed = -15.0f;
		star[5].ffspeed = 15.0f;
		
		star[0].fxspeed = 105.0f;
		star[1].fxspeed = 69.0f;
		star[2].fxspeed = 69.0f;
		star[3].fxspeed = 69.0f;
		star[4].fxspeed = 87.0f;
		star[5].fxspeed = 138.0f;
		
		star[0].fframe = 1.0f;
		star[1].fframe = 3.0f;
		star[2].fframe = 1.0f;
		star[3].fframe = 5.0f;
		star[4].fframe = 5.0f;
		star[5].fframe = 2.0f;
		
		star[0].fx = 45.0f;
		star[1].fx = 75.0f;
		star[2].fx = -2.0f;
		star[3].fx = 8.0f;
		star[4].fx = 79.0f;
		star[5].fx = 40.0f;

		star[0].y = -2;
		star[1].y = 8;
		star[2].y = 21;
		star[3].y = 46;
		star[4].y = 60;
		star[5].y = 73;
		
		
		nyan_cookie = olcSprite(L"sprites/nyan_cookie.spr");
		nyan_head   = olcSprite(L"sprites/nyan_head.spr");
		
		nyan_tail[0] = olcSprite(L"sprites/nyan_tail_0.spr");
		nyan_tail[1] = olcSprite(L"sprites/nyan_tail_1.spr");
		nyan_tail[2] = olcSprite(L"sprites/nyan_tail_2.spr");
		nyan_tail[3] = olcSprite(L"sprites/nyan_tail_3.spr");
		nyan_tail[4] = olcSprite(L"sprites/nyan_tail_4.spr");
		nyan_tail[5] = olcSprite(L"sprites/nyan_tail_5.spr");
		
		nyan_legs[0] = olcSprite(L"sprites/nyan_legs_0.spr");
		nyan_legs[1] = olcSprite(L"sprites/nyan_legs_1.spr");
		nyan_legs[2] = olcSprite(L"sprites/nyan_legs_2.spr");
		nyan_legs[3] = olcSprite(L"sprites/nyan_legs_3.spr");
		nyan_legs[4] = olcSprite(L"sprites/nyan_legs_4.spr");
		nyan_legs[5] = olcSprite(L"sprites/nyan_legs_5.spr");

		return true;
	}

	// Called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		static float time_rainbow;
		static float time_nyan_cookie;
		static float time_nyan_main;
		
		//fElapsedTime = 0;
	
		//clear screen	
		Fill(0, 0, 80, 80, 0x2592, 0x1F);
		
		//rainbow
		Fill(0, 31, 31, 34, 0x2588, 0xCC);
		Fill(0, 34, 31, 37, 0x2593, 0xEC);
		Fill(0, 37, 31, 40, 0x2588, 0xEE);
		Fill(0, 40, 31, 43, 0x2588, 0xAA);
		Fill(0, 43, 31, 46, 0x2593, 0x9B);
		Fill(0, 46, 31, 48, 0x2592, 0x59);
		
		time_rainbow += fElapsedTime * 7.5f; // 133.33333 ms
		if(time_rainbow >= 2.0f) time_rainbow -= 2.0f;
		
		if((int)time_rainbow)
		{
			Fill( 0, 30,  3, 31, 0x2588, 0xCC);
			Fill(12, 30, 21, 31, 0x2588, 0xCC);
			
			Fill( 0, 33,  3, 34, 0x2593, 0xEC);
			Fill(12, 33, 21, 34, 0x2593, 0xEC);
			
			Fill( 0, 36,  3, 37, 0x2588, 0xEE);
			Fill(12, 36, 21, 37, 0x2588, 0xEE);
			
			Fill( 0, 39,  3, 40, 0x2588, 0xAA);
			Fill(12, 39, 21, 40, 0x2588, 0xAA);
			
			Fill( 0, 42,  3, 43, 0x2593, 0x9B);
			Fill(12, 42, 21, 43, 0x2593, 0x9B);
			
			Fill( 0, 45,  3, 46, 0x2592, 0x59);
			Fill(12, 45, 21, 46, 0x2592, 0x59);
			
			Fill( 3, 48, 12, 49, 0x2592, 0x59);
			Fill(21, 48, 30, 49, 0x2592, 0x59);
		}
		else
		{
			Fill( 3, 30, 12, 31, 0x2588, 0xCC);
			Fill(21, 30, 31, 31, 0x2588, 0xCC);
			
			Fill( 3, 33, 12, 34, 0x2593, 0xEC);
			Fill(21, 33, 31, 34, 0x2593, 0xEC);
			
			Fill( 3, 36, 12, 37, 0x2588, 0xEE);
			Fill(21, 36, 31, 37, 0x2588, 0xEE);
			
			Fill( 3, 39, 12, 40, 0x2588, 0xAA);
			Fill(21, 39, 31, 40, 0x2588, 0xAA);
			
			Fill( 3, 42, 12, 43, 0x2593, 0x9B);
			Fill(21, 42, 31, 43, 0x2593, 0x9B);
			
			Fill( 3, 45, 12, 46, 0x2592, 0x59);
			Fill(21, 45, 31, 46, 0x2592, 0x59);
			
			Fill( 0, 48,  3, 49, 0x2592, 0x59);
			Fill(12, 48, 21, 49, 0x2592, 0x59);
		}
		
		for(Star &s : star)
		{
			s.fframe += fElapsedTime * s.ffspeed;
			if(s.fframe >= 6.0f) s.fframe -= 6.0f;
			else if(s.fframe <= 0) s.fframe += 6.0f;
			
			s.fx -= fElapsedTime * s.fxspeed;
			if(s.fx < -15.0f) s.fx += 100.0f;
			
			s.x = (int)s.fx;
			s.frame = (int)s.fframe;
			DrawStar(s);
		}
		
		time_nyan_main += fElapsedTime * 15.0f;
		if(time_nyan_main >= 6.0f) time_nyan_main -= 6.0f;
		
		switch((int)time_nyan_main)
		{
			case  0: DrawSprite(26, 44, &nyan_legs[0]); break;
			case  1: DrawSprite(26, 44, &nyan_legs[1]); break;
			case  2: DrawSprite(26, 45, &nyan_legs[2]); break;
			case  3: DrawSprite(26, 45, &nyan_legs[3]); break;
			case  4: DrawSprite(26, 45, &nyan_legs[4]); break;
			case  5: DrawSprite(26, 45, &nyan_legs[5]); break;
		}
		
		time_nyan_cookie += fElapsedTime * 7.5f;
		if(time_nyan_cookie >= 3.0f) time_nyan_cookie -= 3.0f;
		
		DrawSprite(29, time_nyan_cookie < 1.0f ? 29 : 30, &nyan_cookie);
	
		switch((int)time_nyan_main)
		{
			case  0: DrawSprite(39, 34, &nyan_head); break;
			case  1: DrawSprite(40, 34, &nyan_head); break;
			case  2: DrawSprite(40, 35, &nyan_head); break;
			case  3: DrawSprite(40, 35, &nyan_head); break;
			case  4: DrawSprite(39, 35, &nyan_head); break;
			case  5: DrawSprite(39, 34, &nyan_head); break;
		}
		
		switch((int)time_nyan_main)
		{
			case  0: DrawSprite(22, 36, &nyan_tail[0]); break;
			case  1: DrawSprite(22, 36, &nyan_tail[1]); break;
			case  2: DrawSprite(22, 37, &nyan_tail[2]); break;
			case  3: DrawSprite(22, 37, &nyan_tail[3]); break;
			case  4: DrawSprite(22, 37, &nyan_tail[4]); break;
			case  5: DrawSprite(22, 37, &nyan_tail[5]); break;
		}

		return true;
	}
	
	// Called by olcConsoleGameEngine, optional
	virtual bool OnUserDestroy(float fElapsedTime)
	{
		return true;
	}
	
private:

	struct Star
	{
		float fxspeed, ffspeed;
		float fx, fframe;
		int x, y, frame;
	} star[6];
	
	void DrawStar(Star &star)
	{
		switch(star.frame)
		{
			case 0: Draw(star.x + 3, star.y + 3, 0x2588, 0xFF); break;

			case 1: Draw(star.x + 3, star.y + 2, 0x2588, 0xFF);
			        Draw(star.x + 2, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 4, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 4, 0x2588, 0xFF); break;

			case 2: Draw(star.x + 3, star.y + 1, 0x2588, 0xFF);
			        Draw(star.x + 3, star.y + 2, 0x2588, 0xFF);
					Draw(star.x + 1, star.y + 3, 0x2588, 0xFF);
			        Draw(star.x + 2, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 4, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 5, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 4, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 5, 0x2588, 0xFF); break;

			case 3: Draw(star.x + 3, star.y + 0, 0x2588, 0xFF);
			        Draw(star.x + 3, star.y + 1, 0x2588, 0xFF);
					Draw(star.x + 0, star.y + 3, 0x2588, 0xFF);
			        Draw(star.x + 1, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 5, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 6, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 5, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 6, 0x2588, 0xFF); break;

			case 4: Draw(star.x + 3, star.y + 0, 0x2588, 0xFF);
			        Draw(star.x + 1, star.y + 1, 0x2588, 0xFF);
					Draw(star.x + 5, star.y + 1, 0x2588, 0xFF);
			        Draw(star.x + 0, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 6, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 1, star.y + 5, 0x2588, 0xFF);
					Draw(star.x + 5, star.y + 5, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 6, 0x2588, 0xFF); break;

			case 5: Draw(star.x + 3, star.y + 0, 0x2588, 0xFF);
			        Draw(star.x + 0, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 6, star.y + 3, 0x2588, 0xFF);
					Draw(star.x + 3, star.y + 6, 0x2588, 0xFF); break;
		}
	}
	
	olcSprite nyan_cookie;
	olcSprite nyan_head;
	olcSprite nyan_tail[6];
	olcSprite nyan_legs[6];
};

void CALLBACK waveOutProc(HWAVEOUT hwo,	UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if(uMsg == WOM_DONE)
	{
		static size_t counter;
		
		WAVEHDR *waveupdate = (LPWAVEHDR)dwParam1;
		
		if(counter&1)
		{
			waveupdate->dwBufferLength = 298422 * 2;
			waveupdate->lpData = (LPSTR)&nyan_song[44 + 638910];
		}
		else
		{
			waveupdate->dwBufferLength = 298422 * 2;
			waveupdate->lpData = (LPSTR)&nyan_song[44 + 638910 + 596844];
		}
		
		waveOutWrite(wave_out, waveupdate, sizeof(WAVEHDR));
		counter++;
	}
}


void snd_init(void)
{
	HANDLE file = CreateFileA("nyansong_mono_22khz.wav", GENERIC_READ, 0,
					nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if(file != INVALID_HANDLE_VALUE)
	{
		DWORD filesize = GetFileSize(file, nullptr);
		
		if(filesize > 32 && filesize != 0xFFFFFFFF)
		{
			nyan_song = new uint8_t[filesize];
			
			DWORD bytes_read;
			ReadFile(file, nyan_song, filesize, &bytes_read, nullptr);
		}
		
		CloseHandle(file);
	}
	
	if(nyan_song)
	{
		WAVEFORMATEX *wavfmt = (WAVEFORMATEX*)&nyan_song[20];
		
		MMRESULT res = waveOutOpen(&wave_out, WAVE_MAPPER, wavfmt, (DWORD_PTR)waveOutProc, 0, CALLBACK_FUNCTION);
		
		if(res == MMSYSERR_NOERROR)
		{
			memset(whdr, 0, sizeof(WAVEHDR) * 2);

			whdr[0].dwBufferLength = 319455 * 2;
			whdr[0].lpData = (LPSTR)&nyan_song[44];
			
			whdr[1].dwBufferLength = 298422 * 2;
			whdr[1].lpData = (LPSTR)&nyan_song[44 + 638910];
			
			waveOutPrepareHeader(wave_out, &whdr[0], sizeof(WAVEHDR));
			waveOutPrepareHeader(wave_out, &whdr[1], sizeof(WAVEHDR));
			
			waveOutWrite(wave_out, &whdr[0], sizeof(WAVEHDR));
			waveOutWrite(wave_out, &whdr[1], sizeof(WAVEHDR));
		}
	}
	
	
}

void snd_shutdown(void)
{
	if(wave_out)
	{
		//waveOutReset(wave_out);
		waveOutUnprepareHeader(wave_out, &whdr[0], sizeof(WAVEHDR));
		waveOutUnprepareHeader(wave_out, &whdr[1], sizeof(WAVEHDR));
		
		waveOutClose(wave_out);
		wave_out = nullptr;
	}
	
	if(nyan_song) delete[] nyan_song;
	
	nyan_song = nullptr;
}

int main(void)
{
	OneLoneCoder_Sample game;

	if(game.ConstructConsole(80, 80, 4, 4))
	{
		snd_init();
		game.Start();
		snd_shutdown();
	}

	return 0;
}
