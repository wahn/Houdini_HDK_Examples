#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <ROP/ROP_Node.h>
#include <UT/UT_DSOVersion.h> // use for sesitag
// header file
#include "ROP_Radiance.h"

ROP_Radiance::ROP_Radiance(OP_Network* net, const char* name, OP_Operator* op)
: ROP_Node(net, name, op) {
    // TODO: ifdIndirect ???
}

OP_TemplatePair*
ROP_Radiance::getTemplates() {
    static OP_TemplatePair* tPair = NULL;
    return tPair;
}

OP_VariablePair*
ROP_Radiance::getVariables() {
    static OP_VariablePair* vPair = NULL;
    return vPair;
}

OP_Node*
ROP_Radiance::construct(OP_Network* net, const char* name, OP_Operator* op) {
    return new ROP_Radiance(net, name, op);
}

SYS_VISIBILITY_EXPORT extern void
newDriverOperator(OP_OperatorTable* table) {
    table->addOperator(new OP_Operator("rop_radiance",
                                       "Radiance",
                                       ROP_Radiance::construct,
                                       ROP_Radiance::getTemplates(),
                                       0, // min_sources
                                       0, // max_sources
                                       ROP_Radiance::getVariables(),
                                       OP_FLAG_GENERATOR));
}
