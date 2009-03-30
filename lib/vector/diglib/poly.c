/*
 ****************************************************************************
 *
 * MODULE:       Vector library 
 *              
 * AUTHOR(S):    Original author CERL, probably Dave Gerdes.
 *               Update to GRASS 5.7 Radim Blazek.
 *               Update to GRASS 7.0 Markus Metz
 *
 * PURPOSE:      Lower level functions for reading/writing/manipulating vectors.
 *
 * COPYRIGHT:    (C) 2009 by the GRASS Development Team
 *
 *               This program is free software under the GNU General Public
 *               License (>=v2). Read the file COPYING that comes with GRASS
 *               for details.
 *
 *****************************************************************************/
#include <math.h>
#include <grass/Vect.h>


#ifndef HUGE_VAL
#define HUGE_VAL 9999999999999.0
#endif

/*
 * fills BPoints (must be inited previously) by points from input
 * array LPoints
 * 
 * each input LPoints[i] must have at least 2 points
 *  
 * returns number of points or -1 on error
 */


int dig_get_poly_points(int n_lines, struct line_pnts **LPoints,
	    int *direction,	/* line direction: > 0 or < 0 */
	    struct line_pnts *BPoints)
{
    register int i, j, point, start, end, inc;
    struct line_pnts *Points;
    int n_points;

    BPoints->n_points = 0;

    if (n_lines < 1) {
	return 0;
    }

    /* Calc required space */
    n_points = 0;
    for (i = 0; i < n_lines; i++) {
	Points = LPoints[i];
	n_points += Points->n_points - 1;  /* each line from first to last - 1 */
    }
    n_points++;			/* last point */

    if (0 > dig_alloc_points(BPoints, n_points))
	return (-1);

    point = 0;
    j = 0;
    for (i = 0; i < n_lines; i++) {
	Points = LPoints[i];
	if (direction[i] > 0) {
	    start = 0;
	    end = Points->n_points - 1;
	    inc = 1;
	}
	else {
	    start = Points->n_points - 1;
	    end = 0;
	    inc = -1;
	}

	for (j = start; j != end; j += inc) {
	    BPoints->x[point] = Points->x[j];
	    BPoints->y[point] = Points->y[j];
	    point++;
	}
    }
    /* last point */
    BPoints->x[point] = Points->x[j];
    BPoints->y[point] = Points->y[j];

    BPoints->n_points = n_points;

    return (BPoints->n_points);
}

/*
 * calculate signed area size for polygon
 *
 * points must be closed polygon with first point = last point
 *
 * formula modified from:
 * Sunday, Daniel. 2002. Fast Polygon Area and Newell Normal Computation.
 * Journal of Graphics Tools; 7(2):9-13.
 *
 * returns signed area, positive for clockwise, negative for
 * counterclockwise, 0 for degenerate
 */
int dig_find_area_poly(struct line_pnts *Points, double *totalarea)
{
    int i, n = Points->n_points - 1;
    double *x, *y;
    double tot_area;

    /* TODO: check if results are still accurate without pruning *Points first
     * consecutive duplicate vertices should in theory result in wrong area size */

    x = Points->x;
    y = Points->y;

    /* first point 0 == point n */
    tot_area = y[0] * (x[1] - x[n - 1]);
    for (i = 1; i < n; i++) {
        tot_area += y[i] * (x[i + 1] - x[i - 1]);
    }

    *totalarea = 0.5 * tot_area;

    return (0);
}

/*
 * find orientation of polygon (clockwise or counterclockwise)
 * faster than signed area for > 4 vertices
 *
 * points must be closed polygon with first point = last point
 *
 * this code uses bits and pieces from softSurfer and GEOS
 * (C) 2000 softSurfer (www.softsurfer.com)
 * (C) 2006 Refractions Research Inc.
 *
 * and now copes with partially collapsed boundaries
 * the code is long but fast
 * it can be written much shorter, but that comes with speed penalty
 *
 * returns orientation, positive for CW, negative for CCW, 0 for degenerate
 */
