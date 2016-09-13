/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "qbsp.h"
#include "l_cmd.h"
#include "l_math.h"
#include "l_mem.h"
#include "l_log.h"
#include "l_poly.h"
#include "l_script.h"
#include "l_qfiles.h"
#include "l_bsp_q3.h"
#include "l_bsp_ent.h"

void Q3_ParseEntities (void);
void Q3_PrintBSPFileSizes(void);

void GetLeafNums (void);

//=============================================================================

#define WCONVEX_EPSILON		0.5


int				q3_nummodels;
q3_dmodel_t		*q3_dmodels;//[MAX_MAP_MODELS];

int				q3_numShaders;
q3_dshader_t	*q3_dshaders;//[Q3_MAX_MAP_SHADERS];

int				q3_entdatasize;
char			*q3_dentdata;//[Q3_MAX_MAP_ENTSTRING];

int				q3_numleafs;
q3_dleaf_t		*q3_dleafs;//[Q3_MAX_MAP_LEAFS];

int				q3_numplanes;
q3_dplane_t		*q3_dplanes;//[Q3_MAX_MAP_PLANES];

int				q3_numnodes;
q3_dnode_t		*q3_dnodes;//[Q3_MAX_MAP_NODES];

int				q3_numleafsurfaces;
int				*q3_dleafsurfaces;//[Q3_MAX_MAP_LEAFFACES];

int				q3_numleafbrushes;
int				*q3_dleafbrushes;//[Q3_MAX_MAP_LEAFBRUSHES];

int				q3_numbrushes;
q3_dbrush_t		*q3_dbrushes;//[Q3_MAX_MAP_BRUSHES];

int				q3_numbrushsides;
q3_dbrushside_t	*q3_dbrushsides;//[Q3_MAX_MAP_BRUSHSIDES];

int				q3r_numbrushsides;
q3r_dbrushside_t	*q3r_dbrushsides;//[Q3_MAX_MAP_BRUSHSIDES];

int				q3_numLightBytes;
byte			*q3_lightBytes;//[Q3_MAX_MAP_LIGHTING];

int				q3_numGridBytes;
byte			*q3_gridData;//[Q3_MAX_MAP_LIGHTGRID];

int				q3_numVisBytes;
byte			*q3_visBytes;//[Q3_MAX_MAP_VISIBILITY];

int				q3_numDrawVerts;
q3_drawVert_t	*q3_drawVerts;//[Q3_MAX_MAP_DRAW_VERTS];

int				q3r_numDrawVerts;
q3r_drawVert_t	*q3r_drawVerts;//[Q3_MAX_MAP_DRAW_VERTS];

int				q3_numDrawIndexes;
int				*q3_drawIndexes;//[Q3_MAX_MAP_DRAW_INDEXES];

int				q3_numDrawSurfaces;
q3_dsurface_t	*q3_drawSurfaces;//[Q3_MAX_MAP_DRAW_SURFS];

int				q3r_numDrawSurfaces;
q3r_dsurface_t	*q3r_drawSurfaces;//[Q3_MAX_MAP_DRAW_SURFS];

int				q3_numFogs;
q3_dfog_t		*q3_dfogs;//[Q3_MAX_MAP_FOGS];

char			q3_dbrushsidetextured[Q3_MAX_MAP_BRUSHSIDES];

extern qboolean forcesidesvisible;

