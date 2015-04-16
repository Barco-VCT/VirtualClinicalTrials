//////////////////////////////////////////////////////////////////
//							Container							//
//			MEVIC: MEdical Virtual Image  Chain					//
//				April 2008 - TIG group - Barco MID				//
//					Guillaume Spalla							//
//////////////////////////////////////////////////////////////////

/*!
* AbstractParameter.h
*  
* @author Guillaume Spalla
*/

#include <iostream>
#include <string>

#ifndef AbstractParameter_h
#define AbstractParameter_h

/*!
* @brief Virtual class used for the parameters.
* 
* A parameter will be represented in an XML file as values between square brackets
* 
* @author Guillaume Spalla
*/
class AbstractParameter{
public:
	/*!
	* Creation of the parameter (interpretation of the value between square brackets and initialization of the different variables).
	* 
	* @attention	Before use the other methods present in this class, call this one !
	* @param		parameterValue	Value of the parameter to process
	* @return		true if the process run correctly; false is a problem occured
	*/
	virtual bool	CreateParameter(std::string parameterValue)	=0;

	/*!
	* Return the current value of the parameter without modifiyng it
	*
	* @return		current value of the parameter. If the value to return is out of range, the returned value is ""
	*/
	virtual std::string	GetCurrentPosition()					=0;

	/*!
	* Return the value of the parameter, after incremented (i.e. if the current value is \e n, the returned value is \e n+1);
	*
	* @return value of the parameter after an incrementation. If the value to return is out of range, the returned value is ""
	*/
	virtual std::string	GetNextIterationValue()					=0;

	/*!
	* Check the consistency of the number of iterations of the parameter following a number given as input parameter. Useful to check is the number of iterations of the simulation is coherent with the number of possible values of the parameter.
	*
	* @param		nbExpected expected number of iterations the parameter should take
	* @return		* @return	0 if the parameter is cosistent; 1 is a warning: the simulation can run but all the possible values of the parameter will not be took in account; 2: error: too many iterations in comparison to the number of possible values of the parameter
	*/
	virtual int	CheckConsistency (int nbExpected)			=0;

	/*!
	* Return the current value of the parameter and then increment its value
	*
	* @return		value of the current parameter. If the value to return is out of range, the returned value is ""
	*/
	virtual std::string	GetCurrentPositionAndIterate()			=0;

	/*!
	* Iterate the value of the parameter of one unit (which can be one step, next value of the list, next file of the folder, ...)
	*
	* @return true if the iteration has been done; false if the value is out of range
	*/
	virtual bool	Iterate()								=0;

	/*
	* Derived classes need an implemented virtual destructor here to have their destructor called.
	*/
	virtual ~AbstractParameter() { }

private:
};

#endif

