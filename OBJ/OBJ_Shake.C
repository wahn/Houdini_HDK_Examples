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

#include <SYS/SYS_Math.h>
#include <UT/UT_DSOVersion.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_Director.h>
#include <OP/OP_OperatorTable.h>
#include "OBJ_Shake.h"


using namespace HDK_Sample;

// shakeIndirect is an array of indices that are used to access parameters
// quickly. The indices are initialized by the EVAL_FLOAT methods in the
// header file.
int *OBJ_Shake::shakeIndirect = 0;



// Constructor for new object class
OBJ_Shake::OBJ_Shake(OP_Network *net, const char *name, OP_Operator *op)
	  :OBJ_Geometry(net, name, op)
{
    // initialize local member data here.

    // shakeIndirect is initialized to an array of 1 index (each initialized
    // to -1 -- ie not accessed yet).
    //
    // The base class, OBJ_Geometry, uses its own class static indirect array.
    // We don't have to worry about this as long as we don't change the order
    // of these base parameters in our template list.  If we were to change
    // the order of these base parameters, then we would need to override the
    // getIndirect() method to ensure our class static indirect array is used.
    // In that case, we'd need to ensure it was large enough to handle all of
    // the base parameters as well (i.e. allocIndirect(I_N_GEO_INDICES + 1).
    if (!shakeIndirect) shakeIndirect = allocIndirect(1);
}



// virtual destructor for new object class
OBJ_Shake::~OBJ_Shake()
{
}


// Un-comment the next line to see an example of creating custom tabs
//#define SWITCHER_EXAMPLE
#ifdef SWITCHER_EXAMPLE
static PRM_Name		switcherName("shakeswitcher");
static PRM_Default	switcher[] = {
    PRM_Default( 1, "Shake"),	    // 1 is number of parameters in tab
    PRM_Default( 0, "Other"),	    // actually have no parameters here
};
#endif

// here is the name of the parameter that is used by the shake object
static PRM_Name OBJjitter("jitter",           "Jitter Scale");



// this is the template list that defines the new parameters that are
// used by the shake object.
static PRM_Template	templatelist[] =
{
    // Here we define the new parameter
#ifdef SWITCHER_EXAMPLE
    PRM_Template(PRM_SWITCHER,  sizeof(switcher)/sizeof(PRM_Default),
				&switcherName, switcher),
#endif
    PRM_Template(PRM_XYZ_J, 3, &OBJjitter, PRMoneDefaults),

    // followed by this blank terminating Template.
    PRM_Template()
};



// this function returns the OP_TemplatePair that combines the parameters
// of this object with those of its ancestors in the (object type hierarchy)
OP_TemplatePair     *
OBJ_Shake::buildTemplatePair(OP_TemplatePair *prevstuff)
{
    OP_TemplatePair     *shake, *geo;

    // Here, we have to "inherit" template pairs from geometry and beyond. To
    // do this, we first need to instantiate our template list, then add the
    // base class templates.
    shake = new OP_TemplatePair(templatelist, prevstuff);
    geo = new OP_TemplatePair(OBJ_Geometry::getTemplateList(OBJ_PARMS_PLAIN),
			      shake);
    return geo;
} 



// the myConstructor method is used to create new objects of the correct
// type from the OperatorTable.
OP_Node *
OBJ_Shake::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new OBJ_Shake(net, name, op);
}



// this method pre-multiplies the given matrix with an appropriate transform
// to shake the object. our shake transform gets inserted into the object
// transform pipeline essentially before the transform parameters.
int
OBJ_Shake::applyInputIndependentTransform(OP_Context &context, UT_DMatrix4 &mat)
{
    const fpreal t = context.getTime();	// extract time from OP_Context
    fpreal 	 jx, jy, jz;
    unsigned	 seed;
    int		 modified;

    // call OBJ_Geometry::applyInputIndependentTransform() so that we don't
    // lose any information
    modified = OBJ_Geometry::applyInputIndependentTransform(context, mat);
    if (error() >= UT_ERROR_ABORT)
    {
	// don't do anything since an error has occurred.
	return modified;
    }

    // first we compute our jitter values as a random value within
    // the given jitter scale using a frame dependent seed value
    // (different for x, y, and z)
    jx = JX(t); jy = JY(t); jz = JZ(t);

    seed = (int)OPgetDirector()->getChannelManager()->getSample(t);
    jx *= 2*SYSfastRandom(seed) - 1.0;

    seed ^= 0xdeadbeef;
    jy *= 2*SYSfastRandom(seed) - 1.0;

    seed ^= 0xfadedcab;
    jz *= 2*SYSfastRandom(seed) - 1.0;

    // we add our jitter to the object transform
    mat.pretranslate(jx, jy, jz);

    // since the jitter value is frame dependent and this object won't
    // be flagged as time dependent through any other method, we need to
    // explicitly flag it here. 
    // NB: this flag gets reset at the beginning of every cook, unless
    //     tranform caching is enabled by the object parameter.
    //     When transforms are cached, this flag is cleared before the cook on
    //     which there was a miss that invalidated all cached transforms for
    //     that object.
    flags().setTimeDep(true);

    // return 1 to indicate that we have modified the input matrix.
    // if we didn't modify mat, then we should return 0 instead.
    return 1;
}



// this function installs the new object in houdini's object table.
void
newObjectOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator("hdk_shake", "Shake",
		       OBJ_Shake::myConstructor,
		       OBJ_Shake::buildTemplatePair(0), 0, 1,
		       0));
}
