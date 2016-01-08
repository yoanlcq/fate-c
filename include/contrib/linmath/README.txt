# linmath.h -- A small library for linear math as required for computer graphics

linmath.h provides the most used types required programming computer graphice:

vec3 -- 3 element vector of floats
vec4 -- 4 element vector of floats (4th component used for homogenous computations)
mat4x4 -- 4 by 4 elements matrix, computations are done in column major order
quat -- quaternion

The types are deliberately named like the types in GLSL. In fact they are meant to 
be used for the client side computations and passing to same typed GLSL uniforms.

