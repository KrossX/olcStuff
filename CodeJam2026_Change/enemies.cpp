enum {
	ENEMY_SAUCER_LOW = 0,
	ENEMY_SAUCER_MEDIUM,
	ENEMY_SAUCER_HIGH
};

enum {
	ENEMY_ENTER = 0,
	ENEMY_NORMAL,
	ENEMY_EXIT
};


struct enemy {
	olc::vf2d pos, spd;
	bool active;
	int type;
	int state;
	int pattern;
	float timer;
	float red;
	float hp;
};

enemy enemy_list[MAX_ENEMIES];
int enemy_count;

void enemy_init(void)
{
	for(int i = 0; i < MAX_ENEMIES; i++) {
		enemy_list[i].active = false;
	}
}

void enemy_add(olc::vf2d pos, olc::vf2d speed, int type,  float timer, float hitpoints)
{
	for(enemy &e : enemy_list) {
		if(e.active) continue;
		e = {pos, speed, true, type, ENEMY_ENTER, 0, timer, 0, hitpoints};
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
		e.red -= dt * 3.0f;

		float aposx = std::abs(e.pos.x - (float)world_posx);
		float edge = (30.0f - e.pos.y) * 0.38f;

		switch(e.state) {
		case ENEMY_ENTER:
			if(aposx < edge)
				e.state = ENEMY_NORMAL;
			break;

		case ENEMY_NORMAL:
			if(e.timer > 1) {
				e.timer -= 1.0f;

				if(aposx < edge && e.pos.y < -20.0f) {
					olc::vf2d ppos((float)player.posx, player.posy);
					olc::vf2d diff = ppos - e.pos;
					particle_add(e.pos, diff.norm() * 50.0f, {0,0}, BULLET_ENEMY_PINK);
				}
			}

			if(aposx > edge || e.pos.y > 0.0f) {
				e.state = ENEMY_EXIT;
				e.timer = 0;
			} break;

		case ENEMY_EXIT:
			if(aposx > (edge + 5.0f) || e.pos.y > 4.0f || e.timer > 3.0f)
				e.active = false;
			break;
		}

/*
		if(e.timer > 1) {
			e.timer -= 1.0f;

			if(aposx < edge  && e.pos.y < -20.0f) {
				olc::vf2d ppos((float)player.posx, player.posy);
				olc::vf2d diff = ppos - e.pos;
				particle_add(e.pos, diff.norm() * 50.0f, BULLET_ENEMY_PINK);
			}
		}

		if(e.pos.y > 4.0f || aposx > (edge + 5.0f)) {
			e.active = false;
			//enemy_add_test();
		}
*/
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

		mesh_draw(mesh::enemy_saucer, true, e.red > 0 ? PixelLerp(olc::Colour::WHITE, olc::Colour::RED, e.red * e.red) : olc::Colour::WHITE);
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
				p.active = false;

				e.hp -= p.damage;
				if(e.hp <= 0) {
					e.active = false;
					particle_add(e.pos, e.spd * 0.5f, {0,0}, PARTICLE_EXPLOSION);
					audio_playpan(SND_BOOM, e.pos.x, e.pos.y);
				} else {
					e.red = 1.0f;

					for(int i = 0; i < 3; i++) {
						float angle = (float)rand() / (float)RAND_MAX * 3.14159f * 2.0f;
						float speed = (float)rand() / (float)RAND_MAX * 10.0f + 20.0f;
						float spdx = e.spd.x * 0.5f + std::sinf(angle) * speed;
						float spdy = e.spd.y * 0.5f + speed;
						float spdz = std::cosf(angle) * speed;

						particle_add(e.pos, {spdx, spdy}, {0,spdz}, PARTICLE_SPARK);
					}

					audio_playpan(SND_HIT, e.pos.x, e.pos.y);
				}
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
	static int count;

	//olc::vf2d pos = { (float)world_posx + (30.0f +10.0f) * 0.4f, -10.0f }; //(30.0f - e.pos.y) * 0.38f
	olc::vf2d pos = { (float)world_posx + 30.0f, -40.0f }; //(30.0f - e.pos.y) * 0.38f
	olc::vf2d offset = olc::vf2d(4.0f, 0.0f).norm();

	switch(count&1) {
		// right to left
		case 0: {
			olc::vf2d pos = { (float)world_posx + 30.0f, -40.0f };
			olc::vf2d offset = olc::vf2d(4.0f, 0.0f).norm();
			for(int i = 0; i < 5; i++) {
				enemy_add(pos + offset*(i+1)*7.0f, -offset * 20.0f, ENEMY_SAUCER_LOW, 0, 1);
			}
		} break;
		// left to right
		case 1:	{
			olc::vf2d pos = { (float)world_posx - 30.0f, -40.0f };
			olc::vf2d offset = olc::vf2d(-4.0f, 0.0f).norm();
			for(int i = 0; i < 5; i++) {
				enemy_add(pos + offset*(i+1)*7.0f, -offset * 20.0f, ENEMY_SAUCER_LOW, 0, 3);
			}
		} break;
	}

	count++;
}