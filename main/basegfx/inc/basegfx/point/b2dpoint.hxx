/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _BGFX_POINT_B2DPOINT_HXX
#define _BGFX_POINT_B2DPOINT_HXX

#include <basegfx/tuple/b2dtuple.hxx>
#include <basegfx/point/b2ipoint.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace basegfx
{
	// predeclaration
	class B2DHomMatrix;

	/** Base Point class with two double values

		This class derives all operators and common handling for
		a 2D data class from B2DTuple. All necessary extensions
		which are special for points will be added here.

		@see B2DTuple
	*/
	class B2DPoint : public ::basegfx::B2DTuple
	{
	public:					
		/**	Create a 2D Point

        	The point is initialized to (0.0, 0.0)
		*/
		B2DPoint() 
		:	B2DTuple()
		{}

		/**	Create a 2D Point

			@param fX
			This parameter is used to initialize the X-coordinate
			of the 2D Point.
			
			@param fY
			This parameter is used to initialize the Y-coordinate
			of the 2D Point.
		*/
		B2DPoint(double fX, double fY) 
		:	B2DTuple(fX, fY)
		{}

		/**	Create a copy of a 2D Point

			@param rPoint
			The 2D Point which will be copied.
		*/
		B2DPoint(const B2DPoint& rPoint) 
		:	B2DTuple(rPoint)
		{}

		/**	Create a copy of a 2D Point

			@param rPoint
			The 2D Point which will be copied.
		*/
		B2DPoint(const ::basegfx::B2IPoint& rPoint) 
		:	B2DTuple(rPoint)
		{}

		/** constructor with tuple to allow copy-constructing
			from B2DTuple-based classes
		*/
		B2DPoint(const ::basegfx::B2DTuple& rTuple) 
		:	B2DTuple(rTuple)
		{}

		~B2DPoint() 
		{}

		/** *=operator to allow usage from B2DPoint, too
		*/
		B2DPoint& operator*=( const B2DPoint& rPnt ) 
		{ 
			mfX *= rPnt.mfX;
			mfY *= rPnt.mfY; 
			return *this; 
		}

		/** *=operator to allow usage from B2DPoint, too
		*/
		B2DPoint& operator*=(double t) 
		{ 
			mfX *= t; 
			mfY *= t; 
			return *this; 
		}

		/** assignment operator to allow assigning the results
			of B2DTuple calculations
		*/
		B2DPoint& operator=( const ::basegfx::B2DTuple& rPoint );

		/** Transform point by given transformation matrix.

        	The translational components of the matrix are, in
        	contrast to B2DVector, applied.
		*/
		B2DPoint& operator*=( const ::basegfx::B2DHomMatrix& rMat );

		static const B2DPoint& getEmptyPoint()
		{
			return (const B2DPoint&) ::basegfx::B2DTuple::getEmptyTuple();
		}
	};

	// external operators
	//////////////////////////////////////////////////////////////////////////

	/** Transform B2DPoint by given transformation matrix.
	    
		Since this is a Point, translational components of the
    	matrix are used.
	*/
	B2DPoint operator*( const B2DHomMatrix& rMat, const B2DPoint& rPoint );
} // end of namespace basegfx

//////////////////////////////////////////////////////////////////////////////

#endif /* _BGFX_POINT_B2DPOINT_HXX */
