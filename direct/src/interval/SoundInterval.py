"""SoundInterval module: contains the SoundInterval class"""

from PandaModules import *

import Interval
import WaitInterval

class SoundInterval(Interval.Interval):

    # special methods
    
    def __init__(self, name, sound, loop=0):
        """__init__(name, sound, loop)
        """
	self.name = name
	self.sound = sound
	self.duration = self.sound.length() 
	if (self.duration == 0.0):
	    Interval.Interval.notify.warning(
		'SoundInterval(): zero length sound - setting duration = 1.0')
	self.duration = 1.0
	self.loop = loop
	self.isPlaying = 0

    def setT(self, t, entry=0):
	""" setT(t)
	    Go to time t
	"""
	if (t > self.duration):
	    if (self.isPlaying == 1):
		AudioManager.stop(self.sound)
		self.isPlaying = 0
	    return
	assert(t >= 0)
	if (entry == 1):
	    self.isPlaying = 1
	    AudioManager.play(self.sound)
	    if (self.loop):
		AudioManager.setLoop(self.sound, 1)
