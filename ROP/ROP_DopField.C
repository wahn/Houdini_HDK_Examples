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

#include <fstream.h>
#include <UT/UT_DSOVersion.h>
#include <CH/CH_LocalVariable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Director.h>
#include <DOP/DOP_Node.h>
#include <DOP/DOP_Parent.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ScalarField.h>
#include <SIM/SIM_VectorField.h>
#include <SIM/SIM_MatrixField.h>
#include <ROP/ROP_Error.h>
#include <ROP/ROP_Templates.h>
#include <UT/UT_IOTable.h>
#include "ROP_DopField.h"

using namespace	HDK_Sample;

static PRM_Name		dopPathName("doppath",	"DOP Path");
static PRM_Name		theObjectName("dopobject", "DOP Object");
static PRM_Name		theDataName("dopdata", "DOP Data");
static PRM_Name		 theFileName("file", "Output File");
static PRM_Default	 theFileDefault(0, "$HIP/$F.bgeo");
static PRM_Name		alfprogressName("alfprogress", "Alfred Style Progress");


static PRM_Name collateName("collatevector", "Collate Vector Fields");

static PRM_Template	 f3dTemplates[] = {
    PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &dopPathName,
				0, 0, 0, 0, &PRM_SpareData::dopPath),
    PRM_Template(PRM_FILE,	1, &theFileName, &theFileDefault,0,
				0, 0, &PRM_SpareData::fileChooserModeWrite),
    PRM_Template(PRM_TOGGLE, 1, &alfprogressName, PRMzeroDefaults),

    PRM_Template(PRM_STRING,	1, &theObjectName),
    PRM_Template(PRM_STRING,	1, &theDataName),
    PRM_Template()
			    
};

static PRM_Template *
getTemplates()
{
    static PRM_Template	*theTemplate = 0;

    if (theTemplate)
	return theTemplate;

    theTemplate = new PRM_Template[ROP_DOPFIELD_MAXPARMS+1];
    theTemplate[ROP_DOPFIELD_RENDER] = theRopTemplates[ROP_RENDER_TPLATE];
    theTemplate[ROP_DOPFIELD_RENDER_CTRL] = theRopTemplates[ROP_RENDERDIALOG_TPLATE];
    theTemplate[ROP_DOPFIELD_TRANGE] = theRopTemplates[ROP_TRANGE_TPLATE];
    theTemplate[ROP_DOPFIELD_FRANGE] = theRopTemplates[ROP_FRAMERANGE_TPLATE];
    theTemplate[ROP_DOPFIELD_TAKE] = theRopTemplates[ROP_TAKENAME_TPLATE];
    theTemplate[ROP_DOPFIELD_DOPPATH] = f3dTemplates[0];
    theTemplate[ROP_DOPFIELD_DOPOUTPUT] = f3dTemplates[1];
    theTemplate[ROP_DOPFIELD_DOPOBJECT] = f3dTemplates[3];
    theTemplate[ROP_DOPFIELD_DOPDATA] = f3dTemplates[4];
    theTemplate[ROP_DOPFIELD_INITSIM] = theRopTemplates[ROP_IFD_INITSIM_TPLATE];
    theTemplate[ROP_DOPFIELD_ALFPROGRESS] = f3dTemplates[2];
    theTemplate[ROP_DOPFIELD_TPRERENDER] = theRopTemplates[ROP_TPRERENDER_TPLATE];
    theTemplate[ROP_DOPFIELD_PRERENDER] = theRopTemplates[ROP_PRERENDER_TPLATE];
    theTemplate[ROP_DOPFIELD_LPRERENDER] = theRopTemplates[ROP_LPRERENDER_TPLATE];
    theTemplate[ROP_DOPFIELD_TPREFRAME] = theRopTemplates[ROP_TPREFRAME_TPLATE];
    theTemplate[ROP_DOPFIELD_PREFRAME] = theRopTemplates[ROP_PREFRAME_TPLATE];
    theTemplate[ROP_DOPFIELD_LPREFRAME] = theRopTemplates[ROP_LPREFRAME_TPLATE];
    theTemplate[ROP_DOPFIELD_TPOSTFRAME] = theRopTemplates[ROP_TPOSTFRAME_TPLATE];
    theTemplate[ROP_DOPFIELD_POSTFRAME] = theRopTemplates[ROP_POSTFRAME_TPLATE];
    theTemplate[ROP_DOPFIELD_LPOSTFRAME] = theRopTemplates[ROP_LPOSTFRAME_TPLATE];
    theTemplate[ROP_DOPFIELD_TPOSTRENDER] = theRopTemplates[ROP_TPOSTRENDER_TPLATE];
    theTemplate[ROP_DOPFIELD_POSTRENDER] = theRopTemplates[ROP_POSTRENDER_TPLATE];
    theTemplate[ROP_DOPFIELD_LPOSTRENDER] = theRopTemplates[ROP_LPOSTRENDER_TPLATE];
    theTemplate[ROP_DOPFIELD_MAXPARMS] = PRM_Template();

    UT_ASSERT(PRM_Template::countTemplates(theTemplate) == ROP_DOPFIELD_MAXPARMS);

    return theTemplate;
}

OP_TemplatePair *
ROP_DopField::getTemplatePair()
{
    static OP_TemplatePair *ropPair = 0;
    if (!ropPair)
    {
	ropPair = new OP_TemplatePair(getTemplates());
    }
    return ropPair;
}

OP_VariablePair *
ROP_DopField::getVariablePair()
{
    static OP_VariablePair *pair = 0;
    if (!pair)
	pair = new OP_VariablePair(ROP_Node::myVariableList);
    return pair;
}

OP_Node *
ROP_DopField::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new ROP_DopField(net, name, op);
}

