#define MAX_WEAPONS 3

enum {
	PLAYER_WEAPON_Z = 0,
	PLAYER_WEAPON_X,
	PLAYER_WEAPON_C,
};

struct weapon {
	int active;
	int count, shots;
	float damage, rate, hold, timer;
};

weapon player_weapon[MAX_WEAPONS];

void weapon_init(void)
{
	weapon *w = &player_weapon[PLAYER_WEAPON_Z];

	w->active = 0;
	w->damage = 1;
	w->count  = 1;
	w->shots  = 0;
	w->rate   = 60.0f / 400.0f; // Rounds per minute
	w->hold   = 0;
	w->timer  = 0;

	w = &player_weapon[PLAYER_WEAPON_X];

	w->active = 0;
	w->damage = 1;
	w->count  = 1;
	w->shots  = 0;
	w->rate   = 60.0f / 400.0f; // Rounds per minute
	w->hold   = 0;
	w->timer  = 0;

	w = &player_weapon[PLAYER_WEAPON_C];

	w->active = 0;
	w->damage = 1;
	w->count  = 1;
	w->shots  = 0;
	w->rate   = 60.0f / 400.0f; // Rounds per minute
	w->hold   = 0;
	w->timer  = 0;
}

void weapon_shoot(int id)
{
	weapon &w  = player_weapon[id];
	weapon &w2 = player_weapon[(id+1)%MAX_WEAPONS];
	weapon &w3 = player_weapon[(id+2)%MAX_WEAPONS];

	if(!w.active) {
		w3.active = 0;
		w2.active = 0;
		w.shots = 0;
		w.timer = w.rate;
	}

	w.active  = 2;
}


void Weapon_step(float dt)
{
	for(int i = 0; i < MAX_WEAPONS; i++) {
		weapon &w = player_weapon[PLAYER_WEAPON_Z+i];
		if(!w.active) continue;
		w.timer += dt;

		float factor = (player.speed - 0.5f) * 2.0f;
		float rate =  w.rate * (0.9f + 0.1f*factor);

		if(w.timer >= rate) {
			w.timer -= rate;

			if(w.shots&3) {
				float offset = 0.5f + 0.2f*i;
				float angle = 0;

				switch(i) {
				case PLAYER_WEAPON_Z: angle =  5.0f * 3.14159f / 180.0f * (player.speed - 0.3f) / 0.7f; break;
				case PLAYER_WEAPON_X: angle = 10.0f * 3.14159f / 180.0f; break;
				case PLAYER_WEAPON_C: angle = 90.0f * 3.14159f / 180.0f; break;
				}

				float vely = -std::cosf(angle);
				float velx =  std::sinf(angle);

				olc::vf2d vleft   = olc::vf2d(-velx,vely) * 80.0f;
				olc::vf2d vcenter = olc::vf2d(0.0f,-1.0f) * 80.0f;
				olc::vf2d vright  = olc::vf2d( velx,vely) * 80.0f;


				int bullet_id = BULLET_PLAYER_W1 + i;

				particle_add({(float)player.posx,player.posy}, vcenter, {0,0}, bullet_id, w.damage);
				particle_add({(float)player.posx -offset,player.posy}, vleft, {0,0}, bullet_id, w.damage);
				particle_add({(float)player.posx +offset,player.posy}, vright, {0,0}, bullet_id, w.damage);
				audio_playpan(SND_SHOOT, (float)player.posx, player.posy);
			} else {
				w.active--;
			}

			w.shots++;
		}
	}
}


void check_input(float dt)
{
	float roll = 0;

	player.speed += (player.hold ? -dt : dt);
	player.speed = std::clamp(player.speed, 0.5f, 1.0f);

	float speed = player.speed * 20.0f;

	if (keyboard.GetKey(olc::Key::F11).bPressed) {
		isFullscreen = !isFullscreen;
		ShowFullScreen(isFullscreen);
	}

	if (keyboard.GetKey(olc::Key::UP).bHeld) {
		player.posy -= dt * speed;
		if(player.posy < -40.0f) player.posy = -40.0f;
	}

	if (keyboard.GetKey(olc::Key::DOWN).bHeld) {
		player.posy += dt * speed;
		if(player.posy > 0) player.posy = 0;

		float edge = (30.0f - player.posy) * 0.38f;
		if((player.posx - world_posx) < -edge) { player.posx = world_posx - edge; }
		if((player.posx - world_posx) >  edge) { player.posx = world_posx + edge; }
	}

	if (keyboard.GetKey(olc::Key::LEFT).bHeld) {
		player.posx -= dt * speed;
		roll -= dt;

		float edge = (30.0f - player.posy) * 0.38f;
		if((player.posx - world_posx) < -edge) { world_posx -= 20.0 * dt; }
	}

	if (keyboard.GetKey(olc::Key::RIGHT).bHeld) {
		player.posx += dt * speed;
		roll += dt;

		float edge = (30.0f - player.posy) * 0.38f;
		if((player.posx - world_posx) >  edge) { world_posx += 20.0 * dt; }
	}

	if(std::abs(roll) < dt) {
		player.roll += -player.roll * dt * 16.0f;
	} else {
		player.roll += roll * 16.0f;
		player.roll = std::clamp(player.roll, -1.0f, 1.0f);
	}

	if(keyboard.GetKey(olc::Key::Z).bPressed)  { weapon_shoot(PLAYER_WEAPON_Z); player_weapon[PLAYER_WEAPON_Z].hold  = 0; }
	if(keyboard.GetKey(olc::Key::Z).bHeld)     { weapon_shoot(PLAYER_WEAPON_Z); player_weapon[PLAYER_WEAPON_Z].hold += dt; }
	if(keyboard.GetKey(olc::Key::Z).bReleased) { player_weapon[PLAYER_WEAPON_Z].hold = 0; }

	if(keyboard.GetKey(olc::Key::X).bPressed)  { weapon_shoot(PLAYER_WEAPON_X); player_weapon[PLAYER_WEAPON_X].hold  = 0; }
	if(keyboard.GetKey(olc::Key::X).bHeld)     { weapon_shoot(PLAYER_WEAPON_X); player_weapon[PLAYER_WEAPON_X].hold += dt; }
	if(keyboard.GetKey(olc::Key::X).bReleased) { player_weapon[PLAYER_WEAPON_X].hold = 0; }

	if(keyboard.GetKey(olc::Key::C).bPressed)  { weapon_shoot(PLAYER_WEAPON_C); player_weapon[PLAYER_WEAPON_C].hold  = 0; }
	if(keyboard.GetKey(olc::Key::C).bHeld)     { weapon_shoot(PLAYER_WEAPON_C); player_weapon[PLAYER_WEAPON_C].hold += dt; }
	if(keyboard.GetKey(olc::Key::C).bReleased) { player_weapon[PLAYER_WEAPON_C].hold = 0; }

	player.hold  = false;
	player.hold |= player_weapon[PLAYER_WEAPON_Z].hold > 0.5f;
	player.hold |= player_weapon[PLAYER_WEAPON_X].hold > 0.5f;
	player.hold |= player_weapon[PLAYER_WEAPON_C].hold > 0.5f;


	if(keyboard.GetKey(olc::Key::Q).bPressed)  {  enemy_factory(); }
}