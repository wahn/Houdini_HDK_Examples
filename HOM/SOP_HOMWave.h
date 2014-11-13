/*
 * Copyright (c) 2014
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
 * This SOP provides an example of how to access HOM from C++.
 */

#ifndef __SOP_HOMWave_h__
#define __SOP_HOMWave_h__

#include <SOP/SOP_HOM.h>

namespace HDK_Sample {

/// Example of using the C++ interface to HOM
/// @see @ref HOM/SOP_HOMWave.py SOP_CPPWave, vex_wave()
class SOP_HOMWave : public SOP_HOM
{
public:
    static OP_Node *myConstructor(
	OP_Network *network, const char *name, OP_Operator *op_type);

    static PRM_Template myTemplateList[];

private:
    SOP_HOMWave(
	OP_Network *network, const char *name, OP_Operator *script_op_type);

    // Override this method to provide the algorithm for the cook.
    virtual void cookWithHOM();
};

}	// End HDK_Sample namespace

#endif
