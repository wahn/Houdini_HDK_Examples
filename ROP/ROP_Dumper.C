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
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Director.h>
#include <SOP/SOP_Node.h>
#include <ROP/ROP_Error.h>
#include <ROP/ROP_Templates.h>
#include "ROP_Dumper.h"

using namespace	HDK_Sample;

int			*ROP_Dumper::ifdIndirect = 0;

static PRM_Name		 theFileName("file", "Save to file");
static PRM_Default	 theFileDefault(0, "junk.out");

static PRM_Template *
getTemplates()
{
    static PRM_Template	*theTemplate = 0;

    if (theTemplate)
	return theTemplate;

    theTemplate = new PRM_Template[14];
    theTemplate[0] = PRM_Template(PRM_FILE, 1, &theFileName, &theFileDefault);
    theTemplate[1] = theRopTemplates[ROP_TPRERENDER_TPLATE];
    theTemplate[2] = theRopTemplates[ROP_PRERENDER_TPLATE];
    theTemplate[3] = theRopTemplates[ROP_LPRERENDER_TPLATE];
    theTemplate[4] = theRopTemplates[ROP_TPREFRAME_TPLATE];
    theTemplate[5] = theRopTemplates[ROP_PREFRAME_TPLATE];
    theTemplate[6] = theRopTemplates[ROP_LPREFRAME_TPLATE];
    theTemplate[7] = theRopTemplates[ROP_TPOSTFRAME_TPLATE];
    theTemplate[8] = theRopTemplates[ROP_POSTFRAME_TPLATE];
    theTemplate[9] = theRopTemplates[ROP_LPOSTFRAME_TPLATE];
    theTemplate[10] = theRopTemplates[ROP_TPOSTRENDER_TPLATE];
    theTemplate[11] = theRopTemplates[ROP_POSTRENDER_TPLATE];
    theTemplate[12] = theRopTemplates[ROP_LPOSTRENDER_TPLATE];
    theTemplate[13] = PRM_Template();

    return theTemplate;
}

OP_TemplatePair *
ROP_Dumper::getTemplatePair()
{
    static OP_TemplatePair *ropPair = 0;
    if (!ropPair)
    {
	OP_TemplatePair	*base;

	base = new OP_TemplatePair(getTemplates());
	ropPair = new OP_TemplatePair(ROP_Node::getROPbaseTemplate(), base);
    }
    return ropPair;
}

OP_VariablePair *
ROP_Dumper::getVariablePair()
{
    static OP_VariablePair *pair = 0;
    if (!pair)
	pair = new OP_VariablePair(ROP_Node::myVariableList);
    return pair;
}

OP_Node *
ROP_Dumper::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new ROP_Dumper(net, name, op);
}

ROP_Dumper::ROP_Dumper(OP_Network *net, const char *name, OP_Operator *entry)
	: ROP_Node(net, name, entry)
{

    if (!ifdIndirect)
	ifdIndirect = allocIndirect(16);
}


ROP_Dumper::~ROP_Dumper()
{
}

//------------------------------------------------------------------------------
// The startRender(), renderFrame(), and endRender() render methods are
// invoked by Houdini when the ROP runs.

int
ROP_Dumper::startRender(int /*nframes*/, fpreal tstart, fpreal tend)
{
    myEndTime = tend;
    if (error() < UT_ERROR_ABORT)
	executePreRenderScript(tstart);

    return 1;
}

static void
printNode(ostream &os, OP_Node *node, int indent)
{
    UT_WorkBuffer wbuf;
    wbuf.sprintf("%*s", indent, "");
    os << wbuf.buffer() << node->getName() << endl;

    for (int i=0; i<node->getNchildren(); ++i)
	printNode(os, node->getChild(i), indent+2);
}

ROP_RENDER_CODE
ROP_Dumper::renderFrame(fpreal time, UT_Interrupt *)
{
    // Execute the pre-render script.
    executePreFrameScript(time);

    // Evaluate the parameter for the file name and write something to the
    // file.
    UT_String file_name;
    OUTPUT(file_name, time);

    ofstream os(file_name);
    printNode(os, OPgetDirector(), 0);
    os.close();

    // Execute the post-render script.
    if (error() < UT_ERROR_ABORT)
	executePostFrameScript(time);

    return ROP_CONTINUE_RENDER;
}

ROP_RENDER_CODE
ROP_Dumper::endRender()
{
    if (error() < UT_ERROR_ABORT)
	executePostRenderScript(myEndTime);
    return ROP_CONTINUE_RENDER;
}

void
newDriverOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator("hdk_dumper",
					"Dump Tree",
					ROP_Dumper::myConstructor,
					ROP_Dumper::getTemplatePair(),
					0,
					0,
					ROP_Dumper::getVariablePair(),
					OP_FLAG_GENERATOR));
}

