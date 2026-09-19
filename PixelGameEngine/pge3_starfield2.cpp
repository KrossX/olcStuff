/* Copyright (c) 2026 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#define WND_WIDTH  320
#define WND_HEIGHT 240
#define NUM_STARS  200

class ksx_starfield : public olc::PixelGameEngine
{
	float star_x[NUM_STARS];
	float star_y[NUM_STARS];
	float star_z[NUM_STARS];

	float randf() {
		return rand() / (float)RAND_MAX;
	}

public:
	bool OnUserCreate() override
	{
		for (int i = 0; i < NUM_STARS; i++) {
			star_x[i] = randf() * WND_WIDTH;
			star_y[i] = randf() * WND_HEIGHT;
			star_z[i] = randf() * 9.0f + 1.0f;
		}

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		draw.Clear(olc::Pixel(20,20,20));

		for (int i = 0; i < NUM_STARS; i++) {
			star_x[i] -=  10.0f / star_z[i];

			if(star_x[i] < 0) {
				star_x[i] = WND_WIDTH;
				star_y[i] = randf() * WND_HEIGHT;
			}
			
			draw.Pixel({star_x[i], star_y[i]}, olc::Colour::WHITE / star_z[i] * 2.0f);
		}

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