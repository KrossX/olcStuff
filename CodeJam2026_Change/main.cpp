/* Copyright (c) 2026 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#define OLC_USE_STB_IMAGE
#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

#define WND_WIDTH  320
#define WND_HEIGHT 224

class ksx_pewpew : public olc::PixelGameEngine
{

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		return true;
	}
};

int main(void)
{
	ksx_pewpew pge;
	olc::PGEConfig cfg;

	cfg.vScreenSize = {WND_WIDTH, WND_HEIGHT};
	cfg.vPixelSize = {2,2};
	cfg.sAppName = "olcJam2026: CHANGE";

	if (pge.Construct(cfg))
		pge.Start();

	return 0;
}