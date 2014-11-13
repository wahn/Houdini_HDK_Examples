#
# Copyright (c) 2014
#	Side Effects Software Inc.  All rights reserved.
#
# Redistribution and use of Houdini Development Kit samples in source and
# binary forms, with or without modification, are permitted provided that the
# following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. The name of Side Effects Software may not be used to endorse or
#    promote products derived from this software without specific prior
#    written permission.
#
# THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
# NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#----------------------------------------------------------------------------
# This SOP provides an example of a SOP implemented in Python.
#

"""This is the Python equivalent of SOP_HOMWave.C which uses the C++ API.

To use this code,
    1) In Houdini, choose File -> New Operator Type
    2) Choose "Python Type"
    3) Choose the network type as "Geometry Operator"
    4) Paste this code in the "Code" tab of the type properties.

@see @ref HOM/SOP_HOMWaveNumpy.py, @ref HOM/SOP_HOMWaveInlinecpp.py, @ref HOM/SOP_HOMWave.C, @ref SOP/SOP_CPPWave.C, @ref SOP/SOP_VEXWave.vfl
"""

import math

geo = hou.pwd().geometry()
f = hou.frame() * 0.03
for p in geo.points():
    pos = p.position()
    pos[1] = math.sin(pos[0] * 0.2 + pos[2] * 0.3 + f)
    p.setPosition(pos)

