-- {
-- File Name: 	EQSQLCAG.ADA
-- Purpose:	ADA package specification file for Embedded SQL global
--		version of the sqlca for non-multi programs only.
--
-- Packages:	ESQLGBL	    - Global copy of sqlca.
-- Usage:
--		EXEC SQL INCLUDE SQLCA;
--    generates:
--		with EQUEL; use EQUEL;
--		with ESQL; use ESQL;
--		with ESQLGBL; use ESQLGBL;		-- Non-multi only
--		with EQUEL_FORMS; use EQUEL_FORMS;
--
-- History:	04-May-2007 (toumi01)
--		    Written.
--
-- Copyright (c) 2007 Ingres Corporation
-- } 

with SYSTEM;
with ESQL;

package	ESQLGBL is

use SYSTEM;
use ESQL;

sqlca: IISQLCA;
 pragma psect_object(sqlca);		-- Global and external

end ESQLGBL;
