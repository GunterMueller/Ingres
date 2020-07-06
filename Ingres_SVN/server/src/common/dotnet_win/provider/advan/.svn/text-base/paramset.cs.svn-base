/*
** Copyright (c) 1999, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using Ca.Ingres.Utility;

namespace Ca.Ingres.ProviderInternals
{
	/*
	** Name: paramset.cs
	**
	** Description:
	**	Defines classes representing a set of query parameters.
	**
	**  Classes:
	**
	**	ParamSet    A parameter data-set.
	**	Param	    An entry in a parameter data-set.
	**
	** History:
	**	15-Nov-00 (gordy)
	**	    Created.
	**	 8-Jan-01 (gordy)
	**	    Extracted additional parameter handling from AdvanPrep.
	**	11-Apr-01 (gordy)
	**	    Do not include time with DATE datatypes.
	**	10-May-01 (gordy)
	**	    Added support for UCS2 datatypes.
	**	 1-Jun-01 (gordy)
	**	    requires long strings to be converted to BLOBs, but
	**	    not the other way around.  Removed conversion of short
	**	    BLOBs to fixed length strings, which removed the need to
	**	    track stream lengths.  No longer need the wrapper classes
	**	    and interfaces associated with stream lengths, greatly
	**	    simplifying this implementation.
	**	13-Jun-01 (gordy)
	**	    No longer need setUnicode() and UniRdr class.  The 2.1
	**	    spec requires UTF-8 encoding for unicode streams, which can
	**	    be read using the standard class InputStreamReader.
	**	30-Aug-02 (thoda04)
	**	    Ported for .NET Provider.
	**	31-Oct-02 (gordy)
	**	    Adapted for generic GCF driver.
	**	19-Feb-03 (gordy)
	**	    Updated to 3.0.
	**	15-Apr-03 (gordy)
	**	    Date formatters require synchronization.
	**	26-Jun-03 (gordy)
	**	    All CHAR/VARCHAR coercions need alt storage handling.
	**	11-Jul-03 (wansh01)
	**	    all BINARY/CHAR coercions using getBytes() instead of str2bin().
	**	21-Jul-03 (gordy)
	**	    Permit conversion from empty string to Date/Time/Timestamp.
	**	12-Sep-03 (gordy)
	**	    Date formatter replaced with SqlDates utility.
	**	15-Mar-04 (gordy)
	**	    Added support for BIGINT values sent to DBMS.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	**	14-dec-04 (thoda04) SIR 113624
	**	    Add BLANKDATE=NULL support.
	*/




	/*
	** Name: ParamSet
	**
	** Description:
	**	Class which manages a parameter data set.
	**
	**	Parameter values are stored as specific class instances
	**	depending on the associated SQL type.  The supported SQL
	**	types and their associated storage class are as follows:
	**	
	**	SQL Type	Storage Class
	**	-------------	--------------------
	**	NULL		null
	**	BOOLEAN		ProviderType.Boolean
	**	TINYINT		ProviderType.Byte
	**	SMALLINT	ProviderType.Short
	**	INTEGER		ProviderType.Integer
	**	BIGINT		ProviderType.Long
	**	REAL		ProviderType.Float
	**	DOUBLE		ProviderType.Double
	**	DECIMAL		ProviderType.Decimal
	**	CHAR		char[]
	**	VARCHAR		ProviderType.String 
	**	LONGVARCHAR	Reader
	**	BINARY		byte[]
	**	VARBINARY	byte[]
	**	LONGVARBINARY	InputStream
	**	DATE		ProviderType.String 
	**	TIME		ProviderType.String
	**	TIMESTAMP	ProviderType.String
	**
	**	Note: the messaging protocol currently does not provide
	**	support for BOOLEAN, so the alternate storage format is
	**	always used.  The preferred alternate storage format for
	**	BIGINT is DECIMAL, but DECIMAL has it's own alternate
	**	storage format.  To make this managable, BIGINT has the
	**	same alternate format as DECIMAL, uses the same logic to
	**	select the aternate format, and is forced to the DECIMAL
	**	type when BIGINT is not supported.  This allows BIGINT
	**	to be sent to the DBMS as BIGINT, DECIMAL or DOUBLE as
	**	needed.
	**
	**	The following SQL types are supported on input as aliases
	**	for the indicated types:
	**
	**	Alias Type   	SQL Type
	**	-------------	-------------
	**	BIT          	BOOLEAN
	**	FLOAT        	DOUBLE
	**	NUMERIC      	DECIMAL
	**
	**	In addition to the primary storage class given above, some
	**	SQL types support an alternate storage class as listed below:
	**
	**	SQL Type     	Storage Class
	**	-------------	--------------------
	**	CHAR         	byte[]
	**	VARCHAR      	byte[] 
	**
	**	In all other cases, the alternate storage class is the same as
	**	the primary storage class.  Use of the alternate storage class 
	**	is indicated by presence of the ALT flag for the parameter.
	**	When the parameter value is available in the alternate storage
	**	class, the set() method can be used to assign the type, value
	**	and ALT flag with the least overhead.  Coercion to the alternate 
	**	storage class can be done by passing the primary storage class 
	**	to setObject() and passing TRUE for the alt parameter.  
	**
	**	Note that for the character types, coercion to the alternate 
	**	storage format must be done using the CharSet negotiated for 
	**	the server connection.
	**
	**	The setObject() method also supports the coercion of parameter 
	**	values to other SQL types as required by the driver spec.
	**	The following table lists the ProviderType types supported as input
	**	values and their mapping to SQL types:
	**
	**	ProviderType Type		SQL Type
	**	------------		--------
	**	null			NULL
	**	ProviderType.Boolean	BOOLEAN
	**	ProviderType.Byte		TINYINT
	**	ProviderType.Short		SMALLINT
	**	ProviderType.Integer	INTEGER
	**	ProviderType.Long		BIGINT
	**	ProviderType.Float		REAL
	**	ProviderType.Double	DOUBLE
	**	ProviderType.BigDecimal	DECIMAL
	**	char[]			CHAR
	**	ProviderType.String	VARCHAR
	**	Reader			LONGVARCHAR
	**	byte[]			VARBINARY
	**	InputStream		LONGVARBINARY
	**	ProviderType.Date		DATE
	**	ProviderType.Time		TIME
	**	ProviderType.Timestamp	TIMESTAMP
	**
	**	In addition to coercion to SQL types other than the associated 
	**	type shown above, coercion is also supported for instances where 
	**	the ProviderType type listed above differs from the storage class of the 
	**	associated SQL type.
	**
	**  Constants:
	**
	**	PS_FLG_ALT	    Alternate Format
	**	PS_FLG_PROC_PARAM   Procedure parameter.
	**	PS_FLG_PROC_BYREF   BYREF parameter.
	**	PS_FLG_PROC_GTT	    Global Temp Table parameter.
	**
	**  Public Methods:
	**
	**	getSqlType	    Determine parameter type from value.
	**	setDefaultFlags	    Set default parameter flags.
	**	clone		    Make a copy of this parameter set.
	**	clear		    Clear all parameter settings.
	**	setObject	    Set parameter from object.
	**	set		    Set parameter type and value.
	**	setFlags	    Set parameter flags.
	**	setName		    Set parameter name.
	**	getCount	    Returns number of parameters.
	**	isSet		    Has a parameter been set.
	**	getType		    Returns parameter type.
	**	getValue	    Returns parameter value.
	**	getFlags	    Returns parameter flags.
	**	getName		    Returns parameter name.
	**
	**  Private Data:
	**
	**	PS_FLG_SET	    Parameter is set.
	**	EXPAND_SIZE	    Size to grow the parameter data array.
	**	paramArray		    Parameter array.
	**	param_cnt	    Number of actual parameters.
	**	param_flags	    Default parameter flags.
	**	char_set	    Character-set.
	**	use_gmt		    Date/Time values use GMT timezone.
	**
	**  Private Methods:
	**
	**	check		    Validate parameter index.
	**	coerce		    Validate/convert parameter value.
	**	checkSqlType	    Validate/convert parameter type.
	**	str2bin		    Convert hex string to byte array.
	**
	** History:
	**	15-Nov-00 (gordy)
	**	    Created.
	**	 8-Jan-01 (gordy)
	**	    Extracted additional parameter handling from AdvanPrep.
	**	    Added default parameter flags, param_flags, and max
	**	    varchar string length, max_vchr_len.
	**	10-May-01 (gordy)
	**	    Added constants for parameter flags.  Removed setNull()
	**	    method as can be done through setObject().  Added str2bin
	**	    for String to BINARY conversion.
	**	 1-Jun-01 (gordy)
	**	    Removed setBinary(), setAscii() and setCharacter() since
	**	    wrapper classes are no longer needed to track stream lengths.
	**	13-Jun-01 (gordy)
	**	    No longer need setUnicode() and UniRdr class.  The 2.1
	**	    spec requires UTF-8 encoding for unicode streams, which can
	**	    be read using the class InputStreamReader.
	**      13-Aug-01 (loera01) SIR 105521
	**          Added PS_FLG_PROC_GTT constant to support global temp table
	**          parameters.
	**	31-Oct-02 (gordy)
	**	    Removed unused max_vchr_len and setMaxLength().
	**	25-Feb-03 (gordy)
	**	    Added internal PS_FLG_SET to indicate set parameters rather
	**	    than relying on simply object existence which may be a hold
	**	    over from a prior parameter set (cleared but not purged).
	**	    Changed flags to short for additional bits.  Added char_set 
	**	    to support string conversions.
	**	21-Jul-03 (gordy)
	**	    Added empty string constant for Ingres empty dates.
	**	12-Sep-03 (gordy)
	**	    Date formatter replaced with GMT indicator.
	**	15-Mar-04 (gordy)
	**	    Made checkSqlType non-static as it needs to check protocol level
	**	    to determine BIGINT support (force to DECIMAL).
	*/

	class
		ParamSet : DrvObj
	{

		/*
		** Constants for parameter flags.
		*/
		public const ushort	PS_FLG_ALT	    = 0x01;
		public const ushort	PS_FLG_PROC_PARAM   = 0x02;
		public const ushort	PS_FLG_PROC_BYREF   = 0x04;
		public const ushort	PS_FLG_PROC_GTT     = 0x08;

		private const ushort	PS_FLG_SET	    = 0x80;

		/*
		** While a generic Vector class is a possible
		** implementation for this class, a parameter
		** data set is best represented by a sparse
		** array which is not well suited to the Vector
		** class.
		*/
		private const int   	EXPAND_SIZE = 10;
		private int			param_cnt = 0;
		private Param[]		paramArray;

		private CharSet		char_set;
		private bool		use_gmt;
		private ushort		param_flags = 0;	// Default flags.

		/*
		** Name: getSqlType
		**
		** Description:
		**	Returns the SQL type associated with a object as defined 
		**	by this class for supported input parameter values.
		**
		** Input:
		**	obj	Parameter value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	SQL type.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	10-May-01 (gordy)
		**	    Allow sub-classing by using instanceof instead of class ID.
		**	    Support IO classes for BLOBs.
		**	19-Feb-03 (gordy)
		**	    Replaced BIT with BOOLEAN.  Give priority to strings since
		**	    alternate storage class results in calls to this routine.
		*/

		public static ProviderType
			getSqlType( Object obj )
		{
			if ( obj == null )       	return( ProviderType.DBNull );
			if ( obj is String )     	return( ProviderType.VarChar );
			if ( obj is Boolean )    	return( ProviderType.Boolean );
			if ( obj is Char )       	return( ProviderType.Char );
			if ( obj is Byte )       	return( ProviderType.Binary );
			if ( obj is SByte )      	return( ProviderType.TinyInt );
			if ( obj is Int16 )      	return( ProviderType.SmallInt );
			if ( obj is Int32 )      	return( ProviderType.Integer );
			if ( obj is Int64 )      	return( ProviderType.BigInt );
			if ( obj is Single)      	return( ProviderType.Real );
			if ( obj is Double )     	return( ProviderType.Double );
			if ( obj is Decimal )    	return( ProviderType.Decimal );
//			if ( obj is Reader )     	return( ProviderType.LongVarChar );
//			if ( obj is InputStream )	return( ProviderType.LongVarBinary );
//			if ( obj is Date )       	return( ProviderType.DATE );
//			if ( obj is Time )       	return( ProviderType.TIME );
			if ( obj is DateTime )   	return( ProviderType.DateTime );

			System.Type oc = obj.GetType();
			if (oc.IsArray)
			{
				if (oc.GetElementType() == System.Type.GetType("System.Char"))
					return ProviderType.Char;
				
				if (oc.GetElementType() == System.Type.GetType("System.Byte"))
					return ProviderType.VarBinary;
			}

			throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );
		} // getSqlType


		/*
		** Name: ParamSet
		**
		** Description:
		**	Class constructor.  Creates a parameter set of the
		**	default size.  A date formatter is required for
		**	converting date/time objects into internal format
		**	and a CharSet for string conversions.
		**
		** Input:
		**	use_gmt		True to use GMT with date/time values.
		**	char_set	Connection CharSet
		**
		** Output:
		**	None.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		**	31-Oct-02 (gordy)
		**	    Use negative value for default size.
		**	25-Feb-03 (gordy)
		**	    Added char_set parameter.
		**	12-Sep-03 (gordy)
		**	    Date formatter replaced with GMT indicator.
		*/

		public
			ParamSet( DrvConn conn, bool use_gmt, CharSet char_set ) :
				this(conn, -1, use_gmt, char_set )
		{
		} // ParamSet


		/*
		** Name: ParamSet
		**
		** Description:
		**	Class constructor.  Creates a parameter set of the
		**	requested size.  If the provided size is negative,
		**	a parameter set of the default size is created.  A 
		**	date formatter is required for converting date/time 
		**	objects into internal format and a CharSet for string
		**	conversions.
		**
		** Input:
		**	size		Initial size.
		**	use_gmt		True to use GMT with date/time values.
		**	char_set	Connection CharSet.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	None.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		**	31-Oct-02 (gordy)
		**	    Allow zero as valid initial size.
		**	25-Feb-03 (gordy)
		**	    Added char_set parameter.
		**	12-Sep-03 (gordy)
		**	    Date formatter replaced with GMT indicator.
		*/

		public
			ParamSet(DrvConn conn, int size, bool use_gmt, CharSet char_set ) :
				base(conn)
		{
			paramArray = new Param[ (size < 0) ? EXPAND_SIZE : size ];
			this.char_set = char_set;
			this.use_gmt = use_gmt;
			return;
		} // ParamSet


		/*
		** Name: setDefaultFlags
		**
		** Description:
		**	Set the default parameter flags which are assigned
		**	when a parameter is assigned using the set() method.
		**
		** Input:
		**	flags	    Descriptor flags.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	15-Jun-00 (gordy)
		**	    Created.
		**	 8-Jan-01 (gordy)
		**	    Set global flags rather than specific parameter flags.
		*/

		public void
			setDefaultFlags( ushort flags )
		{
			param_flags = (ushort)flags;
			return;
		} // setDefaultFlags


		/*
		** Name: clone
		**
		** Description:
		**	Create a copy of this parameter set.  This class is not
		**	Cloneable at the field level since changes in individual
		**	parameters should remain local.  The actual set of params
		**	is individually cloned to localize changes.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	ParamSet	Copy of this parameter set.
		**
		** History:
		**	11-Jan-01 (gordy)
		**	    Created.
		**	31-Oct-02 (gordy)
		**	    Add synchronization to ensure no change of object being cloned.
		**	25-Feb-03 (gordy)
		**	    Don't clone unset parameter entries.  Pass on CharSet.
		**	12-Sep-03 (gordy)
		**	    Date formatter replaced with GMT indicator.
		*/

		public Object
		clone()
		{
			lock(this)
			{
				ParamSet	ps = new ParamSet(conn, param_cnt, use_gmt, char_set );

				ps.param_cnt = param_cnt;
				ps.param_flags = param_flags;

				for( int i = 0; i < param_cnt; i++ )
					if ( paramArray[ i ] != null  &&  (paramArray[ i ].flags & PS_FLG_SET) != 0 )  
						ps.paramArray[ i ] = (Param)paramArray[ i ].clone();

				return( ps );
			}
		} // clone


		/*
		** Name: clear
		**
		** Description:
		**	Clear stored parameter values and optionally free resources
		**
		** Input:
		**	purge	Purge allocated data?
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		**	31-Oct-02 (gordy)
		**	    Add synchronization.
		**	25-Feb-03 (gordy)
		**	    Clear params to reset flags.  Purge all parameters.
		*/

		public void
		clear( bool purge )
		{
			lock(this)
			{
				/*
				** When purging, scan all parameters to get hold
				** overs from previous sets.  Otherwise, just scan
				** the current parameter set.
				*/
				if ( purge )  param_cnt = paramArray.Length;

				for( int i = 0; i < param_cnt; i++ )  
					if ( paramArray[ i ] != null )
					{
						paramArray[ i ].clear();
						if ( purge )  paramArray[ i ] = null;
					}

				param_cnt = 0;
				return;
			}
		} // clear


		/*
		** Name: setObject
		**
		** Description:
		**	Set parameter value and type by adjusting input as needed to
		**	conform to the standard types and storage classes supported
		**	by this class.  The default flags are added to any previously 
		**	set flags.  Parameter is marked as set.
		**	
		**	The input object must be an instance of one of the ProviderType types
		**	supported as an input parameter value by this class.  The SQL 
		**	type may be any of the supported parameter types or aliases.  
		**	If necessary, the input value will be coerced to match the 
		**	storage class of the requested type.  
		**
		**	The scale of DECIMAL parameters may be specified.  A scale of
		**	-1 indicates that the scale of the input value should not be
		**	changed.
		**
		** Input:
		**	index		Parameter index (0 based).
		**	value		Parameter value.
		**	sqlType		Target SQL type.
		**	scale		DECIMAL scale, or -1.
		**	alt		Use alternate storage format?
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	 8-Jan-01 (gordy)
		**	    Created.
		**	25-Feb-03 (gordy)
		**	    Added alt parameter to select alternate storage class.
		*/

		public void
			setObject(
			int index,
			Object value,
			ProviderType sqlType,
			int scale,
			bool alt)
		{
			setObject( index, value, sqlType, scale, alt, 0 );
		}


		public void
			setObject(
				int index,
				Object value,
				ProviderType sqlType,
				int scale,
				bool alt,
				int size)
		{
			/*
			** Validate requested type and standardize (resolve aliases).
			*/
			sqlType = checkSqlType( sqlType );

			/*
			** Convert to desired type and/or scale.
			*/
			try { value = coerce( value, sqlType, scale, alt, size ); }
			catch( Exception ex )
			{ 
				throw SqlEx.get( ERR_GC401A_CONVERSION_ERR, ex ); 
			}

			set( index, sqlType, value, alt );
			return;
		} // setObject


		/*
		** Name: set
		**
		** Description:
		**	Set parameter value and type without making any adjustments.
		**	The SQL type must be one of the supported parameter types
		**	(not an alias) and the parameter value must be an instance
		**	of the primary storage class associated with the parameter 
		**	type.  The default flags are added to any previously set 
		**	flags.  Parameter is marked as set.
		**
		** Input:
		**	index	    Parameter index (0 based).
		**	type	    SQL type.
		**	value	    Parameter value.
		**	alt	    Alternate storage class?
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	31-Oct-02 (gordy)
		**	    Add synchronization to ensure consistent update.
		**	25-Feb-03 (gordy)
		**	    Added alt parameter to indicate alternate storage class.
		**	    Mark parameter as set.
		*/

		public void
			set( int index, ProviderType type, Object value, bool alt )
		{
			check( index );
			lock( paramArray[ index ] )
			{
				paramArray[ index ].type = type;
				paramArray[ index ].value = value;
				paramArray[ index ].flags |= (ushort)(PS_FLG_SET | param_flags);
				if ( alt )
					paramArray[ index ].flags |= PS_FLG_ALT;
			}
			return;
		} // set


		/*
		** Name: setFlags
		**
		** Description:
		**      Add flags to a parameter.  Does not clear existing flags.
		**	Does not mark parameter as set.
		**
		** Input:
		**      index       Parameter index (0 based).
		**      flags       Descriptor flags.
		**
		** Output:
		**      None.
		**
		** Returns:
		**      void.
		**
		** History:
		**      15-Jun-00 (gordy)
		**          Created.
		**	31-Oct-02 (gordy)
		**	    Add synchronization to ensure consistent update.
		**	25-Feb-03 (gordy)
		**	    Changed flags to short for additional bits.
		*/

		public void
			setFlags( int index, ushort flags )
		{
			check( index );
			lock( paramArray[ index ] )  { paramArray[ index ].flags |= flags; }
			return;
		} // setFlags


		/*
		** Name: setName
		**
		** Description:
		**	Set the name of a parameter.  Does not mark parameter as set.
		**
		** Input:
		**	index	    Parameter index (0 based).
		**	name	    Parameter name.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	15-Jun-00 (gordy)
		**	    Created.
		*/

		public void
			setName( int index, String name )
		{
			check( index );
			paramArray[ index ].name = name;
			return;
		} // setName


		/*
		** Name: getCount
		**
		** Description:
		**	Returns the number parameters in the data set
		**	(includes unset intermediate parameters).
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Number of parameters.
		**
		** History:
		**	15-Nov-00 (gordy)
		*/

		public int
			getCount()
		{
			return( param_cnt );
		} // getCount


		/*
		** Name: isSet
		**
		** Description:
		**	Has the requested parameter been set?
		**
		** Input:
		**	index	Parameter index (0 based).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean	TRUE if parameter set, FALSE otherwise.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		**	25-Feb-03 (gordy)
		**	    Check set flag.
		*/

		public bool
			isSet( int index )
		{
			return( index < param_cnt  &&  paramArray[ index ] != null  &&  
				(paramArray[ index ].flags & PS_FLG_SET) != 0 );
		} // isSet


		/*
		** Name: getType
		**
		** Description:
		**	Retrieve the type of a parameter.
		**
		** Input:
		**	index	Parameter index (0 based).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Parameter type.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		*/

		public ProviderType
			getType( int index )
		{
			check( index );
			return( paramArray[ index ].type );
		} // getType


		/*
		** Name: getValue
		**
		** Description:
		**	Retrieve the type of a parameter.
		**
		** Input:
		**	index	Parameter index (0 based).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Object	Parameter value.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		*/

		public Object
			getValue( int index )
		{
			check( index );
			return( paramArray[ index ].value );
		} // getValue


		/*
		** Name: getFlags
		**
		** Description:
		**	Retrieve the flags of a parameter.
		**
		** Input:
		**	index	Parameter index (0 based).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	short	Parameter flags.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		**	25-Feb-03 (gordy)
		**	    Changed flags to short for additional bits.
		*/

		public ushort
			getFlags( int index )
		{
			check( index );
			return( paramArray[ index ].flags );
		} // getFlags


		/*
		** Name: getName
		**
		** Description:
		**	Retrieve the name of a parameter.
		**
		** Input:
		**	index	Parameter index (0 based).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String	Parameter name.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		*/

		public String
			getName( int index )
		{
			check( index );
			return( paramArray[ index ].name );
		} // getName


		/*
		** Name: check
		**
		** Description:
		**	Check the parameter index and expand the parameter
		**	array if necessary.  The referenced parameter is
		**	(optionally) allocated.
		**
		** Input:
		**	index	    Parameter index (0 based).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	    
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		*/

		private void
		check( int index )
		{
			lock(this)
			{
				int	    new_max;

				if ( index < 0 )  throw new ArgumentOutOfRangeException();

				/*
									 ** If the new index is beyond the current limit,
									 ** expand the parameter array to include the new
									 ** index.
									 */
				for( new_max = paramArray.Length; index >= new_max; new_max += EXPAND_SIZE );

				if ( new_max > paramArray.Length )
				{
					Param[] new_array = new Param[ new_max ];
					Array.Copy( paramArray, 0, new_array, 0, paramArray.Length );
					paramArray = new_array;
				}

				/*
				** Any pre-allocated parameters between the previous
				** upper limit and the new limit are cleared and the
				** parameter count updated.  Allocate the parameter
				** for the current index if needed.
				*/
				for( ; param_cnt <= index; param_cnt++ )
					if ( paramArray[ param_cnt ] != null )  paramArray[ param_cnt ].clear();

				if ( paramArray[ index ] == null )  paramArray[ index ] = new Param();

				return;
			}
		} // check


		/*
		** Name: coerce
		**
		** Description:
		**	Coerce an input parameter value to the associated primary
		**	or alternate storage class of the target SQL type.  The 
		**	input object must be an instance of a class which is 
		**	supported as a parameter input value.  The target SQL 
		**	type must be a supported type (not an alias).
		**
		**	When the target SQL type is DECIMAL, the resulting scale
		**	may be forced to a desired value or left unchanged by
		**	providing a negative value.
		**
		**	The input value is returned if coercion is not required.
		**
		** Input:
		**	obj	    Parameter value.
		**	sqlType	    Target SQL type.
		**	scale	    Scale for decimal values or -1.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Object	    The coerced value.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	 1-Nov-00 (gordy)
		**	    Date types are now saved as a string.  Throw Exception
		**	    if coercion from objType to sqlType not supported.
		**	11-Apr-01 (gordy)
		**	    Ingres supports (to some extent) dates without time,
		**	    so do not include time with DATE datatypes.
		**	10-May-01 (gordy)
		**	    Removed objType parameter to enforce supported object classes.
		**	    IO streams now supported but require wrapper class, so added
		**	    to classes need identity coercions.  Char arrays now fully
		**	    supported.  Cleaned up permitted coercions compared to spec.
		**	    Character to binary coercions should use hex conversion
		**	    (? same as is done in AdvanRslt).
		**	 1-Jun-01 (gordy)
		**	    Reader/InputStream no longer need wrapper classes.
		**	19-Feb-03 (gordy)
		**	    Added alt parameter for alternate storage class.
		**	    Coerce char arrays and strings to byte arrays as
		**	    alternate storage class.  Replaced BIT with BOOLEAN.
		**	15-Apr-03 (gordy)
		**	    Date formatters require synchronization.
		**	26-Jun-03 (gordy)
		**	    All CHAR/VARCHAR coercions need alt storage handling.
		**	11-Jul-03 (wansh01)
		**	    use string.getBytes() instead of str2bin() to coerce CHAR/VARCHAR 
		** 	    to BINARY/VARBINARY.  
		**	21-Jul-03 (gordy)
		**	    Allow empty strings for Date/Time values.
		**	12-Sep-03 (gordy)
		**	    Date formatter replaced with SqlDates utility.
		*/

		private System.Object coerce(
				System.Object obj,
				ProviderType sqlType,
				int scale,
				bool alt, // true if need to convert to char and not use UCS2 storage
				int maxsize)
		{
			ProviderType objType = getSqlType(obj); // Determine input type.

			bool haveValueToReturn = true;
			System.DateTime ts   = System.DateTime.MaxValue;
			System.Decimal  dec  = System.Decimal.Zero;
			System.String   str;
			char[]          charArray;
			byte[]          byteArray;
			
			/*
			** A null reference is returned for a null input
			** or a request to coerce to NULL.
			*/
			if (objType == ProviderType.DBNull ||
				sqlType == ProviderType.DBNull)
				return (null);
			
			/*
			** If input parameter is a char type
			**   then normalize into a char array type.
			*/
			if (obj is System.Char)
			{
				char[] char1Array = new Char[1];
				char1Array[0] = (char)obj;
				obj = char1Array;
			}

			/*
			** Treat NChar, NVarChar, and LongNVarChar target types the same as
			** their  Char,  VarChar, and LongVarChar counterparts.
			*/
			if (sqlType == ProviderType.NChar)
				sqlType =  ProviderType.Char;
			else
				if (sqlType == ProviderType.NVarChar)
				sqlType =  ProviderType.VarChar;
			else
				if (sqlType == ProviderType.LongNVarChar)
				sqlType =  ProviderType.LongVarChar;
			
			
			/*
			** Handle specific coercions.
			*/
			switch (sqlType)    // target data type
			{
				case ProviderType.Boolean:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						return (obj);
						
					case ProviderType.TinyInt: 
						return ((byte) ((System.SByte) obj) != 0);
						
					case ProviderType.SmallInt: 
						return ((short) ((System.Int16) obj) != 0);
						
					case ProviderType.Integer: 
						return (((System.Int32) obj) != 0);
						
					case ProviderType.BigInt: 
						return ((long) ((System.Int64) obj) != 0L);
						
					case ProviderType.Single: 
						return ((float) ((System.Single) obj) != 0.0);
						
					case ProviderType.Double: 
						return (((System.Double) obj) != 0.0);
						
					case ProviderType.Decimal: 
						return (System.Math.Sign(((System.Decimal) obj)) != 0);
						
					case ProviderType.Char: 
						return (
							ToInvariantUpper(
								new String((char[]) obj)).Equals("TRUE"));
						
					case ProviderType.VarChar: // String
						return (
							ToInvariantUpper(
								((System.String) obj)).Equals("TRUE"));
						
				}
					break;
					
				
				
				case ProviderType.TinyInt:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						return ((sbyte) (((System.Boolean) obj)?1:0));
						
					case ProviderType.TinyInt: 
						return (obj);
						
					case ProviderType.Binary: 
						return ((sbyte) ((System.Byte) obj));
						
					case ProviderType.SmallInt: 
						return ((sbyte) ((System.Int16) obj));
						
					case ProviderType.Integer: 
						return ((sbyte) ((System.Int32) obj));
						
					case ProviderType.BigInt: 
						return ((sbyte) ((System.Int64) obj));
						
					case ProviderType.Single: 
						return ((sbyte) ((System.Single) obj));
						
					case ProviderType.Double: 
						return ((sbyte) ((System.Double) obj));
						
					case ProviderType.Decimal: 
						return (System.Decimal.ToSByte((Decimal)obj));
						
					case ProviderType.Char: 
						return (System.Byte.Parse(new String((char[]) obj)));
						
					case ProviderType.VarChar: // String
						return (System.Byte.Parse((System.String) obj));
						
				}
					break;
					
				
				
				case ProviderType.SmallInt:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						return ((short) (((System.Boolean) obj)?1:0));
						
					case ProviderType.Binary: 
						return ((short) ((System.Byte) obj));
						
					case ProviderType.TinyInt: 
						return ((short) ((System.SByte) obj));
						
					case ProviderType.SmallInt: 
						return (obj);
						
					case ProviderType.Integer: 
						return ((short) ((System.Int32) obj));
						
					case ProviderType.BigInt: 
						return ((short) ((System.Int64) obj));
						
					case ProviderType.Single: 
						return ((short) ((System.Single) obj));
						
					case ProviderType.Double: 
						return ((short) ((System.Double) obj));
						
					case ProviderType.Decimal: 
						return (System.Decimal.ToInt16((Decimal)obj));
						
					case ProviderType.Char: 
						return (System.Int16.Parse(new String((char[]) obj)));
						
					case ProviderType.VarChar: // String
						return (System.Int16.Parse((System.String) obj));
						
				}
					break;
					
				
				
				case ProviderType.Integer:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						return ((int) (((System.Boolean) obj)?1:0));
						
					case ProviderType.Binary: 
						return ((int) ((System.Byte) obj));
						
					case ProviderType.TinyInt: 
						return ((int) ((System.SByte) obj));
						
					case ProviderType.SmallInt: 
						return ((int) ((System.Int16) obj));
						
					case ProviderType.Integer: 
						return (obj);
						
					case ProviderType.BigInt: 
						return ((int) ((System.Int64) obj));
						
					case ProviderType.Single: 
						return ((int) ((System.Single) obj));
						
					case ProviderType.Double: 
						return ((int) ((System.Double) obj));
						
					case ProviderType.Decimal: 
						return (System.Decimal.ToInt32(((System.Decimal) obj)));
						
					case ProviderType.Char: 
						return (System.Int32.Parse(new String((char[]) obj)));
						
					case ProviderType.VarChar: // String
						return (System.Int32.Parse((System.String) obj));
						
				}
					break;
					
				
				
				case ProviderType.BigInt:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						return ((long) (((System.Boolean) obj)?1:0));
						
					case ProviderType.Binary: 
						return ((long) ((System.Byte) obj));
						
					case ProviderType.TinyInt: 
						return ((long) ((System.SByte) obj));
						
					case ProviderType.SmallInt: 
						return ((long) ((System.Int16) obj));
						
					case ProviderType.Integer: 
						return ((long) ((System.Int32) obj));
						
					case ProviderType.BigInt: 
						return (obj);
						
					case ProviderType.Single: 
						return ((long) ((System.Single) obj));
						
					case ProviderType.Double: 
						return ((long) ((System.Double) obj));
						
					case ProviderType.Decimal: 
						return (System.Decimal.ToInt64(((System.Decimal) obj)));
						
					case ProviderType.Char: 
						return (System.Int64.Parse(new String((char[]) obj)));
						
					case ProviderType.VarChar: // String
						return (System.Int64.Parse((System.String) obj));
						
				}
					break;
					
				
				
				case ProviderType.Single:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						return ((float) (((System.Boolean) obj)?1:0));
						
					case ProviderType.Binary: 
						return ((float) ((System.Byte) obj));
						
					case ProviderType.TinyInt: 
						return ((float) ((System.SByte) obj));
						
					case ProviderType.SmallInt: 
						return ((float) ((System.Int16) obj));
						
					case ProviderType.Integer: 
						return ((float) ((System.Int32) obj));
						
					case ProviderType.BigInt: 
						return ((float) ((System.Int64) obj));
						
					case ProviderType.Single: 
						return (obj);
						
					case ProviderType.Double: 
						return ((float) ((System.Double) obj));
						
					case ProviderType.Decimal: 
						return (System.Decimal.ToSingle(((System.Decimal) obj)));
						
					case ProviderType.Char: 
						return (System.Single.Parse(new String((char[]) obj)));
						
					case ProviderType.VarChar: // String
						return (System.Single.Parse((System.String) obj));
						
				}
					break;
					
				
				
				case ProviderType.Double:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						return ((double) (((System.Boolean) obj)?1:0));
						
					case ProviderType.Binary: 
						return ((double) ((System.Byte) obj));
						
					case ProviderType.TinyInt: 
						return ((double) ((System.SByte) obj));
						
					case ProviderType.SmallInt: 
						return ((double) ((System.Int16) obj));
						
					case ProviderType.Integer: 
						return ((double) ((System.Int32) obj));
						
					case ProviderType.BigInt: 
						return ((double) ((System.Int64) obj));
						
					case ProviderType.Single: 
						return ((double) ((System.Single) obj));
						
					case ProviderType.Double: 
						return (obj);
						
					case ProviderType.Decimal: 
						return (System.Decimal.ToDouble(((System.Decimal) obj)));

					case ProviderType.Char: 
						return (System.Double.Parse(new String((char[]) obj)));

					case ProviderType.VarChar: // String
						return (System.Double.Parse((System.String) obj));

				}
					break;



				case ProviderType.DateTime:    // target
					haveValueToReturn = true;
					
				switch (objType)
				{
					case ProviderType.Char: 
						if ( ((char[])obj).Length == 0 ) return String.Empty;
						ts = System.DateTime.Parse(new String((char[]) obj));
						break;
						
					case ProviderType.VarChar: // String
						if ( ((String)obj).Length == 0 ) return String.Empty;
						ts = System.DateTime.Parse((System.String) obj);
						break;
						
					case ProviderType.DateTime: // Convert to string
						ts = (System.DateTime) obj;
						break;
						
					default:
						haveValueToReturn = false;
						break;
				}

					if (haveValueToReturn)
					{
						if (this.conn.is_ingres  &&  // if "9999-12-31 23:59:59"
							ts == DT_EPOCH)
							return String.Empty;     //     return blank date
						return( SqlDates.formatTimestamp( ts, use_gmt ) );
					}
					break;



				case ProviderType.Decimal:    // target
				switch (objType)
				{
					case ProviderType.Boolean: 
						dec = new System.Decimal((long) (((System.Boolean) obj)?1:0));
						return (dec);
						
					case ProviderType.Binary: 
						dec = new System.Decimal((long) ((System.Byte) obj));
						return (dec);
						
					case ProviderType.TinyInt: 
						dec = new System.Decimal((long) ((System.SByte) obj));
						return (dec);
						
					case ProviderType.SmallInt: 
						dec = new System.Decimal((long) ((System.Int16) obj));
						return (dec);
						
					case ProviderType.Integer: 
						dec = new System.Decimal((long) ((System.Int32) obj));
						return (dec);
						
					case ProviderType.BigInt: 
						dec = new System.Decimal((long) ((System.Int64) obj));
						return (dec);
						
					case ProviderType.Single: 
						dec = new System.Decimal((double) ((System.Single) obj));
						return (dec);
						
					case ProviderType.Double: 
						dec = new System.Decimal(((System.Double) obj));
						return (dec);
						
					case ProviderType.Decimal: 
						dec = (System.Decimal) obj;
						return (dec);
						
					case ProviderType.Char: 
						dec = System.Decimal.Parse(new String((char[]) obj));
						return (dec);
						
					case ProviderType.VarChar: // String
						dec = System.Decimal.Parse((System.String) obj);
						return (dec);

				}
					break;



				case ProviderType.Char:    // target
				case ProviderType.NChar:
				switch (objType)
				{
					case ProviderType.Boolean:
					case ProviderType.Binary:
					case ProviderType.TinyInt:
					case ProviderType.SmallInt:
					case ProviderType.Integer:
					case ProviderType.BigInt:
					case ProviderType.Single:
					case ProviderType.Double:
					case ProviderType.Decimal:
					case ProviderType.DateTime:
						/*
						** Convert to VARCHAR primary storage class.
						*/
						obj = obj.ToString();
						/*
						** Now fall through to do VARCHAR processing...
						*/
						goto case ProviderType.VarChar;

					case ProviderType.VarChar: // String
						str = (String) obj;

						if ( ! alt )		// Primary storage class
							return( ToCharArrayWithMaxSize(str, maxsize ) );
						else			// Alternate storage class
							try
							{ 
								str = ToStringWithMaxSize(str, maxsize);
								return( char_set.getBytes( str ));
							}
							catch( Exception ex )	// Should not happen!
							{ throw SqlEx.get( ERR_GC401E_CHAR_ENCODE, ex ); }
						
					case ProviderType.Char: 
						charArray = (char[])obj;
						charArray = ToCharArrayWithMaxSize(charArray, maxsize);

						if ( ! alt )		// Primary storage class
							return( charArray );
						else			// Alternate storage class
							try { return( char_set.getBytes( charArray )); }
							catch( Exception ex )	// Should not happen!
							{ throw SqlEx.get( ERR_GC401E_CHAR_ENCODE, ex ); }
						
				}
					break;
					
				
				
				case ProviderType.VarChar:    // target
				case ProviderType.NVarChar:
				switch (objType)
				{
					case ProviderType.Boolean:
					case ProviderType.Binary:
					case ProviderType.TinyInt:
					case ProviderType.SmallInt:
					case ProviderType.Integer:
					case ProviderType.BigInt:
					case ProviderType.Single:
					case ProviderType.Double:
					case ProviderType.Decimal:
					case ProviderType.DateTime:
						/*
						** Convert to VARCHAR primary storage class.
						*/
						obj = obj.ToString();
						/*
						** Now fall through to do VARCHAR processing...
						*/
						goto case ProviderType.VarChar;

					case ProviderType.VarChar: // String
						str = (String) obj;
						str = ToStringWithMaxSize(str, maxsize);

						if ( ! alt )		// Primary storage class
							return( str );
						else			// Alternate storage class
							try { return( char_set.getBytes( str )); }
							catch( Exception ex )	// Should not happen!
							{ throw SqlEx.get( ERR_GC401E_CHAR_ENCODE, ex ); }
						
					case ProviderType.Char: 
						charArray = (char[])obj;
						charArray = ToCharArrayWithMaxSize(charArray, maxsize);

						if ( ! alt )		// Primary storage class
							return( new String( charArray ));
						else			// Alternate storage class
							try { return( char_set.getBytes( charArray )); }
							catch( Exception ex )	// Should not happen!
							{ throw SqlEx.get( ERR_GC401E_CHAR_ENCODE, ex ); }
				}
					break;



				case ProviderType.LongVarChar:    // target
				case ProviderType.LongNVarChar:
				switch (objType)
				{
					case ProviderType.Boolean:
					case ProviderType.Binary:
					case ProviderType.TinyInt:
					case ProviderType.SmallInt:
					case ProviderType.Integer:
					case ProviderType.BigInt:
					case ProviderType.Single:
					case ProviderType.Double:
					case ProviderType.Decimal:
					case ProviderType.DateTime:
					case ProviderType.VarChar: // String
						str = obj.ToString();
						str = ToStringWithMaxSize(str, maxsize);
						return (new System.IO.StringReader(str));

					case ProviderType.Char: 
						charArray = (char[])obj;
						return (new CharArrayReader( charArray, maxsize ));
						
				}
					break;
					
				
				
				case ProviderType.Binary:
				case ProviderType.VarBinary:    // target
					System.Text.Encoding UTF8Encoding;

				switch (objType)
				{
					case ProviderType.Char: 
						UTF8Encoding = System.Text.Encoding.UTF8;
						obj = UTF8Encoding.GetBytes( (char[]) obj );
						goto case ProviderType.VarBinary;

					case ProviderType.VarChar: // String
						UTF8Encoding = System.Text.Encoding.UTF8;
						obj = UTF8Encoding.GetBytes( (String) obj );
						goto case ProviderType.VarBinary;
	
					case ProviderType.Binary:
						byte[] byte1Array = new Byte[1];
						byte1Array[0] = (byte)obj;
						obj = byte1Array;
						return (obj);

					case ProviderType.VarBinary:
						byteArray = (byte[]) obj;
						byteArray = ToByteArrayWithMaxSize(byteArray, maxsize);
						return (byteArray);
				}
					break;
					
				
				
				case ProviderType.LongVarBinary:    // target
				switch (objType)
				{
					case ProviderType.Char: 
						obj = str2bin(new String((char[]) obj));
						goto case ProviderType.VarBinary;

					case ProviderType.VarChar: // String
						obj = str2bin((System.String) obj);
						goto case ProviderType.VarBinary;

					case ProviderType.Binary:
						byte[] byte1Array = new Byte[1];
						byte1Array[0] = (byte)obj;
						obj = byte1Array;
						goto case ProviderType.VarBinary;

					case ProviderType.VarBinary: 
						byteArray = (byte[]) obj;
						return (new ByteArrayInputStream(byteArray, maxsize));
				}
					break;
				
			}
			
			throw new System.Exception("Invalid coercion: " + objType + " to " + sqlType);
		} // coerce


		/*
		** Name: checkSqlType
		**
		** Description:
		**	Validates that a SQL type is supported as a parameter type
		**	by this class.  Returns the supported parameter type after
		**	applying alias translations.
		**
		** Input:
		**	sqlType	    SQL type to validate.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	    Supported SQL type.
		**
		** History:
		**	19-May-99 (gordy)
		**	    Created.
		**	10-May-01 (gordy)
		**	    CHAR no longer forced to VARCHAR (character arrays fully 
		**	    supported), BINARY to VARBINARY (same storage format, but 
		**	    different transport format at Server), LONGVARCHAR
		**	    and LONGVARBINARY to VARCHAR and VARBINARY (IO classes
		**	    now supported).  NULL is now permitted.
		**	19-Feb-03 (gordy)
		**	    BIT now alias for BOOLEAN.
		**	15-Mar-04 (gordy)
		**	    BIGINT supported at DBMS/API protocol level 3.  Force back to
		**	    DECIMAL when not supported.  Method can no longer be static
		**	    due to need to check protocol level.
		**	    BOOLEAN is also now supported (but always as alternate format).
		*/

		private ProviderType
			checkSqlType( ProviderType sqlType )
		{
			switch( sqlType )
			{
				/*
				** Aliases
				*/
				case ProviderType.Numeric :
					sqlType = ProviderType.Decimal;    break;

				/*
				** BIGINT is supported starting at DBMS/API protocol level 3.
				** Otherwise, force to DECIMAL (BIGINT and DECIMAL use the
				** same alternate storage format to accomodate the change).
				*/
				case ProviderType.BigInt :
					return( (conn.db_protocol_level >= DBMS_API_PROTO_3)
						? ProviderType.BigInt : ProviderType.Decimal );

				/*
				** The following types are OK.
				*/
				case ProviderType.DBNull :
				case ProviderType.Boolean :
				case ProviderType.TinyInt :
				case ProviderType.SmallInt :
				case ProviderType.Integer :
				case ProviderType.Real :
				case ProviderType.Double :
				case ProviderType.Decimal :
				case ProviderType.Char :
				case ProviderType.VarChar :
				case ProviderType.LongVarChar :
				case ProviderType.Binary :
				case ProviderType.VarBinary :
				case ProviderType.LongVarBinary :
				case ProviderType.DateTime :
				case ProviderType.LongNVarChar :
				case ProviderType.NVarChar :
				case ProviderType.NChar :
					break;

					/*
					** All others are not allowed.
					*/
				default :
					throw SqlEx.get( ERR_GC401A_CONVERSION_ERR );
			}

			return( sqlType );
		} // checkSqlType


		/*
		** Name: str2bin
		**
		** Description:
		**	Converts a hex string into a byte array.
		**
		** Input:
		**	str	Hex string.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	byte[]	Byte array.
		**
		** History:
		**	10-May-01 (gordy)
		**	    Created.
		*/

		private byte[]
			str2bin( String str )
		{
			byte[] ba = new byte[str.Length / 2];
			int offset = 0, i = 0;
			
			for (; (offset + 1) < str.Length; offset += 2)
				ba[i++] = (byte) System.Convert.ToByte(
					str.Substring(offset, (offset + 2) - (offset)), 16);
			return (ba);
		}


		/*
		** Name: ToByteArrayWithMaxSize
		**
		** Description:
		**	Truncate parameter byte array down to Size maxsize.
		**
		** Input:
		**	byteArray	Byte array to limit.
		**	maxSize		Maximum size (as usually set by IDbDataParameter.Size.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	byte[]	new byte array, or the original byte[] if no maxsize.
		**
		** History:
		**	 9-Jan-03 (thoda04)
		**	    Created.
		*/
		
		private static byte[] ToByteArrayWithMaxSize(byte[] byteArray, int maxSize)
		{
			if (maxSize > 0  &&  maxSize < byteArray.Length)  //over max size?
			{
				byte[] newArray = new byte[maxSize];
				Array.Copy(byteArray, 0, newArray, 0, maxSize);
				return (newArray);  // return downsized array
			}
			return (byteArray);  // no choppping needed, return original
		}


		/*
		** Name: ToCharArrayWithMaxSize
		**
		** Description:
		**	Truncate parameter char array down to Size maxsize.
		**
		** Input:
		**	charArray	Character array to limit.
		**	maxsize		Maximum size (as usually set by IDbDataParameter.Size).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	char[]	new char array, or the original char[] if no maxsize.
		**
		** History:
		**	 9-Jan-03 (thoda04)
		**	    Created.
		*/
		
		private static char[] ToCharArrayWithMaxSize(char[] charArray, int maxSize)
		{
			if (maxSize > 0  &&  maxSize < charArray.Length)  //over max size?
			{
				char[] newArray = new Char[maxSize];
				Array.Copy(charArray, 0, newArray, 0, maxSize);
				return (newArray);  // return downsized array
			}
			return (charArray);  // no choppping needed, return original
		}

		private static char[] ToCharArrayWithMaxSize(string str, int maxSize)
		{
			if (maxSize > 0  &&  maxSize < str.Length)  //over max size?
				return str.ToCharArray(0, maxSize);
			return str.ToCharArray();   // no choppping needed
		}


		/*
		** Name: ToStringWithMaxSize
		**
		** Description:
		**	Truncate parameter string down to maxsize.
		**
		** Input:
		**	str    	String to limit.
		**	maxSize	Maximum size (as usually set by IDbDataParameter.Size).
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String	new substring, or the original string if no maxsize.
		**
		** History:
		**	 9-Jan-03 (thoda04)
		**	    Created.
		*/
		
		private static String ToStringWithMaxSize(String str, int maxSize)
		{
			if (maxSize > 0  &&  maxSize < str.Length)  //max size?
			{
				str = str.Substring(0, maxSize);  // chop
			}
			return (str);  // return original string or new substring
		}


		/*
		** Name: Param
		**
		** Description:
		**	Class representing an entry in a parameter data-set.
		**
		**  Public data
		**
		**	type	    SQL type.
		**	value	    Parameter value.
		**	flags	    Descriptor flags.
		**	name	    Parameter name.
		**
		** History:
		**	15-Nov-00 (gordy)
		**	    Created.
		**	11-Jan-01 (gordy)
		**	    This class is Cloneable since parameter object
		**	    values are immutable.
		**	25-Feb-03 (gordy)
		**	    Changed flags to short for additional bits.
		*/

		private class
			Param : ICloneable
		{

			public  ProviderType	type = ProviderType.DBNull;
			public  Object      	value = null;
			public  ushort      	flags = 0;
			public  String      	name = null;


			/*
			** Name: clone
			**
			** Description:
			**	Returns a copy of the current object.
			**
			** Input:
			**	None.
			**
			** Output:
			**	None.
			**
			** Returns:
			**	Object	Copy of this object.
			**
			** History:
			**	11-Jan-01 (gordy)
			**	    Created.
			*/

			public Object
				clone()
			{
				Object cpy = null;

				try { cpy = base.MemberwiseClone(); }
				catch( Exception ) {}   // Should not happen!

				return( cpy );
			}

			public  Object Clone()
			{
				return this.clone();
			}


			/*
			** Name: clear
			**
			** Description:
			**	Reset parameter fields to initial settings.
			**
			** Input:
			**	None.
			**
			** Output:
			**	None.
			**
			** Returns:
			**	void.
			**
			** History:
			**	15-Nov-00 (gordy)
			**	    Created.
			*/

			public void
				clear()
			{
				type = ProviderType.DBNull;
				value = null;
				flags = 0;
				name = null;
				return;
			} // clear


		} // class Param

	} // class ParamSet
}