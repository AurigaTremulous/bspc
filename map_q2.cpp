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

//===========================================================================
// ANSI, Area Navigational System Interface
// AAS,  Area Awareness System
//===========================================================================

#include "local.h"
#include "qbsp.h"
#include "l_mem.h"
#include "botlib/aasfile.h"			//aas_bbox_t
#include "aas_store.h"		//AAS_MAX_BBOXES
#include "aas_cfg.h"
#include "aas_map.h"			//AAS_CreateMapBrushes
#include "l_bsp_q2.h"


#ifdef ME

#define NODESTACKSIZE		1024

int nodestack[NODESTACKSIZE];
int *nodestackptr;
int nodestacksize = 0;
int brushmodelnumbers[MAX_MAPFILE_BRUSHES];
int dbrushleafnums[MAX_MAPFILE_BRUSHES];
int dplanes2mapplanes[MAX_MAPFILE_PLANES];

#endif //ME

//====================================================================


#ifdef ME

#define BBOX_NORMAL_EPSILON			0.0001

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void MarkVisibleBrushSides(mapbrush_t *brush)
{
	int n, i, planenum;
	side_t *side;
	dface_t *face;
	//
	for (n = 0; n < brush->numsides; n++)
	{
		side = brush->original_sides + n;
		//if this side is a bevel or the leaf number of the brush is unknown
		if ((side->flags & SFL_BEVEL) || brush->leafnum < 0)
		{
			//this side is a valid splitter
			side->flags |= SFL_VISIBLE;
			continue;
		} //end if
		//assum this side will not be used as a splitter
		side->flags &= ~SFL_VISIBLE;
		//check if the side plane is used by a visible face
		for (i = 0; i < numfaces; i++)
		{
			face = &dfaces[i];
			planenum = dplanes2mapplanes[face->planenum];
			if ((planenum & ~1) == (side->planenum & ~1))
			{
				//this side is a valid splitter
				side->flags |= SFL_VISIBLE;
			} //end if
		} //end for
	} //end for
} //end of the function MarkVisibleBrushSides

#endif //ME

//===================================================================


#ifdef ME		//Begin MAP loading from BSP file
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void Q2_SetLeafBrushesModelNumbers(int leafnum, int modelnum)
{
	int i, brushnum;
	dleaf_t *leaf;

	leaf = &dleafs[leafnum];
	for (i = 0; i < leaf->numleafbrushes; i++)
	{
		brushnum = dleafbrushes[leaf->firstleafbrush + i];
		brushmodelnumbers[brushnum] = modelnum;
		dbrushleafnums[brushnum] = leafnum;
	} //end for
} //end of the function Q2_SetLeafBrushesModelNumbers
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void Q2_InitNodeStack(void)
{
	nodestackptr = nodestack;
	nodestacksize = 0;
} //end of the function Q2_InitNodeStack
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void Q2_PushNodeStack(int num)
{
	*nodestackptr = num;
	nodestackptr++;
	nodestacksize++;
	//
	if (nodestackptr >= &nodestack[NODESTACKSIZE])
	{
		Error("Q2_PushNodeStack: stack overflow\n");
	} //end if
} //end of the function Q2_PushNodeStack
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int Q2_PopNodeStack(void)
{
	//if the stack is empty
	if (nodestackptr <= nodestack) return -1;
	//decrease stack pointer
	nodestackptr--;
	nodestacksize--;
	//return the top value from the stack
	return *nodestackptr;
} //end of the function Q2_PopNodeStack

//End MAP loading from BSP file
#endif //ME
