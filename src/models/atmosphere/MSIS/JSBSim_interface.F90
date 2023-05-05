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