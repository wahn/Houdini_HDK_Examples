#ifndef __ROP_Radiance_h__
#define __ROP_Radiance_h__

#include <ROP/ROP_Node.h>

class ROP_Radiance : public ROP_Node {
public:
    // creates an instance of this node
    static OP_Node* construct(OP_Network* net,
                              const char* name,
                              OP_Operator* op);
    // provides access to our parm tempates
    static OP_TemplatePair* getTemplates(void);
    // provides access to our variables
    static OP_VariablePair* getVariables(void);

protected:
    ROP_Radiance(OP_Network* net, const char* name, OP_Operator* op);
    // render methods
    virtual int startRender(int nframes, fpreal tstart, fpreal tend);
    virtual ROP_RENDER_CODE renderFrame(fpreal time, UT_Interrupt* boss = 0);
    virtual ROP_RENDER_CODE endRender(void);

private:
    fpreal myEndTime;
};

#endif // __ROP_Radiance_h__
