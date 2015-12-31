/*
 * Copyright (c) 2015
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
 *
 *----------------------------------------------------------------------------
 * This custom object defines a geometry object that has parameters which
 * define a frame-dependent jitter in its translation. This can be used to make
 * earth-quake type effects. No doubt next year will be the year of the
 * Earthquake movie.
 */

#ifndef __OBJ_Shake__
#define __OBJ_Shake__

#include <OBJ/OBJ_Geometry.h>

namespace HDK_Sample {

class OBJ_Shake : public OBJ_Geometry
{
public:
				 OBJ_Shake(OP_Network *net,
					   const char *name,
					   OP_Operator *op);
    virtual			~OBJ_Shake();

    static OP_Node              *myConstructor(OP_Network *net,
					       const char *name,
					       OP_Operator *entry);

    static OP_TemplatePair	*buildTemplatePair(OP_TemplatePair *prevstuff);

    fpreal  JX(fpreal t)	{ return evalFloat("jitter", &shakeIndirect[0], 0, t); }
    fpreal  JY(fpreal t)	{ return evalFloat("jitter", &shakeIndirect[0], 1, t); }
    fpreal  JZ(fpreal t)	{ return evalFloat("jitter", &shakeIndirect[0], 2, t); }

protected:
    virtual int			 applyInputIndependentTransform(
				    OP_Context &context, UT_DMatrix4 &mat);

private:
    static int			*shakeIndirect;
};

}	// End HDK_Sample namespace

#endif