//===========================================================================
//
// Parameter:			-
// Returns:				-
// Changes Globals:		-
//===========================================================================
void Q3_FreeMaxBSP(void)
{
	if (q3_dmodels) FreeMemory(q3_dmodels);
	q3_dmodels = NULL;
	q3_nummodels = 0;
	if (q3_dshaders) FreeMemory(q3_dshaders);
	q3_dshaders = NULL;
	q3_numShaders = 0;
	if (q3_dentdata) FreeMemory(q3_dentdata);
	q3_dentdata = NULL;
	q3_entdatasize = 0;
	if (q3_dleafs) FreeMemory(q3_dleafs);
	q3_dleafs = NULL;
	q3_numleafs = 0;
	if (q3_dplanes) FreeMemory(q3_dplanes);
	q3_dplanes = NULL;
	q3_numplanes = 0;
	if (q3_dnodes) FreeMemory(q3_dnodes);
	q3_dnodes = NULL;
	q3_numnodes = 0;
	if (q3_dleafsurfaces) FreeMemory(q3_dleafsurfaces);
	q3_dleafsurfaces = NULL;
	q3_numleafsurfaces = 0;
	if (q3_dleafbrushes) FreeMemory(q3_dleafbrushes);
	q3_dleafbrushes = NULL;
	q3_numleafbrushes = 0;
	if (q3_dbrushes) FreeMemory(q3_dbrushes);
	q3_dbrushes = NULL;
	q3_numbrushes = 0;
	if (q3_dbrushsides) FreeMemory(q3_dbrushsides);
	q3_dbrushsides = NULL;
	q3_numbrushsides = 0;
	if (q3_lightBytes) FreeMemory(q3_lightBytes);
	q3_lightBytes = NULL;
	q3_numLightBytes = 0;
	if (q3_gridData) FreeMemory(q3_gridData);
	q3_gridData = NULL;
	q3_numGridBytes = 0;
	if (q3_visBytes) FreeMemory(q3_visBytes);
	q3_visBytes = NULL;
	q3_numVisBytes = 0;
	if (q3_drawVerts) FreeMemory(q3_drawVerts);
	q3_drawVerts = NULL;
	q3_numDrawVerts = 0;
	if (q3_drawIndexes) FreeMemory(q3_drawIndexes);
	q3_drawIndexes = NULL;
	q3_numDrawIndexes = 0;
	if (q3_drawSurfaces) FreeMemory(q3_drawSurfaces);
	q3_drawSurfaces = NULL;
	q3_numDrawSurfaces = 0;
	if (q3_dfogs) FreeMemory(q3_dfogs);
	q3_dfogs = NULL;
	q3_numFogs = 0;
	if (q3r_dbrushsides) FreeMemory(q3r_dbrushsides);
	q3r_dbrushsides = NULL;
	q3r_numbrushsides = 0;
	if (q3r_drawVerts) FreeMemory(q3r_drawVerts);
	q3r_drawVerts = NULL;
	q3r_numDrawVerts = 0;
	if (q3r_drawSurfaces) FreeMemory(q3r_drawSurfaces);
	q3r_drawSurfaces = NULL;
	q3r_numDrawSurfaces = 0;
} //end of the function Q3_FreeMaxBSP


//===========================================================================
//
// Parameter:			-
// Returns:				-
// Changes Globals:		-
//===========================================================================
void Q3_PlaneFromPoints(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t normal, float *dist)
{
	vec3_t t1, t2;

	VectorSubtract(p0, p1, t1);
	VectorSubtract(p2, p1, t2);
	CrossProduct(t1, t2, normal);
	VectorNormalize(normal);

	*dist = DotProduct(p0, normal);
} //end of the function PlaneFromPoints
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void Q3_SurfacePlane(q3_dsurface_t *surface, vec3_t normal, float *dist)
{
	int i;
	float *p0, *p1, *p2;
	vec3_t t1, t2;

	p0 = q3_drawVerts[surface->firstVert].xyz;
	for (i = 1; i < surface->numVerts-1; i++)
	{
		p1 = q3_drawVerts[surface->firstVert + ((i) % surface->numVerts)].xyz;
		p2 = q3_drawVerts[surface->firstVert + ((i+1) % surface->numVerts)].xyz;
		VectorSubtract(p0, p1, t1);
		VectorSubtract(p2, p1, t2);
		CrossProduct(t1, t2, normal);
		VectorNormalize(normal);
		if (VectorLength(normal)) break;
	} //end for*/
/*
	float dot;
	for (i = 0; i < surface->numVerts; i++)
	{
		p0 = q3_drawVerts[surface->firstVert + ((i) % surface->numVerts)].xyz;
		p1 = q3_drawVerts[surface->firstVert + ((i+1) % surface->numVerts)].xyz;
		p2 = q3_drawVerts[surface->firstVert + ((i+2) % surface->numVerts)].xyz;
		VectorSubtract(p0, p1, t1);
		VectorSubtract(p2, p1, t2);
		VectorNormalize(t1);
		VectorNormalize(t2);
		dot = DotProduct(t1, t2);
		if (dot > -0.9 && dot < 0.9 &&
			VectorLength(t1) > 0.1 && VectorLength(t2) > 0.1) break;
	} //end for
	CrossProduct(t1, t2, normal);
	VectorNormalize(normal);
*/
	if (VectorLength(normal) < 0.9)
	{
		printf("surface %td bogus normal vector %f %f %f\n", surface - q3_drawSurfaces, normal[0], normal[1], normal[2]);
		printf("t1 = %f %f %f, t2 = %f %f %f\n", t1[0], t1[1], t1[2], t2[0], t2[1], t2[2]);
		for (i = 0; i < surface->numVerts; i++)
		{
			p1 = q3_drawVerts[surface->firstVert + ((i) % surface->numVerts)].xyz;
			Log_Print("p%d = %f %f %f\n", i, p1[0], p1[1], p1[2]);
		} //end for
	} //end if
	*dist = DotProduct(p0, normal);
} //end of the function Q3_SurfacePlane
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
q3_dplane_t *q3_surfaceplanes;

