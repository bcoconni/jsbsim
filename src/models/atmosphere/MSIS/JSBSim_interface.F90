!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
!
! Interface between JSBSim C++ code and NRLMSIS2.0 FORTRAN code
!
! -------------- Copyright (C) 2023 Bertrand Coconnier --------------
!
! This program is free software; you can redistribute it and/or modify it under
! the terms of the GNU Lesser General Public License as published by the Free
! Software Foundation; either version 2 of the License, or (at your option) any
! later version.
!
! This program is distributed in the hope that it will be useful, but WITHOUT
! ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
! FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
! details.
!
! You should have received a copy of the GNU Lesser General Public License along
! with this program; if not, write to the Free Software Foundation, Inc., 59
! Temple Place - Suite 330, Boston, MA 02111-1307, USA.
!
! Further information about the GNU Lesser General Public License can also be
! found on the world wide web at http://www.gnu.org.
!
!%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

module JSBSim_interface

  implicit none

contains

  subroutine init(parmpath, parmfile) bind (C)

    use, intrinsic :: iso_c_binding, only: c_char, c_null_char
    use msis_init, only: msisinit

    implicit none

    character(kind=c_char), dimension(*), intent(in), optional :: parmpath
    character(kind=c_char), dimension(*), intent(in) :: parmfile
    character(len=128)           :: parmpath1
    character(len=128)           :: parmfile1
    integer                      :: i

    parmpath1= ''
    if (present(parmpath)) then
      do i=1,128
        if (parmpath(i) == c_null_char) exit
        parmpath1(i:i) = parmpath(i)
      end do
    end if

    parmfile1= ''
    do i=1,128
      if (parmfile(i) == c_null_char) exit
      parmfile1(i:i) = parmfile(i)
    end do

    call msisinit(parmpath1, parmfile1)

  end subroutine init

end module JSBSim_interface