#include "cppdefs.h"
      MODULE ocean_control_mod
!
!================================================== Hernan G. Arango ===
!  Copyright (c) 2005 ROMS/TOMS Group              Christopher Moore   !
!=======================================================================
!                                                                      !
!  ROMS/TOMS Nonlinear model:                                          !
!                                                                      !
!  This driver executes ROMS/TOMS standard nonlinear model.  It        !
!  controls the initialization, time-stepping, and finalization        !
!  of the nonlinear model execution following ESMF conventions:        !
!                                                                      !
!     initialize                                                       !
!     run                                                              !
!     finalize                                                         !
!                                                                      !
!=======================================================================
!
      implicit none

      PRIVATE
      PUBLIC  :: initialize, run, finalize

      CONTAINS

      SUBROUTINE initialize (first, MyCOMM)
!
!=======================================================================
!                                                                      !
!  This routine allocates and initializes ROMS/TOMS state variables    !
!  and internal and external parameters.                               !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_parallel
      USE mod_iounits
      USE mod_scalars
!
#ifdef AIR_OCEAN 
      USE atm_coupler_mod, ONLY : initialize_coupling
#endif
!
!  Imported variable declarations.
!
      logical, intent(inout) :: first

      integer, intent(in), optional :: MyCOMM
!
!  Local variable declarations.
!
      logical :: allocate_vars = .TRUE.

      integer :: ng, thread

#ifdef DISTRIBUTE
!
!-----------------------------------------------------------------------
!  Set distribute-memory (MPI) world communictor.
!-----------------------------------------------------------------------
!
      IF (PRESENT(MyCOMM)) THEN
        OCN_COMM_WORLD=MyCOMM
      ELSE
        OCN_COMM_WORLD=MPI_COMM_WORLD
      END IF
#endif
!
!-----------------------------------------------------------------------
!  On first pass, initialize model parameters a variables for all
!  nested/composed grids.  Notice that the logical switch "first"
!  is used to allow multiple calls to this routine during ensemble
!  configurations.
!-----------------------------------------------------------------------
!
      IF (first) THEN
        first=.FALSE.
!
!  Initialize model internal parameters.
!
        CALL initialize_param
        CALL initialize_parallel
        CALL initialize_scalars
!
!  Initialize wall clocks.
!
        IF (Master) THEN
          WRITE (stdout,10)
 10       FORMAT (' Process Information:',/)
        END IF
        DO ng=1,Ngrids
!$OMP PARALLEL DO PRIVATE(thread) SHARED(ng,numthreads)
          DO thread=0,numthreads-1
            CALL wclock_on (ng, iNLM, 0)
          END DO
!$OMP END PARALLEL DO
        END DO

#ifdef AIR_OCEAN 
!
!  Initialize coupling streams between atmosphere and ocean using the
!  Model Coupling Toolkit (MCT).
!
        CALL initialize_coupling (MyRank)
#endif
!
!  Read in model tunable parameters from standard input.
!
        CALL inp_par (iNLM)
        IF (exit_flag.ne.NoError) THEN
          IF (Master) THEN
            WRITE (stdout,'(/,a,i3,/)') Rerror(exit_flag), exit_flag
          END IF
          RETURN
        END IF
!
!  Allocate and initialize modules variables.
!
        CALL mod_arrays (allocate_vars)
      END IF
!
!-----------------------------------------------------------------------
!  Initialize model state variables for all nested/composed grids.
!-----------------------------------------------------------------------
!
      DO ng=1,Ngrids
        CALL initial (ng)
        IF (exit_flag.ne.NoError) THEN
          IF (Master) THEN
            WRITE (stdout,'(/,a,i3,/)') Rerror(exit_flag), exit_flag
          END IF
          RETURN
        END IF
      END DO

      RETURN
      END SUBROUTINE initialize

      SUBROUTINE run
!
!=======================================================================
!                                                                      !
!  This routine time-steps ROMS/TOMS nonlinear model.                  !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_parallel
      USE mod_iounits
      USE mod_scalars
!
!  Local variable declarations.
!
      integer :: ng, my_iic
!
!-----------------------------------------------------------------------
!  Run model for all nested grids, if any.
!-----------------------------------------------------------------------
!
      Nrun=1

      NEST_LOOP : DO ng=1,Ngrids

        IF (Master) THEN
          WRITE (stdout,10) ntstart, ntend
        END IF

        time(ng)=time(ng)-dt(ng)

        NL_LOOP : DO my_iic=ntstart,ntend+1

          iic(ng)=my_iic
#ifdef SOLVE3D
          CALL main3d (ng)
#else
          CALL main2d (ng)
#endif
          IF (exit_flag.ne.NoError) THEN
            IF (Master) THEN
              WRITE (stdout,20) Rerror(exit_flag), exit_flag
            END IF
            RETURN
          END IF

        END DO NL_LOOP

      END DO NEST_LOOP
!
 10   FORMAT (/,'NL ROMS/TOMS: started time-stepping:',                 &
     &            '( TimeSteps: ',i8.8,' - ',i8.8,')',/)
 20   FORMAT (/,a,i3,/)

      RETURN
      END SUBROUTINE run

      SUBROUTINE finalize
!
!=======================================================================
!                                                                      !
!  This routine terminates ROMS/TOMS nonlinear model execution.        !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_parallel
      USE mod_iounits
      USE mod_ncparam
      USE mod_scalars
!
!  Local variable declarations.
!
      integer :: ng, thread
!
!-----------------------------------------------------------------------
!  If blowing-up, save latest model state into RESTART NetCDF file.
!-----------------------------------------------------------------------
!
!  If cycling restart records, write solution into the next record.
!
      DO ng=1,Ngrids
        IF (LwrtRST(ng).and.(exit_flag.eq.1)) THEN
          IF (Master) WRITE (stdout,10)
 10       FORMAT (/,' Blowing-up: Saving latest model state into ',     & 
     &              ' RESTART file',/)
          IF (LcycleRST(ng).and.(NrecRST(ng).ge.2)) THEN
            tRSTindx(ng)=2
            LcycleRST(ng)=.FALSE.
          END IF
          blowup=exit_flag
          exit_flag=NoError
          CALL wrt_rst (ng)
        END IF
      END DO
!
!-----------------------------------------------------------------------
!  Stop model and time profiling clocks.  Close output NetCDF files.
!-----------------------------------------------------------------------
!
!  Stop time clocks.
!
      IF (Master) THEN
        WRITE (stdout,20)
 20     FORMAT (/,' Elapsed CPU time (seconds):',/)
      END IF

      DO ng=1,Ngrids
!$OMP PARALLEL DO PRIVATE(thread) SHARED(ng,numthreads)
        DO thread=0,numthreads-1
          CALL wclock_off (ng, iNLM, 0)
        END DO
!$OMP END PARALLEL DO
      END DO
!
!  Close IO files.
!
      CALL close_io

      RETURN
      END SUBROUTINE finalize

      END MODULE ocean_control_mod