void Q3_CreatePlanarSurfacePlanes(void)
{
	int i;
	q3_dsurface_t *surface;

	Log_Print("creating planar surface planes...\n");
	q3_surfaceplanes = (q3_dplane_t *) GetClearedMemory(q3_numDrawSurfaces * sizeof(q3_dplane_t));

	for (i = 0; i < q3_numDrawSurfaces; i++)
	{
		surface = &q3_drawSurfaces[i];
		if (surface->surfaceType != MST_PLANAR) continue;
		Q3_SurfacePlane(surface, q3_surfaceplanes[i].normal, &q3_surfaceplanes[i].dist);
		//Log_Print("normal = %f %f %f, dist = %f\n", q3_surfaceplanes[i].normal[0],
		//											q3_surfaceplanes[i].normal[1],
		//											q3_surfaceplanes[i].normal[2], q3_surfaceplanes[i].dist);
	} //end for
} //end of the function Q3_CreatePlanarSurfacePlanes
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
/*
void Q3_SurfacePlane(q3_dsurface_t *surface, vec3_t normal, float *dist)
{
	//take the plane information from the lightmap vector
	//VectorCopy(surface->lightmapVecs[2], normal);
	//calculate plane dist with first surface vertex
//	*dist = DotProduct(q3_drawVerts[surface->firstVert].xyz, normal);
	Q3_PlaneFromPoints(q3_drawVerts[surface->firstVert].xyz,
						q3_drawVerts[surface->firstVert+1].xyz,
						q3_drawVerts[surface->firstVert+2].xyz, normal, dist);
} //end of the function Q3_SurfacePlane*/
//===========================================================================
// returns the amount the face and the winding overlap
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
float Q3_FaceOnWinding(q3_dsurface_t *surface, winding_t *winding)
{
	int i;
	float dist, area;
	q3_dplane_t plane;
	vec_t *v1, *v2;
	vec3_t normal, edgevec;
	winding_t *w;

	//copy the winding before chopping
	w = CopyWinding(winding);
	//retrieve the surface plane
	Q3_SurfacePlane(surface, plane.normal, &plane.dist);
	//chop the winding with the surface edge planes
	for (i = 0; i < surface->numVerts && w; i++)
	{
		v1 = q3_drawVerts[surface->firstVert + ((i) % surface->numVerts)].xyz;
		v2 = q3_drawVerts[surface->firstVert + ((i+1) % surface->numVerts)].xyz;
		//create a plane through the edge from v1 to v2, orthogonal to the
		//surface plane and with the normal vector pointing inward
		VectorSubtract(v2, v1, edgevec);
		CrossProduct(edgevec, plane.normal, normal);
		VectorNormalize(normal);
		dist = DotProduct(normal, v1);
		//
		ChopWindingInPlace(&w, normal, dist, -0.1); //CLIP_EPSILON
	} //end for
	if (w)
	{
		area = WindingArea(w);
		FreeWinding(w);
		return area;
	} //end if
	return 0;
} //end of the function Q3_FaceOnWinding
//===========================================================================
// creates a winding for the given brush side on the given brush
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
winding_t *Q3_BrushSideWinding(q3_dbrush_t *brush, q3_dbrushside_t *baseside)
{
	int i;
	q3_dplane_t *baseplane, *plane;
	winding_t *w;
	q3_dbrushside_t *side;
	
	//create a winding for the brush side with the given planenumber
	baseplane = &q3_dplanes[baseside->planeNum];
	w = BaseWindingForPlane(baseplane->normal, baseplane->dist);
	for (i = 0; i < brush->numSides && w; i++)
	{
		side = &q3_dbrushsides[brush->firstSide + i];
		//don't chop with the base plane
		if (side->planeNum == baseside->planeNum) continue;
		//also don't use planes that are almost equal
		plane = &q3_dplanes[side->planeNum];
		if (DotProduct(baseplane->normal, plane->normal) > 0.999
				&& fabs(baseplane->dist - plane->dist) < 0.01) continue;
		//
		plane = &q3_dplanes[side->planeNum^1];
		ChopWindingInPlace(&w, plane->normal, plane->dist, -0.1); //CLIP_EPSILON);
	} //end for
	return w;
} //end of the function Q3_BrushSideWinding
//===========================================================================
// fix screwed brush texture references
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
qboolean WindingIsTiny(winding_t *w);

