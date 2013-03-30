// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4 -*-
#include "../../audio/audio.h"
#include "mrb_eventable.h"
#include "mrb_audio.h"

/**
 * Class: AudioClass
 *
 * The `Audio` singleton, which is the sole instance of `AudioClass`,
 * allows you to interact with SMC’s sound system. You can play any sound
 * from SMC’s sound collection and any music that could also be used as a
 * background music in a level. Namely, paths to sound files are relative
 * to the SMC `sounds/` directory and music paths relative to the SMC
 * `music/` directory. The following table lists some examples (of
 * course you have to adapt the paths to your local setup):
 *
 * |-----------------------+---------------------------------------+--------------------------------------|
 * | SMC installation path | Sound path                            | Music path                           |
 * |-----------------------+---------------------------------------+--------------------------------------|
 * |/usr/local             | /usr/local/share/smc/sounds           | /usr/local/share/smc/music           |
 * |-----------------------+---------------------------------------+--------------------------------------|
 * |C:\Program files\SMC   | C:\Program files\SMC\share\smc\sounds | C:\Program files\SMC\share\smc\music |
 * |-----------------------+---------------------------------------+--------------------------------------|
 *
 * TODO: Check the Windows path.
 *
 * So, if you want to play the star music, you first have to find where
 * the music file is located. So, assuming your SMC is installed at
 * `/usr/local`, you’d find the star music at
 * `/usr/local/share/smc/music/game/star.ogg`. To play it, you’d take
 * the path relative to `/usr/local/share/smc/music/`,
 * i.e. `game/star.ogg`. This is what you pass on to a method such as
 * [play_music](#playmusic):
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ruby
 * Audio.play_music("game/star.ogg")
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note that path elements are always separated with a forward slash /,
 * even if the native path separation on some platforms is another (such
 * as \ on Windows).
*/

using namespace SMC;
using namespace SMC::Scripting;

// Extern
struct RClass* SMC::Scripting::p_rcAudio = NULL;
struct mrb_data_type SMC::Scripting::rtAudio = {"Audio", NULL};

static mrb_value Initialize(mrb_state* p_state,  mrb_value self)
{
	mrb_raise(p_state, MRB_NOTIMP_ERROR(p_state), "Cannot create instances of this class.");
	return self; // Not reached
}

/**
 * Method: AudioClass#play_sound
 *
 *   play_sound( filename [, volume [, loops [, resid ] ] ] ) → a_bool
 *
 * Plays a sound.
 *
 * #### Parameters
 *
 * filename
 * : Path to the sound file to play, relative to the *sounds/*
 *   directory.
 *
 * volume (-1)
 * : Volume the sound shall have. Between 0 and 100.
 *
 * loops (0)
 * : Number of times to repeat the sound _after_ it has been played once,
 *   i.e. the number of sounds played equals `1 + loops`.
 *
 * resid (-1)
 * : Special identifier to prevent a sound from being
 *   played while another instance of this sound is already being
 *   played. SMC ensures that no two sounds with the same resource
 *   ID are played at the same time, i.e. the running sound will
 *   be stopped and discarded before your sound is played. You can
 *   define your own IDs, but there is a number of IDs predefined
 *   by SMC:
 *
 *   1. Maryo jump sound.
 *   2. Maryo wall hit sound
 *   3. Maryo powerdown sound
 *   4. Maryo ball sound
 *   5. Maryo death sound
 *   6. Fireplant, blue mushroom, ghost mushroom and feather
 *      sound
 *   7. 1-Up mushroom and moon sound
 *   8. Maryo Au! (not used currently)
 *   9. Maryo stop sound
 *
 *   Specifying -1 for this parameter allows the given sound to be played
 *   multiple times.
 *
 * #### Return value
 *
 * True on success, false otherwise. Possible failure reasons include
 * incorrect filenames or the sound may simply have been muted by
 * the user in SMC’s preferences, so you probably shouldn’t give
 * too much on this.
 */
static mrb_value Play_Sound(mrb_state* p_state,  mrb_value self)
{
	char* filename = NULL;
	mrb_int volume = -1;
	mrb_int loops = 0;
	mrb_int resid = -1;
	mrb_get_args(p_state,"z|iii", &filename, &volume, &loops, &resid);

	if (pAudio->Play_Sound(filename, resid, volume, loops))
		return mrb_true_value();
	else
		return mrb_false_value();
}

/**
 * Method: AudioClass#play_music
 *
 *   play_music( filename [, loops [, force [, fadein_ms = 0 ] ] ] ) → a_bool
 *
 * Plays a music (optionally replacing the currently played one, if any).
 *
 * #### Parameters
 *
 * filename
 * : Name of the music file to play, relative to the **music/**
 *   directory.
 *
 * loops (0)
 * : Number of times to repeat the music _after_ it has been played once,
 *   i.e. the number of musics played equals `1 + loops`.
 *
 * force (true)
 * : Enforces the new music to run even if another music
 *   is already running (the running music is stopped and discarded).
 *   Note this behaviour is enabled by default, you have to explicitely
 *   set this to `false`.
 *
 * fadein_ms (0)
 * : Number of milliseconds to fade the music in.
 *
 * #### Return value
 *
 * True on success, false otherwise. Possible failure reasons include
 * incorrect filenames or the music may simply have been muted by
 * the user in SMC’s preferences, so you probably shouldn’t give
 * too much on this.
 */
static mrb_value Play_Music(mrb_state* p_state,  mrb_value self)
{
	char* filename = NULL;
	mrb_int loops = 0;
	mrb_value force = mrb_true_value();
	mrb_int fadein_ms = 0;

	mrb_get_args(p_state, "z|ioi", &filename, &loops, &force, &fadein_ms);

	if (pAudio->Play_Music(filename, loops, mrb_test(force), fadein_ms))
		return mrb_true_value();
	else
		return mrb_false_value();
}



void SMC::Scripting::Init_Audio(mrb_state* p_state)
{
	p_rcAudio = mrb_define_class(p_state, "AudioClass", p_state->object_class);
	mrb_include_module(p_state, p_rcAudio, p_rmEventable);
	MRB_SET_INSTANCE_TT(p_rcAudio, MRB_TT_DATA);

	// Make the Audio constant the only instance of AudioClass
	mrb_define_const(p_state, p_state->object_class, "Audio", pAudio->Create_MRuby_Object(p_state));

	mrb_define_method(p_state, p_rcAudio, "initialize", Initialize, ARGS_NONE());
	mrb_define_method(p_state, p_rcAudio, "play_sound", Play_Sound, ARGS_REQ(1) | ARGS_OPT(3));
	mrb_define_method(p_state, p_rcAudio, "play_music", Play_Music, ARGS_REQ(1) | ARGS_OPT(3));
}