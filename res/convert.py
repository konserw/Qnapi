# -*- coding: utf-8 -*-
"""
copyright (c) 2013 by Kamil Strzempowicz
"""
"""
import sys
sys.path.append("/home/konserw/git/Qnapi/"
"""

import aeidon
from aeidon import *
from aeidon import Project

project = aeidon.Project()
project.open_main("%1", "cp1250")
project.set_framerate(aeidon.framerates.%3)
project.save_main(aeidon.files.new(aeidon.formats.SUBRIP,"%2","utf_8"))