void Q3_FindVisibleBrushSides(void)
{
	int i, j, k, we, numtextured, numsides;
	float dot;
	q3_dplane_t *plane;
	q3_dbrushside_t *brushside;
	q3_dbrush_t *brush;
	q3_dsurface_t *surface;
	winding_t *w;

	memset(q3_dbrushsidetextured, qfalse, Q3_MAX_MAP_BRUSHSIDES);
	//
	numsides = 0;
	//create planes for the planar surfaces
	Q3_CreatePlanarSurfacePlanes();
	Log_Print("searching visible brush sides...\n");
	Log_Print("%6d brush sides", numsides);
	//go over all the brushes
	for (i = 0; i < q3_numbrushes; i++)
	{
		brush = &q3_dbrushes[i];
		//go over all the sides of the brush
		for (j = 0; j < brush->numSides; j++)
		{
			qprintf("\r%6d", numsides++);
			if (brush->firstSide +j > Q3_MAX_MAP_BRUSHSIDES) {
				Error("\nQ3_MAX_MAP_BRUSHSIDES exceeded, %d > %d", brush->firstSide +j, Q3_MAX_MAP_BRUSHSIDES);
			}
			brushside = &q3_dbrushsides[brush->firstSide + j];
			//
			w = Q3_BrushSideWinding(brush, brushside);
			if (!w)
			{
				q3_dbrushsidetextured[brush->firstSide + j] = qtrue;
				continue;
			} //end if
			else
			{
				//RemoveEqualPoints(w, 0.2);
				if (WindingIsTiny(w))
				{
					FreeWinding(w);
					q3_dbrushsidetextured[brush->firstSide + j] = qtrue;
					continue;
				} //end if
				else
				{
					we = WindingError(w);
					if (we == WE_NOTENOUGHPOINTS
						|| we == WE_SMALLAREA
						|| we == WE_POINTBOGUSRANGE
//						|| we == WE_NONCONVEX
						)
					{
						FreeWinding(w);
						q3_dbrushsidetextured[brush->firstSide + j] = qtrue;
						continue;
					} //end if
				} //end else
			} //end else
			if (WindingArea(w) < 20)
			{
				q3_dbrushsidetextured[brush->firstSide + j] = qtrue;
				continue;
			} //end if
			//find a face for texturing this brush
			for (k = 0; k < q3_numDrawSurfaces; k++)
			{
				surface = &q3_drawSurfaces[k];
				if (surface->surfaceType != MST_PLANAR) continue;
				//
				//Q3_SurfacePlane(surface, plane.normal, &plane.dist);
				plane = &q3_surfaceplanes[k];
				//the surface plane and the brush side plane should be pretty much the same
				if (fabs(fabs(plane->dist) - fabs(q3_dplanes[brushside->planeNum].dist)) > 5) continue;
				dot = DotProduct(plane->normal, q3_dplanes[brushside->planeNum].normal);
				if (dot > -0.9 && dot < 0.9) continue;
				//if the face is partly or totally on the brush side
				if (Q3_FaceOnWinding(surface, w))
				{
					q3_dbrushsidetextured[brush->firstSide + j] = qtrue;
					//Log_Write("Q3_FaceOnWinding");
					break;
				} //end if
			} //end for
			FreeWinding(w);
		} //end for
	} //end for
	qprintf("\r%6d brush sides\n", numsides);
	numtextured = 0;
	for (i = 0; i < q3_numbrushsides; i++)
	{
		if (forcesidesvisible) q3_dbrushsidetextured[i] = qtrue;
		if (q3_dbrushsidetextured[i]) numtextured++;
	} //end for
	Log_Print("%d brush sides textured out of %d\n", numtextured, q3_numbrushsides);
} //end of the function Q3_FindVisibleBrushSides

