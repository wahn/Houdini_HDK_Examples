== What this is ==

A sample of adding a new primitive to Houdini using the simplified
packed primitive interface.

== Notes on how it works ==

The GU_PackedSphere implements the interface for a packed primitive.
If you contrast this with the tetprim example, you'll see that this is
a much simpler interface.  In addition, the rendering of packed
primitives is handled automatically.

The packedsphere.C stand-alone application saves a single packed
primitive to a disk file.

== How to build ==

hcustom GU_PackedSphere.C
hcustom -s packedsphere.C
