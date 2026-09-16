/* Copyright (c) 2026 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#define WND_WIDTH  320
#define WND_HEIGHT 240
#define NUM_STARS  200

#define CENTERX (WND_WIDTH/2)
#define CENTERY (WND_HEIGHT/2)


class ksx_starfield : public olc::PixelGameEngine
{
	float star_x[NUM_STARS];
	float star_y[NUM_STARS];
	float star_z[NUM_STARS];
	
	float randf() {
		return rand() / (float)RAND_MAX;
	}
	
	void create_star(int idx) {

		star_x[idx] = (randf() * WND_WIDTH - CENTERX) * 100.0f;
		star_y[idx] = (randf() * WND_HEIGHT - CENTERY) * 100.0f;
		star_z[idx] = (randf() * 200) + 5.0f;
	}

public:
	bool OnUserCreate() override
	{
		for (int i = 0; i < NUM_STARS; i++) {
			create_star(i);
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		static float speed = 0.3f;
		
		draw.Clear(olc::Pixel(20,20,20));
		
		for (int i = 0; i < NUM_STARS; i++) {
			float posz = star_z[i];
			float posx = star_x[i] / posz + CENTERX;
			float posy = star_y[i] / posz + CENTERY;
						
			star_z[i] = posz - speed * speed;
			
			if(posz < 1 || posx < 0 || posy < 0 || posx > WND_WIDTH || posy > WND_HEIGHT) {
				create_star(i);
			}

			draw.Pixel({posx, posy});
		}
		
		speed += fElapsedTime * 0.1f;
		
		return true;
	}
};

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, char *cmdline, int cmdshow)
{
	ksx_starfield pge;
	olc::PGEConfig cfg;
	
	cfg.vScreenSize = {WND_WIDTH,WND_HEIGHT};
	cfg.vPixelSize = {2,2};
	//cfg.bVSync = false;
	cfg.sAppName = "KSX";

	if (pge.Construct(cfg))
		pge.Start();

	return 0;
}