/*
=============
Q3_SwapBlock

If all values are 32 bits, this can be used to swap everything
=============
*/
void Q3_SwapBlock( int *block, int sizeOfBlock ) {
	int		i;

	sizeOfBlock >>= 2;
	for ( i = 0 ; i < sizeOfBlock ; i++ ) {
		block[i] = LittleLong( block[i] );
	}
} //end of the function Q3_SwapBlock

/*
=============
Q3_SwapBSPFile

Byte swaps all data in a bsp file.
=============
*/
void Q3_SwapBSPFile( void ) {
	// models	
	Q3_SwapBlock( (int *)q3_dmodels, q3_nummodels * sizeof( q3_dmodels[0] ) );

	// shaders (don't swap the name)
	for ( int i = 0 ; i < q3_numShaders ; i++ ) {
		q3_dshaders[i].contentFlags = LittleLong( q3_dshaders[i].contentFlags );
		q3_dshaders[i].surfaceFlags = LittleLong( q3_dshaders[i].surfaceFlags );
	}

	// planes
	Q3_SwapBlock( (int *)q3_dplanes, q3_numplanes * sizeof( q3_dplanes[0] ) );
	
	// nodes
	Q3_SwapBlock( (int *)q3_dnodes, q3_numnodes * sizeof( q3_dnodes[0] ) );

	// leafs
	Q3_SwapBlock( (int *)q3_dleafs, q3_numleafs * sizeof( q3_dleafs[0] ) );

	// leaffaces
	Q3_SwapBlock( (int *)q3_dleafsurfaces, q3_numleafsurfaces * sizeof( q3_dleafsurfaces[0] ) );

	// leafbrushes
	Q3_SwapBlock( (int *)q3_dleafbrushes, q3_numleafbrushes * sizeof( q3_dleafbrushes[0] ) );

	// brushes
	Q3_SwapBlock( (int *)q3_dbrushes, q3_numbrushes * sizeof( q3_dbrushes[0] ) );

	// brushsides
	Q3_SwapBlock( (int *)q3_dbrushsides, q3_numbrushsides * sizeof( q3_dbrushsides[0] ) );

	// vis
	int* q3_vis = (int *)&q3_visBytes;
	q3_vis[0] = LittleLong( q3_vis[0] );
	q3_vis[1] = LittleLong( q3_vis[1] );

	// drawverts (don't swap colors )
	for ( int i = 0 ; i < q3_numDrawVerts ; i++ ) {
		q3_drawVerts[i].lightmap[0] = LittleFloat( q3_drawVerts[i].lightmap[0] );
		q3_drawVerts[i].lightmap[1] = LittleFloat( q3_drawVerts[i].lightmap[1] );
		q3_drawVerts[i].st[0] = LittleFloat( q3_drawVerts[i].st[0] );
		q3_drawVerts[i].st[1] = LittleFloat( q3_drawVerts[i].st[1] );
		q3_drawVerts[i].xyz[0] = LittleFloat( q3_drawVerts[i].xyz[0] );
		q3_drawVerts[i].xyz[1] = LittleFloat( q3_drawVerts[i].xyz[1] );
		q3_drawVerts[i].xyz[2] = LittleFloat( q3_drawVerts[i].xyz[2] );
		q3_drawVerts[i].normal[0] = LittleFloat( q3_drawVerts[i].normal[0] );
		q3_drawVerts[i].normal[1] = LittleFloat( q3_drawVerts[i].normal[1] );
		q3_drawVerts[i].normal[2] = LittleFloat( q3_drawVerts[i].normal[2] );
	}

	// drawindexes
	Q3_SwapBlock( (int *)q3_drawIndexes, q3_numDrawIndexes * sizeof( q3_drawIndexes[0] ) );

	// drawsurfs
	Q3_SwapBlock( (int *)q3_drawSurfaces, q3_numDrawSurfaces * sizeof( q3_drawSurfaces[0] ) );

	// fogs
	for ( int i = 0 ; i < q3_numFogs ; i++ ) {
		q3_dfogs[i].brushNum = LittleLong( q3_dfogs[i].brushNum );
	}
}



/*
=============
Q3_CopyLump
=============
*/
int Q3_CopyLump( q3_dheader_t	*header, int lump, void **dest, int size ) {
	int length = header->lumps[lump].filelen;
	int ofs = header->lumps[lump].fileofs;
	
	if ( length % size ) {
		Error ("Q3_LoadBSPFile: odd lump size");
	}

	*dest = GetMemory(length);

	memcpy( *dest, (byte *)header + ofs, length );

	return length / size;
}