double dig_find_poly_orientation(struct line_pnts *Points)
{
    unsigned int pnext, pprev, pcur = 0;
    unsigned int lastpoint = Points->n_points - 1;
    double *x, *y, orientation;

    x = Points->x;
    y = Points->y;

    /* first find leftmost highest vertex of the polygon */
    for (pnext = 1; pnext < lastpoint; pnext++) {
	if (y[pnext] < y[pcur])
	    continue;
	else if (y[pnext] == y[pcur]) {    /* just as high */
	    if (x[pnext] > x[pcur])   	   /* but to the right */
		continue;
	}
	pcur = pnext;          /* a new leftmost highest vertex */
    }

    /* Points are not pruned, so ... */
    pnext = pcur;
    pprev = pcur;

    /* find next distinct point */
    do {
	if (pnext < lastpoint - 1)
	    pnext++;
	else
	    pnext = 0;
    } while (pnext != pcur && x[pcur] == x[pnext] && y[pcur] == y[pnext]);

    /* find previous distinct point */
    do {
	if (pprev > 0)
	    pprev--;
	else
	    pprev = lastpoint - 1;
    } while (pprev != pcur && x[pcur] == x[pprev] && y[pcur] == y[pprev]);
    
    /* orientation at vertex pcur == signed area for triangle pprev, pcur, pnext
     * rather use robust determinant of Olivier Devillers? */
    orientation =  (x[pnext] - x[pprev]) * (y[pcur] - y[pprev])
         - (x[pcur] - x[pprev]) * (y[pnext] - y[pprev]);

    if (orientation)
	return orientation;

    /* orientation is 0, can happen with dirty boundaries, next check */
    /* find rightmost highest vertex of the polygon */
    pcur = 0;
    for (pnext = 1; pnext < lastpoint; pnext++) {
	if (y[pnext] < y[pcur])
	    continue;
	else if (y[pnext] == y[pcur]) {    /* just as high */
	    if (x[pnext] < x[pcur])   	/* but to the left */
		continue;
	}
	pcur = pnext;          /* a new rightmost highest vertex */
    }

    /* Points are not pruned, so ... */
    pnext = pcur;
    pprev = pcur;

    /* find next distinct point */
    do {
	if (pnext < lastpoint - 1)
	    pnext++;
	else
	    pnext = 0;
    } while (pnext != pcur && x[pcur] == x[pnext] && y[pcur] == y[pnext]);

    /* find previous distinct point */
    do {
	if (pprev > 0)
	    pprev--;
	else
	    pprev = lastpoint - 1;
    } while (pprev != pcur && x[pcur] == x[pprev] && y[pcur] == y[pprev]);
    
    /* orientation at vertex pcur == signed area for triangle pprev, pcur, pnext
     * rather use robust determinant of Olivier Devillers? */
    orientation =  (x[pnext] - x[pprev]) * (y[pcur] - y[pprev])
         - (x[pcur] - x[pprev]) * (y[pnext] - y[pprev]);

    if (orientation)
	return orientation;

    /* orientation is 0, next check */
    /* find leftmost lowest vertex of the polygon */
    pcur = 0;
    for (pnext = 1; pnext < lastpoint; pnext++) {
	if (y[pnext] > y[pcur])
	    continue;
	else if (y[pnext] == y[pcur]) {    /* just as low */
	    if (x[pnext] > x[pcur])   	/* but to the right */
		continue;
	}
	pcur = pnext;          /* a new leftmost lowest vertex */
    }

    /* Points are not pruned, so ... */
    pnext = pcur;
    pprev = pcur;

    /* find next distinct point */
    do {
	if (pnext < lastpoint - 1)
	    pnext++;
	else
	    pnext = 0;
    } while (pnext != pcur && x[pcur] == x[pnext] && y[pcur] == y[pnext]);

    /* find previous distinct point */
    do {
	if (pprev > 0)
	    pprev--;
	else
	    pprev = lastpoint - 1;
    } while (pprev != pcur && x[pcur] == x[pprev] && y[pcur] == y[pprev]);
    
    /* orientation at vertex pcur == signed area for triangle pprev, pcur, pnext
     * rather use robust determinant of Olivier Devillers? */
    orientation =  (x[pnext] - x[pprev]) * (y[pcur] - y[pprev])
         - (x[pcur] - x[pprev]) * (y[pnext] - y[pprev]);

    if (orientation)
	return orientation;

    /* orientation is 0, last check */
    /* find rightmost lowest vertex of the polygon */
    pcur = 0;
    for (pnext = 1; pnext < lastpoint; pnext++) {
	if (y[pnext] > y[pcur])
	    continue;
	else if (y[pnext] == y[pcur]) {    /* just as low */
	    if (x[pnext] < x[pcur])   	/* but to the left */
		continue;
	}
	pcur = pnext;          /* a new rightmost lowest vertex */
    }

    /* Points are not pruned, so ... */
    pnext = pcur;
    pprev = pcur;

    /* find next distinct point */
    do {
	if (pnext < lastpoint - 1)
	    pnext++;
	else
	    pnext = 0;
    } while (pnext != pcur && x[pcur] == x[pnext] && y[pcur] == y[pnext]);

    /* find previous distinct point */
    do {
	if (pprev > 0)
	    pprev--;
	else
	    pprev = lastpoint - 1;
    } while (pprev != pcur && x[pcur] == x[pprev] && y[pcur] == y[pprev]);
    
    /* orientation at vertex pcur == signed area for triangle pprev, pcur, pnext
     * rather use robust determinant of Olivier Devillers? */
    orientation =  (x[pnext] - x[pprev]) * (y[pcur] - y[pprev])
         - (x[pcur] - x[pprev]) * (y[pnext] - y[pprev]);

    return orientation;   /* 0 for degenerate */
}
