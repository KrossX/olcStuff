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
	olc::gpu::Shader_GLSL33 bg_render;
	olc::Image bg_image;
	olc::vf2d bg_off, bg_region = olc::vf2d(WND_WIDTH, WND_HEIGHT) / 64.0f;

public:
	bool OnUserCreate() override
	{
		std::string main_ps = 
#include "main_ps.glsl"
		
		bg_render.SetPixelShaderSource(olc::gpu::Shader::PS_DefaultHeader() + main_ps);
		bg_render.SetVertexShaderSource(olc::gpu::Shader::VS_DefaultHeader() + olc::gpu::Shader::VS_DefaultMain());
		
		std::string shader_error = bg_render.Compile();
		
		if(shader_error != "OK") {
			std::cout << "Error compiling shader: " << shader_error << std::endl;
			return false;
		}
		
		bg_render.CreateUniform("bg_offset");
		bg_render.CreateUniform("bg_region");

		CreateImageFromFile(bg_image, "./data/noise.png", {true});

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		bg_off.y -= fElapsedTime * 0.25f;
		if(bg_off.y < -1) bg_off.y += 1;
		
		draw.SetShader(bg_render);
		draw.SetShaderUniform("bg_offset", bg_off);
		draw.SetShaderUniform("bg_region", bg_region);
		draw.ImageRect(bg_image, {0,0}, {320,224});
		draw.ResetShader();
		
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