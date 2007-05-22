      SUBROUTINE ana_scope (ng, tile, model)
!
!! svn $Id$
!!======================================================================
!! Copyright (c) 2002-2007 The ROMS/TOMS Group                         !
!!   Licensed under a MIT/X style license                              !
!!   See License_ROMS.txt                                              !
!!                                                                     !
!=======================================================================
!                                                                      !
!  This subroutine sets analytical adjoint sensitivity spatial scope   !
!  masking arrays.                                                     !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_grid
      USE mod_ncparam
!
! Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model

#include "tile.h"
!
      CALL ana_scope_tile (ng, model, Istr, Iend, Jstr, Jend,           &
     &                     LBi, UBi, LBj, UBj,                          &
#ifdef MASKING
     &                     GRID(ng) % rmask,                            &
     &                     GRID(ng) % umask,                            &
     &                     GRID(ng) % vmask,                            &
#endif
     &                     GRID(ng) % Rscope,                           &
     &                     GRID(ng) % Uscope,                           &
     &                     GRID(ng) % Vscope)
!
! Set analytical header file name used.
!
      IF (Lanafile) THEN
        ANANAME(22)='ROMS/Functionals/ana_scope.h'
      END IF

      RETURN
      END SUBROUTINE ana_scope
!
!***********************************************************************
      SUBROUTINE ana_scope_tile (ng, model, Istr, Iend, Jstr, Jend,     &
     &                           LBi, UBi, LBj, UBj,                    &
#ifdef MASKING
     &                           rmask, umask, vmask,                   &
#endif
     &                           Rscope, Uscope, Vscope)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
!
#if defined EW_PERIODIC || defined NS_PERIODIC
      USE exchange_2d_mod
#endif
#ifdef DISTRIBUTE
      USE mp_exchange_mod, ONLY : mp_exchange2d
#endif
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, model, Iend, Istr, Jend, Jstr
      integer, intent(in) :: LBi, UBi, LBj, UBj
!
#ifdef ASSUMED_SHAPE
# ifdef MASKING
      real(r8), intent(in) :: rmask(LBi:,LBj:)
      real(r8), intent(in) :: umask(LBi:,LBj:)
      real(r8), intent(in) :: vmask(LBi:,LBj:)
# endif
      real(r8), intent(out) :: Rscope(LBi:,LBj:)
      real(r8), intent(out) :: Uscope(LBi:,LBj:)
      real(r8), intent(out) :: Vscope(LBi:,LBj:)
#else
# ifdef MASKING
      real(r8), intent(in) :: rmask(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: umask(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: vmask(LBi:UBi,LBj:UBj)
# endif
      real(r8), intent(out) :: Rscope(LBi:UBi,LBj:UBj)
      real(r8), intent(out) :: Uscope(LBi:UBi,LBj:UBj)
      real(r8), intent(out) :: Vscope(LBi:UBi,LBj:UBj)
#endif
!
!  Local variable declarations.
!
#ifdef DISTRIBUTE
# ifdef EW_PERIODIC
      logical :: EWperiodic=.TRUE.
# else
      logical :: EWperiodic=.FALSE.
# endif
# ifdef NS_PERIODIC
      logical :: NSperiodic=.TRUE.
# else
      logical :: NSperiodic=.FALSE.
# endif
#endif
      integer :: IstrR, IendR, JstrR, JendR, IstrU, JstrV
      integer :: Imin, Imax, Jmin, Jmax, i, j
      real(r8) :: scope(PRIVATE_2D_SCRATCH_ARRAY)

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Set Land/Sea mask of RHO-points: Land=0, Sea=1.
!-----------------------------------------------------------------------
!
!  Notice that private scratch array "mask" is used to allow
!  computation within a parallel loop.
!
#ifdef DOUBLE_GYRE
      Imin=-5+(Lm(ng)+1)/2
      Imax=Imin+10
      Jmin=-5+(Mm(ng)+1)/2
      Jmax=Jmin+10

      DO j=Jstr-2,Jend+2
        DO i=Istr-2,Iend+2
          scope(i,j)=0.0_r8
          IF (((Imin.le.i).and.(i.le.Imax)).and.                        &
     &        ((Jmin.le.j).and.(j.le.Jmax))) THEN
            scope(i,j)=1.0_r8
          END IF
        END DO
      END DO
#else
      ana_scope.h: no values provided for spatial scope masking.
#endif
!
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          Rscope(i,j)=scope(i,j)
#ifdef MASKING
          Rscope(i,j)=Rscope(i,j)*rmask(i,j)
#endif
        END DO
      END DO
!
!-----------------------------------------------------------------------
!  Compute Land/Sea mask of U- and V-points.
!-----------------------------------------------------------------------
!
      DO j=JstrR,JendR
        DO i=Istr,IendR
          Uscope(i,j)=scope(i-1,j)*scope(i,j)
#ifdef MASKING
          Uscope(i,j)=Uscope(i,j)*umask(i,j)
#endif
        END DO
      END DO
      DO j=Jstr,JendR
        DO i=IstrR,IendR
          Vscope(i,j)=scope(i,j-1)*scope(i,j)
#ifdef MASKING
          Vscope(i,j)=Vscope(i,j)*vmask(i,j)
#endif
        END DO
      END DO

#if defined EW_PERIODIC || defined NS_PERIODIC || defined DISTRIBUTE
!
!-----------------------------------------------------------------------
!  Exchange boundary edges.
!-----------------------------------------------------------------------
!
# if defined EW_PERIODIC || defined NS_PERIODIC
      CALL exchange_r2d_tile (ng, Istr, Iend, Jstr, Jend,               &
     &                        LBi, UBi, LBj, UBj,                       &
     &                        Rscope)
      CALL exchange_u2d_tile (ng, Istr, Iend, Jstr, Jend,               &
     &                        LBi, UBi, LBj, UBj,                       &
     &                        Uscope)
      CALL exchange_v2d_tile (ng, Istr, Iend, Jstr, Jend,               &
     &                        LBi, UBi, LBj, UBj,                       &
     &                        Vscope)
# endif
# ifdef DISTRIBUTE
      CALL mp_exchange2d (ng, model, 3, Istr, Iend, Jstr, Jend,         &
     &                    LBi, UBi, LBj, UBj,                           &
     &                    NghostPoints, EWperiodic, NSperiodic,         &
     &                    Rscope, Uscope, Vscope)
# endif
#endif
      RETURN
      END SUBROUTINE ana_scope_tile