/*
=============
CountTriangles
=============
*/
void CountTriangles( void ) {
	int i, numTris, numPatchTris;
	q3_dsurface_t *surface;

	numTris = numPatchTris = 0;
	for ( i = 0; i < q3_numDrawSurfaces; i++ ) {
		surface = &q3_drawSurfaces[i];

		numTris += surface->numIndexes / 3;

		if ( surface->patchWidth ) {
			numPatchTris += surface->patchWidth * surface->patchHeight * 2;
		}
	}

	Log_Print( "%6d triangles\n", numTris );
	Log_Print( "%6d patch tris\n", numPatchTris );
}

/*
=============
Q3R_ConvertBSPData
=============
*/
static void Q3R_ConvertBSPData(void)
{
	q3_numbrushsides = q3r_numbrushsides;
	q3_dbrushsides = (q3_dbrushside_t*)GetMemory(q3_numbrushsides * sizeof(q3_dbrushside_t));
	for( int i = 0; i < q3_numbrushsides; i++ ) {
		q3_dbrushsides[i].planeNum = q3r_dbrushsides[i].planeNum;
		q3_dbrushsides[i].shaderNum = q3r_dbrushsides[i].shaderNum;
	}

	q3_numDrawVerts = q3r_numDrawVerts;
	q3_drawVerts = (q3_drawVert_t*)GetMemory(q3_numDrawVerts * sizeof(q3_drawVert_t));
	for( int i = 0; i < q3_numDrawVerts; i++ ) {
		for( int j = 0; j < 3; j++ ) {
			q3_drawVerts[i].xyz[j] = q3r_drawVerts[i].xyz[j];
			q3_drawVerts[i].normal[j] = q3r_drawVerts[i].normal[j];
		}
		for( int j = 0; j < 2; j++ ) {
			q3_drawVerts[i].st[j] = q3r_drawVerts[i].st[j];
		}
		for( int j = 0; j < 2; j++ ) {
			q3_drawVerts[i].lightmap[j] = q3r_drawVerts[i].lightmap[0][j];
		}
		for( int j = 0; j < 4; j++ ) {
			q3_drawVerts[i].color[j] = q3r_drawVerts[i].color[0][j];
		}
	}

	q3_numDrawSurfaces = q3r_numDrawSurfaces;
	q3_drawSurfaces = (q3_dsurface_t*)GetMemory(q3_numDrawSurfaces * sizeof(q3_dsurface_t));
	for( int i = 0; i < q3_numDrawSurfaces; i++ ) {
		q3_drawSurfaces[i].shaderNum = q3r_drawSurfaces[i].shaderNum;
		q3_drawSurfaces[i].fogNum = q3r_drawSurfaces[i].fogNum;
		q3_drawSurfaces[i].surfaceType = q3r_drawSurfaces[i].surfaceType;

		q3_drawSurfaces[i].firstVert = q3r_drawSurfaces[i].firstVert;
		q3_drawSurfaces[i].numVerts = q3r_drawSurfaces[i].numVerts;

		q3_drawSurfaces[i].firstIndex = q3r_drawSurfaces[i].firstIndex;
		q3_drawSurfaces[i].numIndexes = q3r_drawSurfaces[i].numIndexes;

		q3_drawSurfaces[i].lightmapNum = q3r_drawSurfaces[i].lightmapNum[0];
		q3_drawSurfaces[i].lightmapX = q3r_drawSurfaces[i].lightmapXY[0][0];
		q3_drawSurfaces[i].lightmapY = q3r_drawSurfaces[i].lightmapXY[0][1];
		q3_drawSurfaces[i].lightmapWidth = q3r_drawSurfaces[i].lightmapWidth;
		q3_drawSurfaces[i].lightmapHeight = q3r_drawSurfaces[i].lightmapHeight;

		for( int j = 0; j < 3; j++ ) {
			q3_drawSurfaces[i].lightmapOrigin[j] = q3r_drawSurfaces[i].lightmapOrigin[j];
			q3_drawSurfaces[i].lightmapVecs[0][j] = q3r_drawSurfaces[i].lightmapVecs[0][j];
			q3_drawSurfaces[i].lightmapVecs[1][j] = q3r_drawSurfaces[i].lightmapVecs[1][j];
			q3_drawSurfaces[i].lightmapVecs[2][j] = q3r_drawSurfaces[i].lightmapVecs[2][j];
		}

		q3_drawSurfaces[i].patchWidth = q3r_drawSurfaces[i].patchWidth;
		q3_drawSurfaces[i].patchHeight = q3r_drawSurfaces[i].patchHeight;
	}
}

