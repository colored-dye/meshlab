#ifndef IMPORT_ALL_TYPES_H
#define IMPORT_ALL_TYPES_H


#include "io/io_modeltype.h"
#include <wrap/ply/plylib.h>
#include <wrap/io_trimesh/import.h>

#include "io/MyVCGMesh.h"
#include "Mesh.h"

bool import_all_types(const char *name, Mesh *mesh);

#endif // IMPORT_ALL_TYPES_H
