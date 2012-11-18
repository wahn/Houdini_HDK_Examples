/*
* Copyright (c) 2012
*	Side Effects Software Inc.  All rights reserved.
*
* Redistribution and use of Houdini Development Kit samples in source and
* binary forms, with or without modification, are permitted provided that the
* following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. The name of Side Effects Software may not be used to endorse or
*    promote products derived from this software without specific prior
*    written permission.
*
* THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
* NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __ROP_Dumper_h__
#define __ROP_Dumper_h__

#include <ROP/ROP_Node.h>

#define STR_PARM(name, idx, vi, t) \
		{ evalString(str, name, &ifdIndirect[idx], vi, t); }
#define INT_PARM(name, idx, vi, t) \
                { return evalInt(name, &ifdIndirect[idx], vi, t); }

#define STR_SET(name, idx, vi, t) \
		{ setString(str, name, ifdIndirect[idx], vi, t); }
#define STR_GET(name, idx, vi, t) \
		{ evalStringRaw(str, name, &ifdIndirect[idx], vi, t); }

class OP_TemplatePair;
class OP_VariablePair;

namespace HDK_Sample {

class ROP_Dumper : public ROP_Node {
public:

    /// Provides access to our parm templates.
    static OP_TemplatePair	*getTemplatePair();
    /// Provides access to our variables.
    static OP_VariablePair	*getVariablePair();
    /// Creates an instance of this node.
    static OP_Node		*myConstructor(OP_Network *net, const char*name,
						OP_Operator *op);

protected:
	     ROP_Dumper(OP_Network *net, const char *name, OP_Operator *entry);
    virtual ~ROP_Dumper();

    /// Called at the beginning of rendering to perform any intialization 
    /// necessary.
    /// @param	nframes	    Number of frames being rendered.
    /// @param	s	    Start time, in seconds.
    /// @param	e	    End time, in seconds.
    /// @return		    True of success, false on failure (aborts the render).
    virtual int			 startRender(int nframes, fpreal s, fpreal e);

    /// Called once for every frame that is rendered.
    /// @param	time	    The time to render at.
    /// @param	boss	    Interrupt handler.
    /// @return		    Return a status code indicating whether to abort the
    ///			    render, continue, or retry the current frame.
    virtual ROP_RENDER_CODE	 renderFrame(fpreal time, UT_Interrupt *boss);

    /// Called after the rendering is done to perform any post-rendering steps
    /// required.
    /// @return		    Return a status code indicating whether to abort the
    ///			    render, continue, or retry.
    virtual ROP_RENDER_CODE	 endRender();

public:

    /// A convenience method to evaluate our custom file parameter.
    void  OUTPUT(UT_String &str, fpreal t)
    { STR_PARM("file",  0, 0, t) }

private:
    static int		*ifdIndirect;
    fpreal		 myEndTime;
};

}	// End HDK_Sample namespace


#undef STR_PARM
#undef STR_SET
#undef STR_GET

#endif
