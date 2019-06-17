/*
** git $Id: 560fb376ff8a4576170ebcd4b459de6bcce908f6 $
** svn $Id: overflow.h 937 2019-01-28 06:13:04Z arango $
*******************************************************************************
** Copyright (c) 2002-2019 The ROMS/TOMS Group                               **
**   Licensed under a MIT/X style license                                    **
**   See License_ROMS.txt                                                    **
*******************************************************************************
**
** Options for Gravitational Overflow Test.
**
** Application flag:   OVERFLOW
** Input script:       roms_overflow.in
*/

#define UV_ADV
#define UV_COR
#define UV_QDRAG
#define UV_VIS2
#define MIX_S_UV
#define DJ_GRADPS
#define SPLINES_VDIFF
#define SPLINES_VVISC
#define TS_U3HADVECTION
#define TS_SVADVECTION
#define TS_DIF2
#define MIX_ISO_TS
#define SOLVE3D
#define AVERAGES
#define ANA_GRID
#define ANA_INITIAL
#define ANA_SMFLUX
#define ANA_STFLUX
#define ANA_BTFLUX
