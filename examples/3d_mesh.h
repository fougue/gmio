#pragma once

#include <stddef.h>

/* User-defined 3D point */
struct my_3d_point
{
    double coords[3];
};

/* User-defined 3D triangle */
struct my_3d_triangle
{
    struct my_3d_point vertex[3];
};

/* An example of user-defined 3D mesh */
struct my_3d_mesh
{
    struct my_3d_triangle* triangle_array;
    size_t triangle_array_count;
    size_t triangle_array_capacity;
};
