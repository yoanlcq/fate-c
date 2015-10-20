#ifndef FST_CFG_H
#define FST_CFG_H

/* If this macro is set, GNU extensions are used when they perform 
 * better than otherwise naïve code. */
#define FST_CFG_GNU_SOURCE

/* On Linux, interaction with the X server is provided by the Xlib, 
 * which is the standard wrapper for the X protocol. 
 * XCB provides a more direct access to the X protocol, and claims to be
 * faster. 
 * However, it needs more function calls and data to keep.
 * Thus, using XCB is way more intimidating but for the reasons above, its 
 * use is encouraged. 
 * The macro below means "use XCB instead of the Xlib when possible".
 * If it is not defined, the Xlib is used. */
/* If set, link with "-lxcb -lxcb-randr". 
 * Otherwise, link with "-lX11 -lXrandr". */

/* #define FST_CFG_XCB_OVER_XLIB */

/* This applies to Linux again.
 * Wayland is intended as a replacement to the X protocol. 
 * Though as of today (2015) it is not so widely-adopted, we could try to 
 * be future-proof.
 * The macro below means "use Wayland instead to XCB/Xlib when possible".
 * If it is not defined, XCB or Xlib is used. */
#define FST_CFG_WAYLAND_OVER_X11

/* You know, in case it changes in 2040. */
#define FST_CFG_DEFAULT_REFRESH_RATE 60

/* Node names only take the needed amount of memory (not fixed-sized buffers).
 * However, this defines the static buffer's size used to hold a name in 
 * the path resolution process.
 * fst_checkpath() reports any node name in a path which length exceeds 
 * this size. */
#define FST_CFG_NODE_NAME_MAX_LENGTH 255

/* By default, fst's path resolution routines don't check if any node name's
 * length exceeds the limit defined above (leaving it up to fst_checkpath()). 
 * Defining the macro below forces them to do so. 
 * Since it has a negative impact on speed, the user is advised to comment 
 * it out when their game is ready for release. */
#define FST_CFG_CHECK_NODE_NAME_OVERFLOW

/* This defines the static stack's capacity used to hold the absolute 
 * full path to the Current Working Node (CWN) in the path resolution process.
 * Therefore, it also defines the Global Tree's maximum depth.
 * It has no impact on speed, only space.
 * This macro's value must be in the range of an unsigned 32-bit integer. */
#define FST_CFG_MAX_PATH_STACK 255

/* This defines the integer size to be used for fst_nodepack's count field.
 * fst_nodepacks are used to store a node's children.
 * Using a smaller value saves memory, but also limits the number of 
 * children any given node can have to (2^n-1), where n is this macro's value.
 * It also affects the fst_nodepack functions' memory allocation strategy 
 * and behaviour.
 * The user is advised to adapt this value to their needs.
 * This macro's value can be set to either 8, 16, 32 or 64. */
#define FST_CFG_DENTRY_CAPACITY_INTSIZE 16



#endif /* FST_MACROS_H */
