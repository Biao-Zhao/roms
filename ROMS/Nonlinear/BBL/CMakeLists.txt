# git $Id$
# svn $Id: CMakeLists.txt 1185 2023-08-01 21:42:38Z arango $
#:::::::::::::::::::::::::::::::::::::::::::::::::::::: David Robertson :::
# Copyright (c) 2002-2023 The ROMS/TOMS Group                           :::
#   Licensed under a MIT/X style license                                :::
#   See License_ROMS.md                                                 :::
#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#
# Source code list for sub-directory "ROMS/Nonlinear"

list( APPEND _files
      ROMS/Nonlinear/BBL/bbl.F
      ROMS/Nonlinear/BBL/bbl_output.F
)

set ( ROMS_Nonlinear_BBL_files
      ${_files}
      PARENT_SCOPE
)