ROP_DopField::ROP_DopField(OP_Network *net, const char *name, OP_Operator *entry)
	: ROP_Node(net, name, entry)
{
}


ROP_DopField::~ROP_DopField()
{
}

//------------------------------------------------------------------------------
// The startRender(), renderFrame(), and endRender() render methods are
// invoked by Houdini when the ROP runs.

int
ROP_DopField::startRender(int /*nframes*/, fpreal tstart, fpreal tend)
{
    int			 rcode = 1;

    myEndTime = tend;
    myStartTime = tstart;

    if (INITSIM())
    {
        initSimulationOPs();
	OPgetDirector()->bumpSkipPlaybarBasedSimulationReset(1);
    }

    if (error() < UT_ERROR_ABORT)
    {
	if( !executePreRenderScript(tstart) )
	    return 0;
    }

    return rcode;
}

static void
addField(GU_Detail *gdp, const SIM_RawField *rawfield)
{
    GU_PrimVolume	*vol;

    vol = (GU_PrimVolume *) GU_PrimVolume::build(gdp);

    vol->getVertex().getPt()->setPos(
	rawfield->getOrig() + rawfield->getSize()*0.5 );
    
    UT_Matrix3		 scale;
    scale.identity();
    scale.scale(rawfield->getSize().x(), rawfield->getSize().y(), rawfield->getSize().z());
    scale.scale(0.5, 0.5, 0.5);
    vol->setTransform(scale);

    // If we use setVoxels, we'll have to make a copy of the field.
    // If we use steal, we can avoid this, but then we *must* leak
    // the gdp or we'll delete the field out from under DOPs.
    // vol->setVoxels(rawfield->fieldNC());
    vol->stealVoxels(rawfield->fieldNC());
}

ROP_RENDER_CODE
ROP_DopField::renderFrame(fpreal time, UT_Interrupt *)
{
    OP_Node		*op;
    DOP_Parent		*dopparent;
    UT_String		 doppath, savepath;
    UT_String		 dopobject, dopdata;

    if( !executePreFrameScript(time) )
	return ROP_ABORT_RENDER;

    DOPPATH(doppath, time);

    if( !doppath.isstring() )
    {
	addError(ROP_MESSAGE, "Invalid DOP path");
	return ROP_ABORT_RENDER;
    }

    op = findNode(doppath);
    if (!op)
    {
	addError(ROP_COOK_ERROR, (const char *)doppath);
	return ROP_ABORT_RENDER;
    }

    dopparent = op ? op->castToDOPParent() : 0;
    if( !dopparent )
    {
	addError(ROP_COOK_ERROR, (const char *)doppath);
	return ROP_ABORT_RENDER;
    }

    DOPOBJECT(dopobject, time);
    DOPDATA(dopdata, time);
    OUTPUT(savepath, time);


    time = DOPsetBestTime(dopparent, time);

    OP_Context		context(time);

    const SIM_Object		*object;
    object = dopparent->findObjectFromString(dopobject, 0, 0, time);

    if (!object)
    {
	addError(ROP_COOK_ERROR, (const char *)dopobject);
	return ROP_ABORT_RENDER;
    }

    const SIM_Data		*data;

    data = object->getConstNamedSubData(dopdata);

    if (!data)
    {
	addError(ROP_COOK_ERROR, (const char *) dopdata);
	return ROP_ABORT_RENDER;
    }

    // Create our GDP.
    GU_Detail			*gdp = new GU_Detail();

    const SIM_ScalarField	*scalarfield = SIM_DATA_CASTCONST(data, SIM_ScalarField);
    if (scalarfield)
    {
	addField(gdp, scalarfield->getField());
    }
    const SIM_VectorField	*vectorfield = SIM_DATA_CASTCONST(data, SIM_VectorField);
    if (vectorfield)
    {
	for (int i = 0; i < 3; i++)
	{
	    addField(gdp, vectorfield->getField(i));
	}
    }
    const SIM_MatrixField	*matrixfield = SIM_DATA_CASTCONST(data, SIM_MatrixField);
    if (matrixfield)
    {
	for (int i = 0; i < 3; i++)
	    for (int j = 0; j < 3; j++)
	    {
		addField(gdp, matrixfield->getField(i, j));
	    }
    }

    if (!gdp->save((const char *)savepath, 0, 0).success())
    {
	addError(ROP_SAVE_ERROR, (const char *)savepath);
	return ROP_ABORT_RENDER;
    }

    // DO NOT delete gdp if we are stealing the voxels!
    // delete gdp;

    if (ALFPROGRESS() && (myEndTime != myStartTime))
    {
	fpreal		fpercent = (time - myStartTime) / (myEndTime - myStartTime);
	int		percent = (int)SYSrint(fpercent * 100);
	percent = SYSclamp(percent, 0, 100);
	fprintf(stdout, "ALF_PROGRESS %d%%\n", percent);
	fflush(stdout);
    }
    
    if (error() < UT_ERROR_ABORT)
    {
	if( !executePostFrameScript(time) )
	    return ROP_ABORT_RENDER;
    }

    return ROP_CONTINUE_RENDER;
}

ROP_RENDER_CODE
ROP_DopField::endRender()
{
    if (INITSIM())
	OPgetDirector()->bumpSkipPlaybarBasedSimulationReset(-1);

    if (error() < UT_ERROR_ABORT)
    {
	if( !executePostRenderScript(myEndTime) )
	    return ROP_ABORT_RENDER;
    }
    return ROP_CONTINUE_RENDER;
}

void
newDriverOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator("hdk_dopfield",
					"Dop Field",
					ROP_DopField::myConstructor,
					ROP_DopField::getTemplatePair(),
					0,
					0,
					ROP_DopField::getVariablePair(),
					OP_FLAG_GENERATOR));
}

