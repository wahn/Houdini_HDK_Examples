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
 *
 * NOTE:
 *     See SOP_HOMWave.py for a pure Python implementation of this SOP.
 */

/// This SOP provides an example of how to access HOM from C++.  See
/// SOP_HOMWave.py and related files for a pure Python implementation of this
/// SOP.
/// @see @ref HOM/SOP_HOMWave.py, @ref HOM/SOP_HOMWaveNumpy.py, @ref HOM/SOP_HOMWaveInlinecpp.py, @ref SOP/SOP_CPPWave.C, @ref SOP/SOP_VEXWave.vfl

#include <UT/UT_DSOVersion.h>
#include "SOP_HOMWave.h"
#include <OP/OP_OperatorTable.h>
#include <HOM/HOM_Module.h>
#include <HOM/HOM_SopNode.h>
#include <HOM/HOM_Geometry.h>
#include <HOM/HOM_Vector3.h>
#include <HOM/HOM_Point.h>
#include <HOM/HOM_NodeType.h>
#include <boost/shared_ptr.hpp>

using namespace HDK_Sample;

void
newSopOperator(OP_OperatorTable *table)
{
     table->addOperator(new OP_Operator(
	"hom_wave", "HOM Wave",
	&SOP_HOMWave::myConstructor, SOP_HOMWave::myTemplateList,
	/*min_num_inputs=*/1, /*max_num_inputs=*/1));
}

OP_Node *
SOP_HOMWave::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_HOMWave(net, name, op);
}

// List any of the sop's parameters here:
PRM_Template
SOP_HOMWave::myTemplateList[] = {
    PRM_Template(),
};

SOP_HOMWave::SOP_HOMWave(
	OP_Network *network, const char *name, OP_Operator *op_type)
    : SOP_HOM(network, name, op_type)
{
}

void
SOP_HOMWave::cookWithHOM()
{
    HOM_Module &hou = HOM();

    // This C++ code does the equivalent of this Python code:
    //
    // import math
    // geo = hou.pwd().geometry()
    // f = hou.frame() * 0.03
    // for p in geo.points():
    //     pos = p.position()
    //     pos[1] = math.sin(pos[0] * 0.2 + pos[2] * 0.3 + f)
    //     p.setPosition(pos)
    //
    // It modifies the input geometry's y coordinate to create an animated
    // sin wave.  Connect this sop to the output of a grid and press play.

    boost::shared_ptr<HOM_Geometry> geo(HOMdel(
	dynamic_cast<HOM_SopNode*>(hou.pwd()))->geometry());

    double f = hou.frame() * 0.03;
    std::vector<HOM_ElemPtr<HOM_Point> > points = geo->points();
    std::vector<double> pos_vector(3);
    for (int i=0; i<points.size(); ++i)
    {
	boost::shared_ptr<HOM_Point> p(points[i].myPointer);

	boost::shared_ptr<HOM_Vector3> pos_ptr(p->position());
	HOM_Vector3 &pos = *pos_ptr;
	pos[1] = sin(pos[0] * 0.2 + pos[2] * 0.3 + f);

	for (int i=0; i<3; ++i)
            pos_vector[i] = pos[i];
	p->setPosition(pos_vector);
    }
}

