# git $Id$
# svn $Id: compiler_flags_GNU_Fortran.cmake 1178 2023-07-11 17:50:57Z arango $
#:::::::::::::::::::::::::::::::::::::::::::::::::::::: David Robertson :::
# Copyright (c) 2002-2023 The ROMS/TOMS Group                           :::
#   Licensed under a MIT/X style license                                :::
#   See License_ROMS.md                                                 :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#
# CMake Flags for the GNU Fortran Compiler.

###########################################################################
# Set the name of the Fortran compiler based on FORT and USE_MPI
###########################################################################

if( MPI )
  set( CMAKE_Fortran_COMPILER mpif90 )
else()
  set( CMAKE_Fortran_COMPILER gfortran )
endif()

###########################################################################
# FLAGS COMMON TO ALL BUILD TYPES
###########################################################################

set( CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -frepack-arrays -fallow-argument-mismatch" )

###########################################################################
# RELEASE FLAGS
###########################################################################

set( CMAKE_Fortran_FLAGS_RELEASE "-O3 -ffast-math" )

###########################################################################
# RELEASE WITH DEBUG INFORMATION FLAGS
###########################################################################

set( CMAKE_Fortran_FLAGS_RELWITHDEBINFO "-O3 -g -ffast-math -fbounds-check -fbacktrace -fcheck=all" )

###########################################################################
# DEBUG FLAGS
###########################################################################

set( CMAKE_Fortran_FLAGS_DEBUG   "-O0 -g -fbounds-check -fbacktrace -fcheck=all -finit-real=nan -ffpe-trap=invalid,zero,overflow" )

###########################################################################
# BIT REPRODUCIBLE FLAGS
###########################################################################

set( CMAKE_Fortran_FLAGS_BIT     "-O2 -fbounds-check" )

###########################################################################
# LINK FLAGS
###########################################################################

if( APPLE )
  message( STATUS "MacOS so setting -Wl,-undefined dynamic_lookup linker flag" )
  set( CMAKE_SHARED_LINKER_FLAGS    "-Wl,-undefined,dynamic_lookup" )
endif()

set( CMAKE_Fortran_LINK_FLAGS    "" )

###########################################################################
# ROMS Definitions
###########################################################################

# Special flags for files that may contain very long lines.

set( ROMS_FREEFLAGS "-ffree-form -ffree-line-length-none" )

# Special flag for def_var since gfortran is confused by "dimension(*)"

set( ROMS_NOBOUNDSFLAG "-fno-bounds-check" )

# Special defines for "mod_strings.F"

set( my_fort   "gfortran" )
set( my_fc     "${CMAKE_Fortran_COMPILER}" )

# Flags for the C-preprocessor

set( CPPFLAGS  "-P" "--traditional-cpp" "-w" )

###########################################################################
