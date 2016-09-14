typedef uint32_t  cj_region_id;
typedef uint64_t  cj_spacial_unit;
typedef uint16_t  cj_angular_unit;
typedef uint32_t  cj_physics_unit;
typedef fe_q32_32 cj_physics_qunit;
typedef uint64_t  cj_time_unit;

/*
 * Spacial units and angular units must be unsigned.
 * Rationale :
 * - Signed integer overflow leads to undefined behaviour.
 *   Doesn't seem like a problem in practice, but still.
 *
 * Regions are not connected between them.
 * Instead, portals should be used.
 * Rationale :
 * + It prevents regions from having to store references to their 
 *   neighbours;
 * - On the other hand, more portals are to be created.
 *   It seems inconvenient to do in the case of multiple regions which
 *   are intended to be connected to each other.
 * + Assuming that regions were all connected by default :
 *   It'd be bothering having to handle both portals and connected
 *   region borders like they were not the same thing.
 *   On the other hand, it's the job of portals to warp moving objects to 
 *   another place.
 * ? Some user-side mechanism could allow automatic connections between 
 *   regions, which would actually be implemented as a portal.
 *   Something like :
 *   typedef struct {
 *       cj_region_id self, above, under, ...;
 *   } cj_region;
 *   Would simply imply having a portal for each connection.
 *   The user doesn't have to know it.
 *   The implementation just has to handle regular portals as always.
 *
 * Physics simulations are limited by the Physics qUnit type, which is
 * required to be a fixed-point representation.
 * The Physics Unit type MUST have a number of bits less than or equal to
 * the number of decimal bits of the Physics qUnit type.
 * Rationale :
 * + For fixed-point : We have constant resolution, consistent results across
 *   platforms, while still being fast. Also, it is a better match for the
 *   integer types used to represent spatial units.
 * + We can't go for regular integers because of these reasons :
 *   + At some point, we'll have to apply rotations around an origin to 
 *     several collision primitives. This implies sin() and cos(), which
 *     implies the ability to store values between 0 and 1.
 *   + This also give us access to standard 3D math stuff like transformation
 *     matrices, normalized vectors, etc.
 *
 *
 * Here is a hypothetical structure for a sample 3d instance :
 *
 * typedef struct {
 *     // Members from which others are computed
 *     cj_region_id region_id;
 *     struct { cj_spacial_unit x, y, z; } position;
 *     struct { cj_spacial_unit x, y, z; cj_angular_unit w;} rotation;
 *     struct { cj_physics_unit w, h, d;} scale;
 *
 *     // These members are cached, and refreshed by the above members.
 *     struct {
 *         // Sphere data (model-specific, not instance-specific)
 *         //struct { cj_physics_unit_clamp x, y, z; } center;
 *              //// Between 0 and 1, scaled by the 'scale' member.
 *         //cj_physics_unit_clamp radius;
 *         // Sphere instance
 *
 *     } *collision_spheres_cached_positions ???;
 *     fe_mat4 render_transform;
 *     size_t collision_sphere_count;
 *     cj_mesh3d mesh;
 * } cj_instance3d;
 */
