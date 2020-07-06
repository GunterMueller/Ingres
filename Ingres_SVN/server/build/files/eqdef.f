C
C Copyright (c) 2005 Ingres Corporation
C
C	eqdef.f
C		-- External INGRES integer functions for  F77 FORTRAN
C
C	Because of APOLLO Fortran, tabs are no longer legal.
C
C History:
C	01-Nov-2005 (hanje04)
C	    Added history.
C	    BUGS 113212, 114839 & 115285.
C	    Make IIsd, IIsdno, IIps, IIstr, IInum & IIsadr INTEGER*8 on
C	    64bit Linux.
C
      external IInexe, IInext, IItupg, IIerrt, IIcsRe, IILprs
      external IIsd, IIsdno, IIps
      integer  IInexe, IInext, IItupg, IIerrt, IIcsRe, IILprs
      integer    IIsd, IIsdno, IIps
C
C -- Utility calls for user
      external IIstr, IInum, IIslen, IIsadr
      integer  IIslen
      integer    IIstr, IInum, IIsadr
