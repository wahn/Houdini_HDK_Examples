/*
 * Copyright (c) 2011
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
 * The Star SOP
 */

#include <limits.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Math.h>
#include <UT/UT_Interrupt.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <CH/CH_LocalVariable.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include "SOP_Star.h"

using namespace HDK_Sample;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/star.txt
//
// See the sample_install.sh file for an example.
//


///
/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case we add ourselves
/// to the specified operator table.
///
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(
	    new OP_Operator("hdk_star",			// Internal name
			    "Star",			// UI name
			     SOP_Star::myConstructor,	// How to build the SOP
			     SOP_Star::myTemplateList,	// My parameters
			     0,				// Min # of sources
			     0,				// Max # of sources
			     SOP_Star::myVariables,	// Local variables
			     OP_FLAG_GENERATOR)		// Flag it as generator
	    );
}

static PRM_Name		negativeName("nradius", "Negative Radius");
//				     ^^^^^^^^    ^^^^^^^^^^^^^^^
//				     internal    descriptive version

static PRM_Default	fiveDefault(5);		// Default to 5 divisions
static PRM_Default	radiiDefaults[] = {
			   PRM_Default(1),		// Outside radius
			   PRM_Default(0.3)		// Inside radius
			};

PRM_Template
SOP_Star::myTemplateList[] = {
    PRM_Template(PRM_INT,			// Integer parameter.
		 PRM_Template::PRM_EXPORT_TBX,	// Export to top of viewer
						// when user selects this node
		 1, 		// One integer in this row/parameter
		 &PRMdivName,   // Name of this parameter - must be static
		 &fiveDefault,  // Default for this parameter - ditto
		 0,		// Menu for this parameter
		 &PRMdivision2Range // Valid range
		 ),
    PRM_Template(PRM_XYZ,	2, &PRMradiusName, radiiDefaults),
    PRM_Template(PRM_TOGGLE,	1, &negativeName),
    PRM_Template(PRM_XYZ,       3, &PRMcenterName),
    PRM_Template(PRM_ORD,	1, &PRMorientName, 0, &PRMplaneMenu),
    PRM_Template()
};


// Here's how we define local variables for the SOP.
enum {
	VAR_PT,		// Point number of the star
	VAR_NPT		// Number of points in the star
};

CH_LocalVariable
SOP_Star::myVariables[] = {
    { "PT",	VAR_PT, 0 },		// The table provides a mapping
    { "NPT",	VAR_NPT, 0 },		// from text string to integer token
    { 0, 0, 0 },
};

float
SOP_Star::getVariableValue(int index, int thread)
{
    // Note that "gdp" may be null here, so we do the safe thing
    // and cache values we are interested in.
    if (myCurrPoint < 0) return 0;
    switch (index)
    {
	case VAR_PT:	return myCurrPoint;
	case VAR_NPT:	return myTotalPoints;
    }
    // Not one of our variables, must delegate to the base class.
    return SOP_Node::getVariableValue(index, thread);
}

OP_Node *
SOP_Star::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Star(net, name, op);
}

SOP_Star::SOP_Star(OP_Network *net, const char *name, OP_Operator *op)
	: SOP_Node(net, name, op)
{
    myCurrPoint = -1;	// To prevent garbage values from being returned
}

SOP_Star::~SOP_Star() {}

unsigned
SOP_Star::disableParms()
{
    return 0;
}

OP_ERROR
SOP_Star::cookMySop(OP_Context &context)
{
    double		 now;
    float		 rad, tx, ty, tz;
    int			 divisions, plane, negradius;
    int			 xcoord, ycoord, zcoord;
    float		 tmp, tinc;
    GEO_Point		*ppt;
    GU_PrimPoly		*poly;
    int			 i;
    UT_Interrupt	*boss;

    now = context.getTime();

    // Since we don't have inputs, we don't need to lock them.

    divisions  = DIVISIONS(now)*2;	// We need twice our divisions of points
    myTotalPoints = divisions;		// Set the NPT local variable value
    myCurrPoint   = 0;			// Initialize the PT local variable

    plane      = ORIENT();
    negradius  = NEGRADIUS();
    tx	       = CENTERX(now);
    ty	       = CENTERY(now);
    tz	       = CENTERZ(now);

    switch (plane)
    {
	case 0:		// XY Plane
		xcoord = 0;
		ycoord = 1;
		zcoord = 2;
		break;
	case 1:		// YZ Plane
		xcoord = 1;
		ycoord = 2;
		zcoord = 0;
		break;
	case 2:		// XZ Plane
		xcoord = 0;
		ycoord = 2;
		zcoord = 1;
		break;
    }

    // Check to see that there hasn't been a critical error in cooking the SOP.
    if (error() < UT_ERROR_ABORT)
    {
	boss = UTgetInterrupt();
	if (divisions < 4)
	{
	    // With the range restriction we have on the divisions, this
	    //	is actually impossible, but it shows how to add an error
	    //	message or warning to the SOP.
	    addWarning(SOP_MESSAGE, "Invalid divisions");
	    divisions = 4;
	}
	gdp->clearAndDestroy();

	// Start the interrupt server
	if (boss->opStart("Building Star"))
	{
	    // Build a polygon
	    poly = GU_PrimPoly::build(gdp, divisions, GU_POLY_CLOSED);
	    tinc = M_PI*2 / (float)divisions;

	    // Now, set all the points of the polygon
	    for (i = 0; i < divisions; i++)
	    {
		// Check to see if the user has interrupted us...
		if (boss->opInterrupt())
		    break;

		myCurrPoint = i;

		// Since we expect the local variables to be used in specifying
		// the radii, we have to evaluate the channels INSIDE the loop
		// through the points...

		tmp = (float)i * tinc;
		rad = (i & 1) ? XRADIUS(now) : YRADIUS(now);
		if (!negradius && rad < 0)
		    rad = 0;

		ppt = poly->getVertex(i).getPt();
		ppt->getPos()(xcoord) = cos(tmp) * rad + tx;
		ppt->getPos()(ycoord) = sin(tmp) * rad + ty;
		ppt->getPos()(zcoord) = 0 + tz;
	    }

	    // Highlight the star which we have just generated.  This routine
	    // call clears any currently highlighted geometry, and then it
	    // highlights every primitive for this SOP. 
	    select(GU_SPrimitive);
	}

	// Tell the interrupt server that we've completed. Must do this
	// regardless of what opStart() returns.
	boss->opEnd();
    }

    // Notify the display cache that we have directly edited
    // points, this lets NURBs surfaces know that they need to rebuild,
    // for example.
    // In this case it isn't needed, but included in case this
    // code is reused for a more complicated example.
    gdp->notifyCache(GU_CACHE_ALL);

    myCurrPoint = -1;
    return error();
}
