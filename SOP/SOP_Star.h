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
 *
 *----------------------------------------------------------------------------
 * This SOP builds a star.
 */

#ifndef __SOP_Star_h__
#define __SOP_Star_h__

#include <SOP/SOP_Node.h>

namespace HDK_Sample {
class SOP_Star : public SOP_Node
{
public:
    static OP_Node		*myConstructor(OP_Network*, const char *,
							    OP_Operator *);

    /// Stores the description of the interface of the SOP in Houdini.
    /// Each parm template refers to a parameter.
    static PRM_Template		 myTemplateList[];

    /// This optional data stores the list of local variables.
    static CH_LocalVariable	 myVariables[];

protected:
	     SOP_Star(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_Star();

    /// Disable parameters according to other parameters.
    virtual unsigned		 disableParms();


    /// cookMySop does the actual work of the SOP computing, in this
    /// case, a star shape.
    virtual OP_ERROR		 cookMySop(OP_Context &context);

    /// This function is used to lookup local variables that you have
    /// defined specific to your SOP.
    virtual bool		 evalVariableValue(
				    fpreal &val,
				    int index,
				    int thread);
    // Add virtual overload that delegates to the super class to avoid
    // shadow warnings.
    virtual bool		 evalVariableValue(
				    UT_String &v,
				    int i,
				    int thread)
				 {
				     return evalVariableValue(v, i, thread);
				 }

private:
    /// The following list of accessors simplify evaluating the parameters
    /// of the SOP.
    int		DIVISIONS(fpreal t)	{ return evalInt  ("divs", 0, t); }
    fpreal	XRADIUS(fpreal t)	{ return evalFloat("rad", 0, t); }
    fpreal	YRADIUS(fpreal t)	{ return evalFloat("rad", 1, t); }
    int		NEGRADIUS()		{ return evalInt  ("nradius", 0, 0); }
    fpreal	CENTERX(fpreal t) 	{ return evalFloat("t", 0, t); }
    fpreal	CENTERY(fpreal t) 	{ return evalFloat("t", 1, t); }
    fpreal	CENTERZ(fpreal t) 	{ return evalFloat("t", 2, t); }
    int		ORIENT()		{ return evalInt  ("orient", 0, 0); }

    /// Member variables are stored in the actual SOP, not with the geometry
    /// In this case these are just used to transfer data to the local 
    /// variable callback.
    /// Another use for local data is a cache to store expensive calculations.
    int		myCurrPoint;
    int		myTotalPoints;
};
} // End HDK_Sample namespace

#endif
