#ifndef EXPORT_ALL_TYPES_H
#define EXPORT_ALL_TYPES_H

#include <vcg/complex/algorithms/create/platonic.h>
#include <wrap/io_trimesh/export.h>

#include "io/io_modeltype.h"
#include "io/MyVCGMesh.h"
#include "Mesh.h"

bool export_all_types(const char *name, Mesh *mesh);

#endif // EXPORT_ALL_TYPES_H
