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

#ifndef __GU_PackedSphere__
#define __GU_PackedSphere__

#include <GU/GU_PackedImpl.h>

class GU_PrimPacked;

namespace HDK_Sample
{

/// Example of a simple packed primitive
///
/// The packed primitive created references a GU_Detail containing a polysoup
/// of a sphere.  The primitive has a single parameter:
/// - LOD @n
///   The level of detail (i.e. the level of refinement of the icosahedron)
///
/// Each sphere primitive shares its geometry with other spheres of the same LOD
class GU_PackedSphere : public GU_PackedImpl
{
public:
    GU_PackedSphere();
    GU_PackedSphere(const GU_PackedSphere &src);
    virtual ~GU_PackedSphere();

    static void	install(GA_PrimitiveFactory *factory);

    /// @{
    /// Virtual interface from GU_PackedImpl interface
    virtual GU_PackedFactory	*getFactory() const;
    virtual GU_PackedImpl	*copy() const;
    virtual void		 clearData();

    virtual bool	 isValid() const;
    virtual bool	 load(const UT_Options &options, const GA_LoadMap &map);
    virtual void	 update(const UT_Options &options);
    virtual bool	 save(UT_Options &options, const GA_SaveMap &map) const;
    virtual bool	 getBounds(UT_BoundingBox &box) const;
    virtual bool	 getRenderingBounds(UT_BoundingBox &box) const;
    virtual void	 getVelocityRange(UT_Vector3 &min, UT_Vector3 &max) const;
    virtual void	 getWidthRange(fpreal &min, fpreal &max) const;
    virtual bool			unpack(GU_Detail &destgdp) const;
    virtual GU_ConstDetailHandle	getPackedDetail() const;
    /// @}

    /// @{
    /// Member data accessors for intrinsics
    exint	lod() const		{ return myLOD; }
    void	setLOD(exint l);
    /// @}

private:
    void			clearSphere();

    GU_ConstDetailHandle	myDetail;
    exint			myLOD;
};

}	// End namespace

#endif
