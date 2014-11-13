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
 */


#include <stdio.h>
#include <iostream>
#include <CMD/CMD_Args.h>
#include <UT/UT_Assert.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>

static void
usage(const char *program)
{
    cerr << "Usage: " << program << "\n";
}

static inline float
sphereVal(int i, int j, int k, int xres, int yres, int zres)
{
    float	x, y, z;

    x = 2.0F * (i-0.5F*xres+0.5F) / xres;
    y = 2.0F * (j-0.5F*yres+0.5F) / yres;
    z = 2.0F * (k-0.5F*zres+0.5F) / zres;
    return SYSsqrt(x*x + y*y + z*z) < 1.0F ? 1.0F : 0;
}

// Generate a volume primitive with a resolution of 16x16x16.  The volume
// primitive will be dumped as binary output to standard output.
//
// Build using:
//	hcustom -s gengeovolume.C
//
// Example usage:
//	gengeovolume > volume.bgeo
int
main(int argc, char *argv[])
{
    CMD_Args		 args;
    GU_Detail		 gdp;
    GU_PrimVolume	*volume;
    const int		 xres = 16;
    const int		 yres = 16;
    const int		 zres = 16;
    const int		 binary = 1;
    int			 i, j, k;

    args.initialize(argc, argv);

    if (args.argc() != 1)
    {
	usage(argv[0]);
	return 1;
    }

    volume = (GU_PrimVolume *)GU_PrimVolume::build(&gdp);

    // The COW handle will write data to the voxel array on destruction
    {
	UT_VoxelArrayWriteHandleF	handle = volume->getVoxelWriteHandle();

	handle->size(xres, yres, zres);
	for (i = 0; i < xres; i++)
	    for (j = 0; j < yres; j++)
		for (k = 0; k < zres; k++)
		    handle->setValue(i, j, k,
			    sphereVal(i, j, k, xres, yres, zres));
    }

    gdp.save(std::cout, binary, 0);
    return 0;
}
