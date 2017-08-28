#include "real_audio_channel.h"

#include "../audio_controller/audio_controller.h"

using namespace lda;

real_audio_channel::real_audio_channel(int i, const audio_controller& ac)
	:index(i), repeat(0), volume(128), playing(false), busy(false), 
	monitoring(false), paused(false), controller_ref(&ac),
	sound_playing(nullptr) 
{

}


real_audio_channel::real_audio_channel(const real_audio_channel& o)
	:index(o.index), repeat(o.repeat), volume(o.volume), 
	playing(o.playing), busy(o.busy), monitoring(o.monitoring),
	paused(o.paused), controller_ref(o.controller_ref),
	sound_playing(o.sound_playing)
{

}

real_audio_channel& real_audio_channel::operator=(const real_audio_channel& o)
{
	index=o.index;
	repeat=o.repeat;
	volume=o.volume;
	playing=o.playing;
	busy=o.busy;
	monitoring=o.monitoring;
	paused=o.paused;
	controller_ref=o.controller_ref;
	sound_playing=o.sound_playing;

	return *this;
}

//!Frees a channel after the playback is done. 

//!This is part of the SDL_Audio callback system, which needs a callback 
//!function to call after a sound has been played. Must be manually called on 
//!monitored channels. Nullifies repeat, sets busy and monitoring to false and 
//!cleans pointer to played sound. The callback listener is also removed and
//!the panning is cleared.

void real_audio_channel::free()
{
	sound_playing=nullptr;
	repeat=0;
	busy=false;
	monitoring=false;
	callback_listener=nullptr;
	clear_panning();
}

//!Forces sound stop.

//!Forces sound stop. Internally calls Mix_HaltChannel, sets playing to false
//!and calls "free" if the channel is unmonitored.

void real_audio_channel::stop()
{
	Mix_HaltChannel(index);
	playing=false;
	if(!monitoring) free();
}

//!Clears the "playing" flag and calls free if not monitored. 

//!This is part of SDL_Audio callback system.

void real_audio_channel::do_callback()
{
	playing=false;
	if(callback_listener) callback_listener->do_callback();
	if(!monitoring) free();
}

//!Sets the volume for the channel. 

//!The volume for the channel is mixed with the overall master volume set on the 
//!controller. Maximum loudness is achieved when both are set at their maximum values.

void real_audio_channel::set_volume(int v)
{
	volume=v;
	int overall_volume=controller_ref->get_main_sound_volume();
	Mix_Volume(index, (float) volume * ((float)volume / (float)overall_volume));
}

//!Plays the sound_struct. 

//!A bug in the SDL Audio library (or in my code) prevents the panning from 
//!working on Windows builds.

int real_audio_channel::play(const sound_struct& e)
{
	int overall_volume=controller_ref->get_main_sound_volume();
	sound_playing=e.sound_ptr;
	repeat=e.repeat;
	
	if(e.volume!=-1) volume=e.volume;
	Mix_Volume(index, (float) volume * ((float)volume / (float)overall_volume));

	playing=true;

	int res=!e.ms_fade ? Mix_PlayChannel(index, (*sound_playing).get_data(), repeat)
		: Mix_FadeInChannel(index, (*sound_playing).get_data(), repeat, e.ms_fade);

//#ifndef WINCOMPIL
	if(e.panning.left!=-1 && e.panning.right!=-1) 
	{
		set_stereo_volume(e.panning);
	}
//#endif

	return res;
}

//!Sets the left and right channel volumes..

void real_audio_channel::set_stereo_volume(sound_panning sp)
{
	Mix_SetPanning(index, sp.left, sp.right);
}

//!Removes panning from the channel.

void real_audio_channel::clear_panning()
{
	Mix_SetPanning(index, 255, 255);
}

//!Pauses the sound. 

//!Internally calls Mix_Pause for the channel and sets the paused flag to true.

void real_audio_channel::pause()
{
	Mix_Pause(index);
	paused=true;
}

//!Resumes playing after pausing. 

//!Internally calls Mix_Resume for the channel and unsets the pause flag.

void real_audio_channel::resume()
{
	Mix_Resume(index);
	paused=false;
}
