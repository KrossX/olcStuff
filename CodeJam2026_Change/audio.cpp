enum {
	SND_SHOOT = 0,
	SND_HIT,
	SND_BOOM,
	SND_TOTAL
};

olc::ext::Miniaudio::AudioEngine audio;
olc::ext::Miniaudio::Sound sound[SND_TOTAL];

bool audio_init(void)
{
	if(!InstallSystemExtension(&audio))
			return false;


	audio.CreateSoundFromFile(sound[SND_SHOOT], "audio/shoot.wav");
	audio.CreateSoundFromFile(sound[SND_HIT], "audio/hit.wav");
	audio.CreateSoundFromFile(sound[SND_BOOM], "audio/boom.wav");

#if 0
	/**
         * this is here to demonstrate how the adventurous can
         * exploit other features of miniaudio that hasn't been
         * abstracted by the PGEX
         *
         * Here you get a pointer to a next active voice, or the
		 * currently playing voice.
         */
        ma_sound_set_position(song1.GetMASound(), 0.0f, 0.0f, 0.0f);
#endif

	return true;
}

void audio_playpan(int id, float posx, float posy)
{
	float pan = (posx - (float)world_posx) / ((30.0f - posy) * 0.38f) * 0.7f;
	sound[id].SetPan(std::clamp(pan, -1.0f, 1.0f));
	sound[id].Play();
}