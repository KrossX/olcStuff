#define MAX_ENEMIES 32

enum {
	ENEMY_SAUCER_LOW = 0,
	ENEMY_SAUCER_MEDIUM,
	ENEMY_SAUCER_HIGH
};

struct enemy {
	olc::vf2d pos, spd;
	bool active;
	int type;
	float timer;
	int hitpoints;
};

enemy enemy_list[MAX_ENEMIES];
int enemy_count;

void enemy_init(void)
{
	for(int i = 0; i < MAX_ENEMIES; i++) {
		enemy_list[i].active = false;
	}
}

void enemy_add(olc::vf2d pos, olc::vf2d speed, int type,  float timer, int hitpoints)
{
	for(enemy &e : enemy_list) {
		if(e.active) continue;
		e = {pos, speed, true, type, timer, hitpoints};
		return;
	}

	//int newposx = (int)world_posx + (rand() % 50 - 25) * 4;
	//enemy_pos[idx] = {(float)newposx, -300.0f - (rand() % 10) * 4.0f};
	//enemy_timer[idx] = -1.0f - (rand() % 800) / 800.0f;
}


// shooting 0.38

void enemy_step(float dt)
{
	for(enemy &e : enemy_list) {
		if(!e.active) continue;

		e.pos += e.spd * dt;
		e.timer += dt;

		if(e.timer > 1) {
			e.timer -= 1.0f;
			
			float edge = (30.0f - e.pos.y) * 0.38f;
			
			if(std::abs(e.pos.x - (float)world_posx) < edge  && e.pos.y < -20.0f) {
				olc::vf2d ppos((float)player.posx, player.posy);
				olc::vf2d diff = ppos - e.pos;
				particle_add(e.pos, diff.norm() * 50.0f, BULLET_ENEMY_PINK);
			}
		}

		if(e.pos.y > 4.0f) {
			e.active = false;
			enemy_add_test();
		}
	}
}

void enemy_draw(void)
{
	for(enemy &e : enemy_list) {
		if(!e.active) continue;

		enemy_count++;

		olc::mf4d world, scale, rotate;

		rotate.rotateZ((float)TotalTimeElapsed() * 2.0f);
		scale.scale(3.0f, 3.0f, 3.0f);
		world.translate(e.pos.x, e.pos.y, 0.0f);

		draw.SetModelMatrix(world * scale * rotate);

		mesh_draw(mesh::enemy_saucer, true);
	}
}

void check_enemy_dead()
{
	for(enemy &e : enemy_list) {
		if(!e.active) continue;

		for(particle &p : player_bullet) {
			if(!p.active) continue;

			olc::vf2d dist = e.pos - p.pos;
			if(dist.mag2() < 9.0) {
				e.active = false;
				p.active = false;

				audio_playpan(SND_BOOM, e.pos.x, e.pos.y);
				enemy_add_test();
			}
		}
	}
}

void enemy_add_test(void)
{
	int newposx = (int)world_posx + (rand() % 50 - 25) * 4;
	olc::vf2d pos = {(float)newposx, -300.0f - (rand() % 10) * 4.0f};
	olc::vf2d speed = {0, 40.0f};
	float timer = -1.0f - (rand() % 800) / 800.0f;
	
	
	enemy_add(pos, speed, ENEMY_SAUCER_LOW, timer, 1);
}

void enemy_factory(void)
{
	for(int i = 0; i < MAX_ENEMIES; i++) {
		enemy_add_test();
	}
	
	
	
	//int newposx = (int)world_posx + (rand() % 50 - 25) * 4;
	//enemy_pos[idx] = {(float)newposx, -300.0f - (rand() % 10) * 4.0f};
	//enemy_timer[idx] = -1.0f - (rand() % 800) / 800.0f;
	
	
	
	/*
	olc::vf2d pos, speed;
	
	pos.y = -40;
	pos.x = (float)world_posx; // -pos.y * 0.38f + 15.0f;
	//pos.x += (float)world_posx;
	
	speed.x = 0; //(float)(player.posx - world_posx);
	speed.y = 0;
	
	//speed = speed.norm() * 1.0f;
	
	
	enemy_add(pos, speed, ENEMY_SAUCER_LOW, 1);
	*/
}