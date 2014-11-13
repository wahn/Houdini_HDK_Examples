/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * Produced by:
 *	Side Effects Software Inc
 *	123 Front Street West, Suite 1401
 *	Toronto, Ontario
 *	Canada   M5J 2M2
 *	416-504-9876
 *
 * NAME:	GU_PackedSphere.h (GU  Library, C++)
 *
 * COMMENTS:
 */

#include "GU_PackedSphere.h"
#include <GU/GU_PackedFactory.h>
#include <GU/GU_PrimSphere.h>
#include <GU/GU_PrimPacked.h>
#include <UT/UT_DSOVersion.h>

using namespace HDK_Sample;

namespace
{
    class SphereFactory : public GU_PackedFactory
    {
    public:
	SphereFactory()
	    : GU_PackedFactory("PackedSphere", "Packed Sphere")
	{
	    registerIntrinsic("lod",
		    IntGetterCast(&GU_PackedSphere::lod),
		    IntSetterCast(&GU_PackedSphere::setLOD));
	}
	virtual ~SphereFactory() {}

	virtual GU_PackedImpl	*create() const
	{
	    return new GU_PackedSphere();
	}
    };

    static SphereFactory	*theFactory = NULL;

    /// Store spheres in a shared cache
    class CacheEntry
    {
    public:
	CacheEntry(int lod)
	    : myGdp()
	    , myRef(0)
	{
	    GU_Detail		*gdp = new GU_Detail();
	    GU_PrimSphereParms	 parms(gdp);
	    parms.freq = lod;
	    GU_PrimSphere::build(parms, GEO_PRIMPOLYSOUP);
	    myGdp.allocateAndSet(gdp);
	};
	~CacheEntry()
	{
	}
	GU_ConstDetailHandle	detail() const
	{
	    return GU_ConstDetailHandle(myGdp);
	}

	void	incref()	{ myRef++; }
	bool	decref()	{ myRef--; return !myRef; }
    private:
	GU_DetailHandle	myGdp;
	int		myRef;
    };
    // The cache is quite simple, we just support lod's between 1 and 32.
    #define MIN_LOD	1
    #define MAX_LOD	32
    static CacheEntry	*theCache[MAX_LOD+1];
    static UT_Lock	 theLock;

    static GU_ConstDetailHandle
    getSphere(int lod)
    {
	UT_AutoLock	lock(theLock);
	lod = SYSclamp(lod, MIN_LOD, MAX_LOD);
	if (!theCache[lod])
	    theCache[lod] = new CacheEntry(lod);
	theCache[lod]->incref();
	return theCache[lod]->detail();
    }
    static void
    releaseSphere(int lod)
    {
	UT_AutoLock	lock(theLock);
	UT_ASSERT(theCache[lod]);
	if (theCache[lod]->decref())
	{
	    delete theCache[lod];
	    theCache[lod] = NULL;
	}
    }
}

GU_PackedSphere::GU_PackedSphere()
    : GU_PackedImpl()
    , myDetail()
    , myLOD(2)
{
}

GU_PackedSphere::GU_PackedSphere(const GU_PackedSphere &src)
    : GU_PackedImpl(src)
    , myDetail()
    , myLOD(src.myLOD)
{
}

GU_PackedSphere::~GU_PackedSphere()
{
    clearSphere();
}

void
GU_PackedSphere::install(GA_PrimitiveFactory *gafactory)
{
    UT_ASSERT(!theFactory);
    if (theFactory)
	return;

    theFactory = new SphereFactory();
    GU_PrimPacked::registerPacked(gafactory, theFactory);
}

void
GU_PackedSphere::clearSphere()
{
    if (myLOD > 0 && myDetail.isValid())
    {
	releaseSphere(myLOD);
	myDetail = GU_ConstDetailHandle();
	myLOD = 0;
    }
}

GU_PackedFactory *
GU_PackedSphere::getFactory() const
{
    return theFactory;
}

GU_PackedImpl *
GU_PackedSphere::copy() const
{
    return new GU_PackedSphere(*this);
}

void
GU_PackedSphere::clearData()
{
    clearSphere();
}

bool
GU_PackedSphere::isValid() const
{
    return myDetail.isValid();
}

#define LOD_TOKEN	"lod"

bool
GU_PackedSphere::load(const UT_Options &options, const GA_LoadMap &)
{
    update(options);
    return true;
}

void
GU_PackedSphere::update(const UT_Options &options)
{
    int		ival;
    if (options.importOption(LOD_TOKEN, ival))
	setLOD(ival);
}

bool
GU_PackedSphere::save(UT_Options &options, const GA_SaveMap &map) const
{
    options.setOptionI(LOD_TOKEN, myLOD);
    return true;
}

bool
GU_PackedSphere::getBounds(UT_BoundingBox &box) const
{
    // All spheres are unit spheres with transforms applied
    box.initBounds(-1, -1, -1);
    box.enlargeBounds(1, 1, 1);
    // If computing the bounding box is expensive, you may want to cache the
    // box by calling setBoxCache(box)
    return true;
}

bool
GU_PackedSphere::getRenderingBounds(UT_BoundingBox &box) const
{
    // When geometry contains points or curves, the width attributes need to be
    // taken into account when computing the rendering bounds.
    return getBounds(box);
}

void
GU_PackedSphere::getVelocityRange(UT_Vector3 &min, UT_Vector3 &max) const
{
    min = 0;	// No velocity attribute on geometry
    max = 0;
}

void
GU_PackedSphere::getWidthRange(fpreal &min, fpreal &max) const
{
    min = max = 0;	// Width is only important for curves/points.
}

bool
GU_PackedSphere::unpack(GU_Detail &destgdp) const
{
    // This may allocate geometry for the primitive
    GU_DetailHandleAutoReadLock	rlock(getPackedDetail());
    if (!rlock.getGdp())
	return false;
    return unpackToDetail(destgdp, rlock.getGdp());
}

GU_ConstDetailHandle
GU_PackedSphere::getPackedDetail() const
{
    if (!myDetail.isValid() && myLOD > 0)
    {
	/// We only get the sphere on demand.  If the user only requests the
	/// bounding box (i.e. the viewport LOD is set to "box"), then we never
	/// have to actually create the sphere's geometry.
	GU_PackedSphere	*me = const_cast<GU_PackedSphere *>(this);
	me->myDetail = getSphere(myLOD);
    }
    return myDetail;
}

void
GU_PackedSphere::setLOD(exint l)
{
    clearSphere();
    myLOD = l;
    topologyDirty();	// Notify base primitive that topology has changed
}

/// DSO registration callback
void
newGeometryPrim(GA_PrimitiveFactory *f)
{
    GU_PackedSphere::install(f);
}
