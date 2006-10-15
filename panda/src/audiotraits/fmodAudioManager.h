// Filename: fmodAudioManager.h
// Created by:  cort (January 22, 2003)
// Extended by: ben  (October 22, 2003)
// Prior system by: cary
// Rewrite [for new Version of FMOD-EX] by: Stan Rosenbaum "Staque" - Spring 2006
//
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////
//
//Hello, all future Panda audio code people! This is my errata documentation to
//help any future programmer maintain FMOD and PANDA.
//
//This documentation more then that is needed, but I wanted to go all out, with
//the documentation. Because I was a totally newbie at programming [especially
//with C/C++] this semester I want to make sure future code maintainers have that
//insight I did not when starting on the PANDA project here at the ETC/CMU.
//
//As of Spring 2006, Panda's FMOD audio support has been pretty much completely
//rewritten. This has been done so PANDA can use FMOD-EX [or AKA FMOD 4] and some
//of its new features.
//
//First, the FMOD-EX API itself has been completely rewritten compared to previous
//versions. FMOD now handles any type of audio files, wave audio [WAV, AIF, MP3,
//OGG, etc...] or musical file [MID, TRACKERS] as the same type of an object. The
//API has also been structured more like a sound studio, with 'sounds' and
//'channels'. This will be covered more in the FmodAudioSound.h/.cxx sources.
//
//Second, FMOD now offers virtually unlimited sounds to be played at once via
//their virtual channels system. Actually the theoretical limit is around 4000,
//but that is still a lot. What you need to know about this, is that even thought
//you might only hear 32 sound being played at once, FMOD will keep playing any
//additional sounds, and swap those on virtual channels in and out with those on real channels
//depending on priority, or distance [if you are dealing with 3D audio].
//
//Third, FMOD's DSP support has been added. So you can now add GLOBAL or SOUND
//specific DSP effects. Right not you can only use FMOD's built in DSP effects.
//But adding support for FMOD's support of VST effects shouldn't be that hard.
//
//As for the FmodManager itself, it is pretty straight forward, I hope. As a
//manager class, it will create the FMOD system with the �_system� variable which
//is an instance of FMOD::SYSTEM. This class is also the one responsible for
//creation of Sounds, DSP, and maintaining the GLOBAL DSP chains [The GLOBAL DSP
//chain is the DSP Chain which affects ALL the sounds].
//
//Any way that is it for an intro, lets move on to looking at the rest of the
//code.
//
////////////////////////////////////////////////////////////////////


#ifndef __FMOD_AUDIO_MANAGER_H__
#define __FMOD_AUDIO_MANAGER_H__

//First the includes.
#include "pandabase.h"
#include "pset.h"

#ifdef HAVE_FMODEX //[

#include "audioManager.h"

//The Includes needed for FMOD
#include <fmod.hpp>
#include <fmod_errors.h>

class FmodAudioSound;
class FmodAudioDSP;

extern void fmod_audio_errcheck(FMOD_RESULT n);

class EXPCL_FMOD_AUDIO FmodAudioManager : public AudioManager {
  friend class FmodAudioSound;
  friend class FmodAudioDSP;

 public:

  //Constructor and Destructor
  FmodAudioManager();
  virtual ~FmodAudioManager();

  virtual bool is_valid();
          
  virtual PT(AudioSound) get_sound(const string&, bool positional = false);
    
  virtual PT(AudioDSP) create_dsp(DSP_category);
  virtual bool add_dsp(PT(AudioDSP) dspToAdd);
  virtual bool remove_dsp(PT(AudioDSP) x);

  virtual int getSpeakerSetup();
  virtual void setSpeakerSetup(SpeakerModeCategory cat);

  virtual void set_volume(float);
  virtual float get_volume() const;
          
  virtual void set_active(bool);
  virtual bool get_active() const;

  virtual void stop_all_sounds();

  virtual void update();
  
  // This controls the "set of ears" that listens to 3D spacialized sound
  // px, py, pz are position coordinates. Can be 0.0f to ignore.
  // vx, vy, vz are a velocity vector in UNITS PER SECOND (default: meters).
  // fx, fy and fz are the respective components of a unit forward-vector
  // ux, uy and uz are the respective components of a unit up-vector
  // These changes will NOT be invoked until audio_3d_update() is called.
  virtual void audio_3d_set_listener_attributes(float px, float py, float pz,
                                                float vx, float xy, float xz, 
                                                float fx, float fy, float fz,
                                                float ux, float uy, float uz);

  // REMOVE THIS ONE
  virtual void audio_3d_get_listener_attributes(float *px, float *py, float *pz,
                                                float *vx, float *vy, float *vz,
                                                float *fx, float *fy, float *fz,
                                                float *ux, float *uy, float *uz);
          
  // Control the "relative distance factor" for 3D spacialized audio. Default is 1.0
  // Fmod uses meters internally, so give a float in Units-per meter
  // Don't know what Miles uses.
  virtual void audio_3d_set_distance_factor(float factor);
  virtual float audio_3d_get_distance_factor() const;

  // Control the presence of the Doppler effect. Default is 1.0
  // Exaggerated Doppler, use >1.0
  // Diminshed Doppler, use <1.0
  virtual void audio_3d_set_doppler_factor(float factor);
  virtual float audio_3d_get_doppler_factor() const;

  // Exaggerate or diminish the effect of distance on sound. Default is 1.0
  // Faster drop off, use >1.0
  // Slower drop off, use <1.0
  virtual void audio_3d_set_drop_off_factor(float factor);
  virtual float audio_3d_get_drop_off_factor() const;

  //THESE ARE NOT USED ANYMORE.
  //THEY ARE ONLY HERE BECAUSE THEY are still needed by Miles.
  //THESE are stubs in FMOD-EX version
  ////////////////////////////////////////////////////////////////////
  virtual void set_concurrent_sound_limit(unsigned int limit = 0);
  virtual unsigned int get_concurrent_sound_limit() const;
  virtual void reduce_sounds_playing_to(unsigned int count);
  virtual void uncache_sound(const string&);
  virtual void clear_cache();
  virtual void set_cache_limit(unsigned int count);
  virtual unsigned int get_cache_limit() const;
  ////////////////////////////////////////////////////////////////////

 private:

  FMOD::System *_system; 
  
  static pset<FmodAudioManager *> _all_managers;

  FMOD_VECTOR _position;
  FMOD_VECTOR _velocity;
  FMOD_VECTOR _forward;
  FMOD_VECTOR _up;

  bool _is_valid;
  bool _active;

  float _distance_factor;
  float _doppler_factor;
  float _drop_off_factor;

  // The set of all sounds.  Needed only to implement stop_all_sounds.
  typedef pset<FmodAudioSound *> SoundSet;
  SoundSet _all_sounds;

  // The Data Structure that holds all the DSPs.
  typedef pset<PT (FmodAudioDSP) > DSPSet;
  DSPSet _system_dsp;

  friend class FmodAudioSound;

  ////////////////////////////////////////////////////////////
  //These are needed for Panda's Pointer System. DO NOT ERASE!
  ////////////////////////////////////////////////////////////

 public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    AudioManager::init_type();
    register_type(_type_handle, "FmodAudioManager", AudioManager::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }
  virtual TypeHandle force_init_type() {
    init_type(); 
    return get_class_type();
  }

 private:
  static TypeHandle _type_handle;

  ////////////////////////////////////////////////////////////
  //DONE
  ////////////////////////////////////////////////////////////

};

EXPCL_FMOD_AUDIO PT(AudioManager) Create_AudioManager();


#endif //]

#endif /* __FMOD_AUDIO_MANAGER_H__ */
