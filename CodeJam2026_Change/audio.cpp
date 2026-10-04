enum {
	SND_SHOOT = 0,
	SND_HIT,
	SND_BOOM,
	SND_POWERUP,
	SND_SHOOT2,
	SND_HIT2,
	SND_HIT3,
	SND_CHANGE,
	MUS_GOTHIC,
	MUS_HAVOC,
	SND_TOTAL
};

olc::ext::Miniaudio::AudioEngine audio;
olc::ext::Miniaudio::Sound sound[SND_TOTAL];


bool audio_load_file(olc::ext::Miniaudio::Sound &snd, const char *filename, int voices = 1)
{
	if(!audio.CreateSoundFromFile(snd, filename, voices)) {
		std::cout << "Error loading audio file: " << filename << std::endl;
		return false;
	}

	return true;
}

bool audio_load_array(olc::ext::Miniaudio::Sound &snd, unsigned char *data, size_t size, const char *label, int voices = 1)
{
	if(!audio.CreateSoundFromMemory(snd, data, size, voices)) {
		std::cout << "Error loading audio array: " << label << std::endl;
		return false;
	}

	return true;
}

bool audio_init(void)
{
	if(!InstallSystemExtension(&audio))
			return false;

	bool all_ok = true;

	all_ok &= audio_load_array(sound[SND_SHOOT], bin2h::shoot_wav, sizeof(bin2h::shoot_wav), "audio/shoot.wav");
	all_ok &= audio_load_array(sound[SND_HIT], bin2h::hit_wav, sizeof(bin2h::hit_wav), "audio/hit.wav");
	all_ok &= audio_load_array(sound[SND_BOOM], bin2h::boom_wav, sizeof(bin2h::boom_wav), "audio/boom.wav");
	all_ok &= audio_load_array(sound[SND_POWERUP], bin2h::powerup_wav, sizeof(bin2h::powerup_wav), "audio/powerup.wav");
	all_ok &= audio_load_array(sound[SND_SHOOT2], bin2h::shoot2_wav, sizeof(bin2h::shoot2_wav), "audio/shoot2.wav");
	all_ok &= audio_load_array(sound[SND_HIT2], bin2h::hit2_wav, sizeof(bin2h::hit2_wav), "audio/hit2.wav");
	all_ok &= audio_load_array(sound[SND_HIT3], bin2h::hit3_wav, sizeof(bin2h::hit3_wav), "audio/hit3.wav");
	all_ok &= audio_load_array(sound[SND_CHANGE], bin2h::change_wav, sizeof(bin2h::change_wav), "audio/change.wav");
	
	all_ok &= audio_load_array(sound[MUS_GOTHIC], bin2h::gothic_dark_loop_64k_mp3, sizeof(bin2h::gothic_dark_loop_64k_mp3), "audio/gothic_dark_loop_64k.mp3");
	all_ok &= audio_load_array(sound[MUS_HAVOC], bin2h::havoc_loop_64k_mp3, sizeof(bin2h::havoc_loop_64k_mp3), "audio/havoc_loop_64k.mp3");

	//all_ok &= audio_load_file(sound[SND_SHOOT], "audio/shoot.wav");
	//all_ok &= audio_load_file(sound[SND_HIT], "audio/hit.wav");
	//all_ok &= audio_load_file(sound[SND_BOOM], "audio/boom.wav");

	return all_ok;
}

void audio_playpan(int id, float posx, float posy)
{
	float pan = (posx - (float)world_posx) / ((30.0f - posy) * 0.38f) * 0.7f;
	sound[id].SetPan(std::clamp(pan, -1.0f, 1.0f));
	sound[id].Play();
}
