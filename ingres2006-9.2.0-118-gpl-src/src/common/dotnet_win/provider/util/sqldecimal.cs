/*
** Copyright (c) 2003, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.IO;
using System.Text;

namespace Ca.Ingres.Utility
{
	/*
	** Name: sqlDecimal.cs
	**
	** Description:
	**	Defines class which represents an SQL Decimal value.
	**
	**  Classes:
	**
	**	SqlDecimal	An SQL Decimal value.
	**
	** History:
	**	 5-Sep-03 (gordy)
	**	    Created.
	**	 9-Dec-03 (thoda04)
	**	    Ported to .NET.
	**	21-jun-04 (thoda04)
	**	    Cleaned up code for Open Source.
	**	15-Aug-06 (thoda04) B116461
	**	    Parse decimal string (with a possible decimal-point character)
	**	    with InvariantCulture, regardless of CurrentCulture.
	*/



	/*
	** Name: SqlDecimal
	**
	** Description:
	**	Class which represents an SQL Decimal value.  
	**
	**	Supports conversion to boolean, byte, short, integer, long, 
	**	float, double, String and Object.
	**
	**	The data value accessor methods do not check for the NULL
	**	condition.  The super-class isNull() method should first
	**	be checked prior to calling any of the accessor methods.
	**
	**	Decimal values are truncatable.  A truncated decimal value
	**	results in data value of 0.0.
	**
	**  Public Methods:
	**
	**	set			Assign a new data value.
	**	getBoolean		Data value accessor methods
	**	getByte
	**	getShort
	**	getInt
	**	getLong
	**	getFloat
	**	getDouble
	**	getDecimal
	**	getString
	**	getObject
	**
	**  Private Data:
	**
	**	value			The decimal value.
	**	truncated		Decimal value is truncted.
	**
	** History:
	**	 5-Sep-03 (gordy)
	**	    Created.
	*/

	internal class
		SqlDecimal : SqlData
		{

			private Decimal		value = Decimal.Zero;
			private bool   		truncated = false;


		/*
		** Name: SqlDecimal
		**
		** Description:
		**	Class constructor.  Data value is initially NULL.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	None.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public
			SqlDecimal() : base( true )
		{
		} // SqlDecimal


		/*
		** Name: set
		**
		** Description:
		**	Assign a new data value.  If the input is NULL,
		**	a NULL data value results.
		**
		** Input:
		**	value		The new data value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public void
			set( Decimal value )
		{
			setNotNull();
			this.value = value;

			truncated = false;
			return;
		} // set


		/*
		** Name: set
		**
		** Description:
		**	Assign a new data value.  If the input is NULL,
		**	a NULL data value results.
		**
		** Input:
		**	value		The new data value.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	void.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public void
			set( String value )
		{
			if ( value == null )
			{
				setNull();
				truncated = false;
			}
			else  if ( value.Length > 0 )
			{
				setNotNull();
				System.Globalization.CultureInfo invariantCulture =
					System.Globalization.CultureInfo.InvariantCulture;
				this.value = Decimal.Parse( value, invariantCulture );
				truncated = false;
			}
			else
			{
				setNotNull();
				this.value = new Decimal( 0.0 );
				truncated = true;
			}
			return;
		} // set


		/*
		** Name: isTruncated
		**
		** Description:
		**	Returns an indication that the data value was truncated.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean		True if truncated.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override bool
			isTruncated()
		{
			return( truncated );
		} // isTruncated


		/*
		** Name: getBoolean
		**
		** Description:
		**	Return the current data value as a boolean value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	boolean		Current value converted to boolean.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override bool
			getBoolean() 
		{
			return( (value == 0) ? false : true );
		} // getBoolean


		/*
		** Name: getByte
		**
		** Description:
		**	Return the current data value as a byte value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	byte	Current value converted to byte.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override byte 
			getByte() 
		{
			return( Decimal.ToByte(value) );
		} // getByte


		/*
		** Name: getShort
		**
		** Description:
		**	Return the current data value as a short value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	short	Current value converted to short.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override short 
			getShort() 
		{
			return( Decimal.ToInt16( value ));
		} // getShort


		/*
		** Name: getInt
		**
		** Description:
		**	Return the current data value as a integer value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	int	Current value converted to integer.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override int 
			getInt() 
		{
			return( Decimal.ToInt32( value ));
		} // getInt


		/*
		** Name: getLong
		**
		** Description:
		**	Return the current data value as a long value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	long	Current value converted to long.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override long 
			getLong() 
		{
			return( Decimal.ToInt64( value ) );
		} // getLong


		/*
		** Name: getFloat
		**
		** Description:
		**	Return the current data value as a float value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	float	Current value converted to float.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override float 
			getFloat() 
		{
			return( Decimal.ToSingle( value ) );
		} // getFloat


		/*
		** Name: getDouble
		**
		** Description:
		**	Return the current data value as a double value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	double	Current value converted to double.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override double 
			getDouble() 
		{
			return( Decimal.ToDouble( value ) );
		} // getDouble


		/*
		** Name: getDecimal
		**
		** Description:
		**	Return the current data value as a BigDecimal value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	BigDecimal	Current value converted to BigDecimal.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override Decimal 
			getDecimal() 
		{
			return( value );
		} // getDecimal


		/*
		** Name: getDecimal
		**
		** Description:
		**	Return the current data value as a BigDecimal value
		**	with a specific scale.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	scale		Desired scale.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	BigDecimal	Current value converted to BigDecimal.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public Decimal 
			getDecimal( int scale ) 
		{
			return( value );
//			return( value.setScale( scale, Decimal.ROUND_HALF_UP ) );
		} // getDecimal


		/*
		** Name: getString
		**
		** Description:
		**	Return the current data value as a String value.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	String		Current value converted to String.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override String 
			getString() 
		{
			return( value.ToString() );
		} // getString


		/*
		** Name: getObject
		**
		** Description:
		**	Return the current data value as a BigDecimal object.
		**
		**	Note: the value returned when the data value is 
		**	NULL is not well defined.
		**
		** Input:
		**	None.
		**
		** Output:
		**	None.
		**
		** Returns:
		**	Object	Current value converted to BigDecimal.
		**
		** History:
		**	 5-Sep-03 (gordy)
		**	    Created.
		*/

		public override Object 
			getObject() 
		{
			return( getDecimal() );
		} // getObject


	} // class SqlDecimal
}  // namespace