/*
=============
Q3_LoadBSPFile
=============
*/
void	Q3_LoadBSPFile(struct quakefile_s *qf)
{
	q3_dheader_t	*header;
	int raven;

	// load the file header
	//LoadFile(filename, (void **)&header, offset, length);
	//
	LoadQuakeFile(qf, (void **)&header);

	// swap the header
	Q3_SwapBlock( (int *)header, sizeof(*header) );

	if ( header->ident != Q3_BSP_IDENT && header->ident != QL_BSP_IDENT && header->ident != QF_BSP_IDENT ) {
		Error( "%s is not a IBSP or a FBSP file", qf->filename );
	}

	if( header->ident == QF_BSP_IDENT ) {
		if ( header->version != QF_BSP_VERSION ) {
			Error( "%s is version %i, not %i", qf->filename, header->version, QF_BSP_VERSION );
		}
		raven = 1;
	}
	else {
		if ( header->version != Q3_BSP_VERSION && header->version != QL_BSP_VERSION ) {
			Error( "%s is version %i, not (%i or %i)", qf->filename, header->version, Q3_BSP_VERSION, QL_BSP_VERSION );
		}
		raven = 0;
	}

	q3_numShaders = Q3_CopyLump( header, Q3_LUMP_SHADERS, (void**)&q3_dshaders, sizeof(q3_dshader_t) );
	q3_nummodels = Q3_CopyLump( header, Q3_LUMP_MODELS, (void **) &q3_dmodels, sizeof(q3_dmodel_t) );
	q3_numplanes = Q3_CopyLump( header, Q3_LUMP_PLANES, (void **) &q3_dplanes, sizeof(q3_dplane_t) );
	q3_numleafs = Q3_CopyLump( header, Q3_LUMP_LEAFS, (void **) &q3_dleafs, sizeof(q3_dleaf_t) );
	q3_numnodes = Q3_CopyLump( header, Q3_LUMP_NODES, (void **) &q3_dnodes, sizeof(q3_dnode_t) );
	q3_numleafsurfaces = Q3_CopyLump( header, Q3_LUMP_LEAFSURFACES, (void **) &q3_dleafsurfaces, sizeof(q3_dleafsurfaces[0]) );
	q3_numleafbrushes = Q3_CopyLump( header, Q3_LUMP_LEAFBRUSHES, (void **) &q3_dleafbrushes, sizeof(q3_dleafbrushes[0]) );
	q3_numbrushes = Q3_CopyLump( header, Q3_LUMP_BRUSHES, (void **) &q3_dbrushes, sizeof(q3_dbrush_t) );
	q3_numFogs = Q3_CopyLump( header, Q3_LUMP_FOGS, (void **) &q3_dfogs, sizeof(q3_dfog_t) );
	q3_numDrawIndexes = Q3_CopyLump( header, Q3_LUMP_DRAWINDEXES, (void **) &q3_drawIndexes, sizeof(q3_drawIndexes[0]) );

	if( raven ) {
		q3r_numbrushsides = Q3_CopyLump( header, Q3_LUMP_BRUSHSIDES, (void **) &q3r_dbrushsides, sizeof(q3r_dbrushside_t) );
		q3r_numDrawVerts = Q3_CopyLump( header, Q3_LUMP_DRAWVERTS, (void **) &q3r_drawVerts, sizeof(q3r_drawVert_t) );
		q3r_numDrawSurfaces = Q3_CopyLump( header, Q3_LUMP_SURFACES, (void **) &q3r_drawSurfaces, sizeof(q3r_dsurface_t) );
	}
	else {
		q3_numbrushsides = Q3_CopyLump( header, Q3_LUMP_BRUSHSIDES, (void **) &q3_dbrushsides, sizeof(q3_dbrushside_t) );
		q3_numDrawVerts = Q3_CopyLump( header, Q3_LUMP_DRAWVERTS, (void **) &q3_drawVerts, sizeof(q3_drawVert_t) );
		q3_numDrawSurfaces = Q3_CopyLump( header, Q3_LUMP_SURFACES, (void **) &q3_drawSurfaces, sizeof(q3_dsurface_t) );
	}

	q3_numVisBytes = Q3_CopyLump( header, Q3_LUMP_VISIBILITY, (void **) &q3_visBytes, 1 );
	q3_numLightBytes = Q3_CopyLump( header, Q3_LUMP_LIGHTMAPS, (void **) &q3_lightBytes, 1 );
	q3_entdatasize = Q3_CopyLump( header, Q3_LUMP_ENTITIES, (void **) &q3_dentdata, 1);

	q3_numGridBytes = Q3_CopyLump( header, Q3_LUMP_LIGHTGRID, (void **) &q3_gridData, 1 );

	if( raven ) {
		Q3R_ConvertBSPData();
	}

	CountTriangles();

	FreeMemory( header );		// everything has been copied out

	// swap everything
	Q3_SwapBSPFile();

	Q3_FindVisibleBrushSides();

	//Q3_PrintBSPFileSizes();
}


