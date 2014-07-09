#pragma once

namespace audio {


	struct sfx {
		sfx() : _chunk(0) {
		}

		~sfx() {
			if (_chunk)
				Mix_FreeChunk(_chunk);
		}

		void load(const char* filename) {
			_chunk = Mix_LoadWAV(filename);
		}

		void play(int channel) {
			Mix_PlayChannel(channel, _chunk, 0);
		}

		Mix_Chunk* _chunk;
	};


	struct music {
		music() : _music(0) {
		}

		~music() {
			if (_music)
				Mix_FreeMusic(_music);
		}

		void load(const char* filename) {
			_music = Mix_LoadMUS(filename);
		}

		void play() {
			Mix_FadeInMusic(_music, -1, 200);
		}

		Mix_Music* _music;
	};


}
