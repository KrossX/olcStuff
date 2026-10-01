#define MAX_WEAPONS 3

enum {
	PLAYER_WEAPON_Z = 0,
	PLAYER_WEAPON_X,
	PLAYER_WEAPON_C,
};

struct weapon {
	int active;
	int damage, count, shots;
	float rate, hold, timer;
};

weapon player_weapon[MAX_WEAPONS];

void weapon_init(void)
{
	weapon *w = &player_weapon[PLAYER_WEAPON_Z];

	w->active = 0;
	w->damage = 1;
	w->count = 1;
	w->shots = 0;
	w->rate = 60.0f / 400.0f; // Rounds per minute
	w->hold = 0;
	w->timer = 0;
}

void weapon_shoot(int id)
{
	weapon &w  = player_weapon[id];
	weapon &w2 = player_weapon[(id+1)%MAX_WEAPONS];
	weapon &w3 = player_weapon[(id+2)%MAX_WEAPONS];

	if(!w.active) {
		w3.active = 0;
		w2.active = 0;
		w.active  = 2;
		w.shots = 0;
		w.hold = 0;
		w.timer = 0;
	}

	switch(id) {
	case PLAYER_WEAPON_Z:
		w.active = 2;
		break;

	case PLAYER_WEAPON_X:
		break;

	case PLAYER_WEAPON_C:
		break;

	default:
		break;
	}
}


void Weapon_step(float dt)
{
	weapon *w = &player_weapon[PLAYER_WEAPON_Z];

	if(w->active) {
		w->timer += dt;

		if(w->timer >= w->rate) {
			w->timer -= w->rate;

			w->count++;

			if(w->count&3) {
				particle_add({(float)player.posx,player.posy}, {0.0f,-80.0f}, BULLET_PLAYER_W1, w->damage);
				audio_playpan(SND_SHOOT, (float)player.posx, player.posy);
			} else {
				if(w->count > 4) w->active--;
			}
		}
	}

	w = &player_weapon[PLAYER_WEAPON_X];
	if(w->active) {
	}

	w = &player_weapon[PLAYER_WEAPON_C];
	if(w->active) {
	}
}


void check_input(float dt)
{
	float roll = 0;

	if (keyboard.GetKey(olc::Key::F11).bPressed) {
		isFullscreen = !isFullscreen;
		ShowFullScreen(isFullscreen);
	}

	if (keyboard.GetKey(olc::Key::UP).bHeld) {
		player.posy -= 20.0f * dt;
		if(player.posy < -40.0f) player.posy = -40.0f;
	}

	if (keyboard.GetKey(olc::Key::DOWN).bHeld) {
		player.posy += 20.0f * dt;
		if(player.posy > 0) player.posy = 0;
		
		float edge = (30.0f - player.posy) * 0.38f;
		if((player.posx - world_posx) < -edge) { player.posx = world_posx - edge; }
		if((player.posx - world_posx) >  edge) { player.posx = world_posx + edge; }
	}

	if (keyboard.GetKey(olc::Key::LEFT).bHeld) {
		player.posx -= 20.0 * dt;
		roll -= dt;

		float edge = (30.0f - player.posy) * 0.38f;
		if((player.posx - world_posx) < -edge) { world_posx -= 20.0 * dt; }
	}

	if (keyboard.GetKey(olc::Key::RIGHT).bHeld) {
		player.posx += 20.0 * dt;
		roll += dt;

		float edge = (30.0f - player.posy) * 0.38f;
		if((player.posx - world_posx) >  edge) { world_posx += 20.0 * dt; }
	}
	
	if(std::abs(roll) < dt) {
		player.roll += -player.roll * dt * 8.0f;
	} else {
		player.roll += roll * 8.0f;
		player.roll = std::clamp(player.roll, -1.0f, 1.0f);
	}

	if(keyboard.GetKey(olc::Key::Z).bPressed)  {  weapon_shoot(PLAYER_WEAPON_Z); }
	if(keyboard.GetKey(olc::Key::Z).bHeld)     { weapon_shoot(PLAYER_WEAPON_Z); } //player_weapon[PLAYER_WEAPON_Z].hold += dt; }
	if(keyboard.GetKey(olc::Key::Z).bReleased) { player_weapon[PLAYER_WEAPON_Z].hold  = 0;	}

	if(keyboard.GetKey(olc::Key::X).bPressed)  {  weapon_shoot(PLAYER_WEAPON_X); }
	if(keyboard.GetKey(olc::Key::X).bHeld)     { player_weapon[PLAYER_WEAPON_X].hold += dt; }
	if(keyboard.GetKey(olc::Key::X).bReleased) { player_weapon[PLAYER_WEAPON_X].hold  = 0;	}

	if(keyboard.GetKey(olc::Key::C).bPressed)  {  weapon_shoot(PLAYER_WEAPON_C); }
	if(keyboard.GetKey(olc::Key::C).bHeld)     { player_weapon[PLAYER_WEAPON_C].hold += dt; }
	if(keyboard.GetKey(olc::Key::C).bReleased) { player_weapon[PLAYER_WEAPON_C].hold  = 0;	}
	
	
	if(keyboard.GetKey(olc::Key::Q).bPressed)  {  enemy_factory(); }
}