//============================================================================

/*
=============
Q3_PrintBSPFileSizes

Dumps info about current file
=============
*/
void Q3_PrintBSPFileSizes( void )
{
	if ( !num_entities )
	{
		Q3_ParseEntities();
	}

	Log_Print ("%6i models       %7i\n"
		,q3_nummodels, (int)(q3_nummodels*sizeof(q3_dmodel_t)));
	Log_Print ("%6i shaders      %7i\n"
		,q3_numShaders, (int)(q3_numShaders*sizeof(q3_dshader_t)));
	Log_Print ("%6i brushes      %7i\n"
		,q3_numbrushes, (int)(q3_numbrushes*sizeof(q3_dbrush_t)));
	Log_Print ("%6i brushsides   %7i\n"
		,q3_numbrushsides, (int)(q3_numbrushsides*sizeof(q3_dbrushside_t)));
	Log_Print ("%6i fogs         %7i\n"
		,q3_numFogs, (int)(q3_numFogs*sizeof(q3_dfog_t)));
	Log_Print ("%6i planes       %7i\n"
		,q3_numplanes, (int)(q3_numplanes*sizeof(q3_dplane_t)));
	Log_Print ("%6i entdata      %7i\n", num_entities, q3_entdatasize);

	Log_Print ("\n");

	Log_Print ("%6i nodes        %7i\n"
		,q3_numnodes, (int)(q3_numnodes*sizeof(q3_dnode_t)));
	Log_Print ("%6i leafs        %7i\n"
		,q3_numleafs, (int)(q3_numleafs*sizeof(q3_dleaf_t)));
	Log_Print ("%6i leafsurfaces %7i\n"
		,q3_numleafsurfaces, (int)(q3_numleafsurfaces*sizeof(q3_dleafsurfaces[0])));
	Log_Print ("%6i leafbrushes  %7i\n"
		,q3_numleafbrushes, (int)(q3_numleafbrushes*sizeof(q3_dleafbrushes[0])));
	Log_Print ("%6i drawverts    %7i\n"
		,q3_numDrawVerts, (int)(q3_numDrawVerts*sizeof(q3_drawVerts[0])));
	Log_Print ("%6i drawindexes  %7i\n"
		,q3_numDrawIndexes, (int)(q3_numDrawIndexes*sizeof(q3_drawIndexes[0])));
	Log_Print ("%6i drawsurfaces %7i\n"
		,q3_numDrawSurfaces, (int)(q3_numDrawSurfaces*sizeof(q3_drawSurfaces[0])));

	Log_Print ("%6i lightmaps    %7i\n"
		,q3_numLightBytes / (LIGHTMAP_WIDTH*LIGHTMAP_HEIGHT*3), q3_numLightBytes );
	Log_Print ("       visibility   %7i\n"
		, q3_numVisBytes );
}

/*
================
Q3_ParseEntities

Parses the q3_dentdata string into entities
================
*/
void Q3_ParseEntities (void)
{
	num_entities = 0;
	script_t *script = LoadScriptMemory(q3_dentdata, q3_entdatasize, "*Quake3 bsp file");
	SetScriptFlags(script, SCFL_NOSTRINGWHITESPACES |
									SCFL_NOSTRINGESCAPECHARS);

	while(ParseEntity(script))
	{
	} //end while

	FreeScript(script);
} //end of the function Q3_ParseEntities


