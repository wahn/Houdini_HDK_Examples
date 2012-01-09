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
 */

#include <stdio.h>
#include <iostream>
#include <IMG3D/IMG3D_Manager.h>
#include <UT/UT_Vector3.h>
#include <UT/UT_BoundingBox.h>
#include <UT/UT_Exit.h>
#include <UT/UT_Options.h>

namespace HDK_Sample {

static void
error(const char *msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    UT_Exit::exit( UT_Exit::EXIT_GENERIC_ERROR );
}

static void
i3dSphere(int nvalues,
		const UT_Vector3 *P,
		fpreal32 *result[],
		const char *names[],
		int sizes[],
		int for_aa)
{
    fpreal32	*density;
    fpreal32	*color;
    int		 i;
    fpreal	 d;

    // Verify correct channels
    UT_ASSERT(!strcmp(names[0], "density") && sizes[0] == 1);
    UT_ASSERT(!strcmp(names[1], "color") && sizes[1] == 3);
    density = result[0];
    color = result[1];

    for (i = 0; i < nvalues; i++)
    {
	// Density is 1 inside the sphere and 0 outside
	d = P[i].length() > 1 ? 0 : 1;
	*density = d;
	color[0] = d * SYSfit(P[i].x(), -1, 1, 0, 1);
	color[1] = d * SYSfit(P[i].y(), -1, 1, 0, 1);
	color[2] = d * SYSfit(P[i].z(), -1, 1, 0, 1);

	// Now, move the result buffers to the next pixel
	density += 1;	// Skip to the next float
	color   += 3;	// Skip three floats
    }
}

}	// End of HDK_Sample namespace

using namespace HDK_Sample;

int
main(int argc, char *argv[])
{
    IMG3D_Manager	fp;
    UT_BoundingBox	bounds;
    const char		*chnames[2] = { "density", "color" };
    int			 chsizes[2] = { 1, 3 };

    printf("Generating sphere.i3d\n");
    bounds.initBounds(-1, -1, -1);
    bounds.enlargeBounds(1, 1, 1);
    if (!fp.createTexture("sphere.i3d", bounds, 10, 10, 10))
	error("Unable to createTexture()");

    if (!fp.fillTexture(2, chnames, chsizes, i3dSphere, 1))
	error("Unable to fill the texture");

    if (!fp.exportTag("software", "hdk_isosphere"))
	error("Unable to save software tag");

    if (!fp.closeTexture())
	error("Unable to close texture");

    return 0;
}
