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
#include "q2files.h"
#include "l_bsp_q2.h"
#include "l_bsp_ent.h"

#define q2_dmodel_t			dmodel_t
#define q2_lump_t				lump_t
#define q2_dheader_t			dheader_t
#define q2_dmodel_t			dmodel_t
#define q2_dvertex_t			dvertex_t
#define q2_dplane_t			dplane_t
#define q2_dnode_t			dnode_t
#define q2_texinfo_t			texinfo_t
#define q2_dedge_t			dedge_t
#define q2_dface_t			dface_t
#define q2_dleaf_t			dleaf_t
#define q2_dbrushside_t		dbrushside_t
#define q2_dbrush_t			dbrush_t
#define q2_dvis_t				dvis_t
#define q2_dareaportal_t	dareaportal_t
#define q2_darea_t			darea_t

#define q2_nummodels			nummodels
#define q2_dmodels			dmodels
#define q2_numleafs			numleafs
#define q2_dleafs				dleafs
#define q2_numplanes			numplanes
#define q2_dplanes			dplanes
#define q2_numvertexes		numvertexes
#define q2_dvertexes			dvertexes
#define q2_numnodes			numnodes
#define q2_dnodes				dnodes
#define q2_numtexinfo		numtexinfo
#define q2_texinfo			texinfo
#define q2_numfaces			numfaces
#define q2_dfaces				dfaces
#define q2_numedges			numedges
#define q2_dedges				dedges
#define q2_numleaffaces		numleaffaces
#define q2_dleaffaces		dleaffaces
#define q2_numleafbrushes	numleafbrushes
#define q2_dleafbrushes		dleafbrushes
#define q2_dsurfedges		dsurfedges
#define q2_numbrushes		numbrushes
#define q2_dbrushes			dbrushes
#define q2_numbrushsides	numbrushsides
#define q2_dbrushsides		dbrushsides
#define q2_numareas			numareas
#define q2_dareas				dareas
#define q2_numareaportals	numareaportals
#define q2_dareaportals		dareaportals


//=============================================================================

int				nummodels;
dmodel_t			*dmodels;//[MAX_MAP_MODELS];

int				visdatasize;
byte				*dvisdata;//[MAX_MAP_VISIBILITY];
dvis_t			*dvis;// = (dvis_t *)dvisdata;

int				lightdatasize;
byte				*dlightdata;//[MAX_MAP_LIGHTING];

int				entdatasize;
char				*dentdata;//[MAX_MAP_ENTSTRING];

int				numleafs;
dleaf_t			*dleafs;//[MAX_MAP_LEAFS];

int				numplanes;
dplane_t			*dplanes;//[MAX_MAP_PLANES];

int				numvertexes;
dvertex_t		*dvertexes;//[MAX_MAP_VERTS];

int				numnodes;
dnode_t			*dnodes;//[MAX_MAP_NODES];

//NOTE: must be static for q2 .map to q2 .bsp
int				numtexinfo;
texinfo_t		texinfo[MAX_MAP_TEXINFO];

int				numfaces;
dface_t			*dfaces;//[MAX_MAP_FACES];

int				numedges;
dedge_t			*dedges;//[MAX_MAP_EDGES];

int				numleaffaces;
unsigned short	*dleaffaces;//[MAX_MAP_LEAFFACES];

int				numleafbrushes;
unsigned short	*dleafbrushes;//[MAX_MAP_LEAFBRUSHES];

int				numsurfedges;
int				*dsurfedges;//[MAX_MAP_SURFEDGES];

int				numbrushes;
dbrush_t			*dbrushes;//[MAX_MAP_BRUSHES];

int				numbrushsides;
dbrushside_t	*dbrushsides;//[MAX_MAP_BRUSHSIDES];

int				numareas;
darea_t			*dareas;//[MAX_MAP_AREAS];

int				numareaportals;
dareaportal_t	*dareaportals;//[MAX_MAP_AREAPORTALS];

#define MAX_MAP_DPOP			256
byte				dpop[MAX_MAP_DPOP];

//
char brushsidetextured[MAX_MAP_BRUSHSIDES];

//#ifdef ME

int bspallocated = qfalse;
int allocatedbspmem = 0;

//============================================================================

FILE		*wadfile;
dheader_t	outheader;
