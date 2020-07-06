$ inst := 'f$trnlnm("II_INSTALLATION")
$ if inst .eqs. ""
$ then
$   inst_code := <production>
$ else
$   inst_code = inst
$ endif
$ Write sys$output "Building Sharable Image for INGRES User Defined Datatypes for ''inst_code' installation"
$!
$ if f$getsyi("hw_model") .lt. 1024
$ then
$!
$!	VAX version
$!
$ macro/object=ii_system:[ingres.library]ii_useradt_xfer.obj -
	ii_system:[ingres.library]ii_useradt_xfer.mar
$ link/share=ii_system:[ingres.library]iiuseradt'inst'/sysshr -
	sys$input/opt /nodebug	'p1'
!
! This CLUSTER statement forces the transfer vector to the beginning of
! the shared image.  It should not be removed.
!
Cluster = TRANSFER_VECTOR,,,ii_system:[ingres.library]ii_useradt_xfer.obj
!
! Replace the object module below with the object modules
! defining Installation Datatypes
!
ii_system:[ingres.demo.udadts]op.obj,-
ii_system:[ingres.demo.udadts]common.obj,-
ii_system:[ingres.demo.udadts]cpx.obj,-
ii_system:[ingres.demo.udadts]intlist.obj,-
ii_system:[ingres.demo.udadts]zchar.obj,-
ii_system:[ingres.demo.udadts]numeric.obj,-
ii_system:[ingres.demo.udadts]nvarchar.obj,-
ii_system:[ingres.demo.udadts]iicvpk.obj,-
ii_system:[ingres.demo.udadts]iimhpk.obj
!
! End of object modules defining datatypes.
!
psect_attr = _HUGE, NOWRT
NAME = IIUSERADT
!
! Note that the shared image id should not be changed.  INGRES expects this
! level.  The shared image ID can be changed ONLY by the product vendor.
!
IDENTIFICATION = "v2-000"
GSMATCH=LEQUAL, 2, 0
$ exit
$!
$ else
$!
$!	Alpha version
$!
$ link/share/notrace/nodebug/ -
 exe=ii_system:[ingres.library]iiuseradt'inst' SYS$INPUT/OPTION
II_SYSTEM:[ingres.library]iiclsadt.obj, -
ii_system:[ingres.demo.udadts]op.obj,-
ii_system:[ingres.demo.udadts]common.obj,-
ii_system:[ingres.demo.udadts]cpx.obj,-
ii_system:[ingres.demo.udadts]intlist.obj,-
ii_system:[ingres.demo.udadts]zchar.obj,-
ii_system:[ingres.demo.udadts]numeric.obj,-
ii_system:[ingres.demo.udadts]nvarchar.obj,-
ii_system:[ingres.demo.udadts]iicvpk.obj,-
ii_system:[ingres.demo.udadts]iimhpk.obj
psect_attr = _HUGE, NOWRT
NAME = IIUSERADT
IDENTIFICATION = "V2-000"
GSMATCH=LEQUAL, 2, 0
SYMBOL_VECTOR = (iiudadt_register = PROCEDURE)
SYMBOL_VECTOR = (iiclsadt_register = PROCEDURE)
$ exit
$ endif
