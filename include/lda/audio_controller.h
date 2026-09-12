#pragma once

#include "sound.h"
#include "music.h"
#include "audio_channel.h"
#include "real_audio_channel.h"
#include "audio_callback_interface.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <iostream>
#include <map>
#include <string>

namespace lda
{

//!Configuration to instruct SDL2 on how to setup the audio framework.

//!Refer to SDL2 reference for further information on their meaning.

struct audio_controller_config {
	int     ratio, 	/**< A sane default is 44100. */
	        out,	/**< A sane default is 2. */
	        buffers,	/**< A sane default is 1024. */
	        channels,	/**< A sane default is 8... like Doom. */
	        initial_sound_volume, /**< A sane default is 128. */
	        initial_music_volume; //!< Defines the initial volume for the music. Sane default values could be 96-128.
	Uint16  format; /**< A sane default is SDL2 value AUDIO_S16SYS. */
};

class audio_controller;

//!Non singleton audio controller.

//!Unlike the video elements - which need to be loaded, queued and refreshed
//!in sequence - the audio_controller is interactive: each action takes place
//!instantly.
//!Only one instance can exist for each application. Provides access to all
//!sound channels, music, volumes and debug information.

class audio_controller
{
	public:

	                        audio_controller(const audio_controller_config&);
	                        ~audio_controller();

	void                    play_sound(const sound_struct&);
	void                    stop_sound(int);
	void                    stop_sound(); //Stops all sounds
	void                    pause_sound(); //Pauses all sounds.
	void                    resume_sound(); //Restarts all sounds.
	void                    play_music(const music_struct&, int=-1);
	void                    stop_music(int =0);
	//!Main gain for all channels. This is meant to be the factor that the end
	//!user adjusts.
	void                    set_main_sound_volume(int);
	void                    set_sound_volume(int p_vol, int pchannel); //p_vol de 0 a 128.
	void                    add_music_stop_callback(const std::string&, audio_callback_interface&);
	void                    remove_music_stop_callback(const std::string&);
	void                    pause_music();
	void                    resume_music();

	//!Sets the "mix" of the volume for the application. This is not meant to
	//!be adjusted by the user, but used as an app-wide switch meant to control
	//!how loud music is based in the app resources themselves.
	void                    set_main_music_volume(int v);
	//!Sets the user music volume. This is meant to be the factor that the 
	//!end user adjusts when they go into some menu and set "music volume" to
	//!some value.
	void                    set_music_volume(int);
	//!Indicates whether music is playing.
	bool                    is_music_playing() const {return music_playing;}
	int                     get_music_volume() const;
	//!Returns the main music volume.
	int                     get_main_music_volume() const {return main_music_volume;}
	//!Returns the number of channels requested to SDL by the controller.
	int                     get_requested_channels() const {return requested_channels;}
	//!Gets the main sound volume.
	unsigned int            get_main_sound_volume() const {return main_sound_volume;}
	audio_channel           get_channel(int);
	audio_channel           get_free_channel();
	//!Returns true if any channel is not playing and unmonitored.
	bool                    has_free_channels() const;

	std::string             debug_state();

	/////////////////////////////////////
	// Propiedades...

	private:

	//!Can't be copied.
	                        audio_controller(const audio_controller&)=delete;
	//!Can't be assigned.
	                        audio_controller& operator=(const audio_controller&)=delete;

	int                     get_free_channel_index(int=0, int=-1);
	void                    check_channel(int);


	int                     main_sound_volume,
	                        main_music_volume,
	                        current_music_volume, //!Auxiliary value.
	                        ratio,
	                        out,	//1 mono, 2 estéreo.
	                        buffers,
	                        requested_channels;

	Uint16                  format;

	bool                    music_playing{false},
	                        music_paused{false};

	std::vector<real_audio_channel> channels;
	std::map<std::string, audio_callback_interface *> music_stop_callbacks;

	static audio_controller * sdl_callback_controller_ptr;
};

}
