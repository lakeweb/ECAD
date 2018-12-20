#pragma once


//gear_gen.h
//
//

// ..........................................................................
typedef struct gear_params
{

	//only three parameters to define a gear; angle in radians
	gear_params( size_t tooth_count, double presure_anglee, double diametrical_pitch, double center_bore= 0.0 )
		:tc( tooth_count )
		,pa( presure_anglee )
		,dp( diametrical_pitch )
		,cb( center_bore )
		,pd( (double)tooth_count / diametrical_pitch )
		,rd( ( tooth_count - 2 ) / diametrical_pitch )
		,ad( 1 / diametrical_pitch )
		,od( ( tooth_count + 2 ) / diametrical_pitch )
	{
		bd= pd * cos( pa );
		//thanks to: SprocketsAreNotGears at this link
		//http://engineering.stackexchange.com/questions/12944/involute-gear-calculation
		//offset angle for base radials
		of= ( atan( pa ) - pa ) * 2;
	}
	//given
	size_t tc;	//number of teeth
	double pa;	//presure angle, input as radians
	double dp;	//diametrical pitch, pitch circle
	double cb;	//center bore

	//calculated from given
	double pd; //pitch circle
	double rd; //root diameter = ( N - 2 ) / D;
	double ad; //addendum, inverse of dp
	double bd; //base diameter for involute
	double od; //outside diameter
	double of; //offset of base to pitch tooth half point
}gear_params_t, *pgear_params_t;

typedef boost::shared_ptr< gear_params_t > SP_gear_params_t;

