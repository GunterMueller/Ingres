# define int_lnx
/*
** Ingres bzarch.h created on Sat May 17 22:20:18 EDT 2008
** by the /home/ingres/SANDBOX/Ingres_SVN/server/src/bin/mkbzarch shell script
*/

# define conf_INGRESII
# define conf_libc6
# define conf_SVR_AR
# define conf_DBL
# define conf_B64
# define conf_CAS_ENABLED
# define conf_BUILD_SPATIAL
/* SCALARP, SCALARP_IS_LONG may be used ONLY inside the CL. */
# define SCALARP int
# if !defined(MAINWIN) || defined(conf_W4GL)
# define UNIX
# endif
# define BAD_BITFIELDS
# ifdef __cplusplus
# define GLOBALREF extern "C"
# else  /* __cplusplus */
# define GLOBALREF extern
# endif  /* __cplusplus */
# define GLOBALDEF
# define NODY
# define II_DMF_MERGE
# define INGRES65
# define GCF65
# define LOADDS
# define FILE_UNIX
# define IEEE_FLOAT
# define OS_THREADS_USED
# define POSIX_THREADS
# define NO_INTERNAL_THREADS
# define LNX
# define ZERO_FILL = {0}
# define READONLY	const
# define WSCREADONLY const
# define GLOBALCONSTDEF const
# define GLOBALCONSTREF extern const
/* VOLATILE may ONLY be used inside the CL. */
# define VOLATILE volatile
# define LITTLE_ENDIAN_INT
# define BITSPERBYTE	8
# define	ALIGN_I2	0
# define	ALIGN_I4	0
# define	ALIGN_I8	7
# define	ALIGN_F4	0
# define	ALIGN_F8	3
# define I2ASSIGN_MACRO(a,b) ((*(i2 *)&b) = (*(i2 *)&a))
# define I4ASSIGN_MACRO(a,b) ((*(i4 *)&b) = (*(i4 *)&a))
# define I8ASSIGN_MACRO(a,b) ((*(i8 *)&b) = (*(i8 *)&a))
# define F4ASSIGN_MACRO(a,b) ((*(f4 *)&b) = (*(f4 *)&a))
# define F8ASSIGN_MACRO(a,b) ((*(f8 *)&b) = (*(f8 *)&a))
# define ALIGN_RESTRICT   int
# define I1_CHECK	(i4)
/* 6.1 renamed this... */
# define I1_CHECK_MACRO( x )	I1_CHECK( (x) )
# define CSASCII
# define DOUBLEBYTE
# define INGRESII
# define xEX_HARDMATH
# define xCL_FLOAT_H_EXISTS
# define xCL_ABS_EXISTS
# define xCL_FINITE_EXISTS
# define xCL_GTK_EXISTS
# define xCL_RPM_EXISTS

/* End of bzarch.h */
