#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <ROP/ROP_Node.h>
#include <ROP/ROP_Templates.h> // ROP_TPRERENDER_TPLATE etc.
#include <UT/UT_DSOVersion.h>  // use for sesitag
// header file
#include "ROP_Radiance.h"

static PRM_Name theFileName("file", "Save to file");
static PRM_Default theFileDefault(0, "houdini.rad");

// helper functions

static PRM_Template*
getBaseTemplates() {
    static PRM_Template* theTemplate = NULL;

    if (theTemplate) return theTemplate;

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

// class ROP_Radiance

ROP_Radiance::ROP_Radiance(OP_Network* net, const char* name, OP_Operator* op)
: ROP_Node(net, name, op) {
    // TODO: ifdIndirect ???
}

OP_TemplatePair*
ROP_Radiance::getTemplates() {
    static OP_TemplatePair* tPair = NULL;
    if (!tPair) {
        OP_TemplatePair* base;
        base = new OP_TemplatePair(getBaseTemplates());
        tPair = new OP_TemplatePair(ROP_Node::getROPbaseTemplate(), base);
    }
    return tPair;
}

OP_VariablePair*
ROP_Radiance::getVariables() {
    static OP_VariablePair* vPair = NULL;
    if (!vPair) vPair = new OP_VariablePair(ROP_Node::myVariableList);
    return vPair;
}

OP_Node*
ROP_Radiance::construct(OP_Network* net, const char* name, OP_Operator* op) {
    return new ROP_Radiance(net, name, op);
}

int
ROP_Radiance::startRender(int nframes, fpreal tstart, fpreal tend) {
    return 1;
}

ROP_RENDER_CODE
ROP_Radiance::renderFrame(fpreal time, UT_Interrupt* boss) {
    return ROP_CONTINUE_RENDER;
}

ROP_RENDER_CODE
ROP_Radiance::endRender() {
    return ROP_CONTINUE_RENDER;
}

// entry point

SYS_VISIBILITY_EXPORT extern void
newDriverOperator(OP_OperatorTable* table) {
    table->addOperator(new OP_Operator("radiance",
                                       "Radiance",
                                       ROP_Radiance::construct,
                                       ROP_Radiance::getTemplates(),
                                       0, // min_sources
                                       0, // max_sources
                                       ROP_Radiance::getVariables(),
                                       OP_FLAG_GENERATOR));
}
