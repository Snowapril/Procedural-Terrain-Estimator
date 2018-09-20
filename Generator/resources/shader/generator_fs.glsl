#version 430 core

// voronoi noise from https://www.shadertoy.com/view/lsjGWD
//

in vec2 noisePos;
out vec4 fragColors;

#define OCTAVES   		1		// 7
#define SWITCH_TIME 	60.0		// seconds

uniform float systemTime;

struct Voronoi {
	float blend;
    float function 		;//	= mod(t,4.0);
    float distance_type	 ;//   = mod(t/16.0,4.0);
    bool  multiply_by_F1;//	= mod(t,8.0)  >= 4.0;       
    bool  inverse		;//	= mod(t,16.0) >= 8.0;
};  

uniform Voronoi voronoi;

vec2 hash( vec2 p ){
	p = vec2( dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3)));
	return fract(sin(p)*43758.5453);
}

float voronoiNoise( vec2 x ){
	vec2 n = floor( x );
	vec2 f = fract( x );
	
	float F1 = 8.0;
	float F2 = 8.0;
	
	for( int j=-1; j<=1; j++ )
		for( int i=-1; i<=1; i++ ){
			vec2 g = vec2(i,j);
			vec2 o = hash( n + g );

			o = 0.5 + 0.41*sin( systemTime + 6.2831*o );	
			vec2 r = g - f + o;

		    float d = 	voronoi.distance_type < 1.0 ? dot(r,r)  :				// euclidean^2
		    		  	voronoi.distance_type < 2.0 ? sqrt(dot(r,r)) :			// euclidean
		    			voronoi.distance_type < 3.0 ? abs(r.x) + abs(r.y) :		// manhattan
		    			voronoi.distance_type < 4.0 ? max(abs(r.x), abs(r.y)) :	// chebyshev
		    			0.0;

		    if( d<F1 ) { 
		    	F2 = F1; 
		    	F1 = d; 
		    } else if( d<F2 ) {
		    	F2 = d;
		    }
    }
	
	float c = voronoi.function < 1.0 ? F1 : 
			  voronoi.function < 2.0 ? F2 : 
			  voronoi.function < 3.0 ? F2-F1 :
			  voronoi.function < 4.0 ? (F1+F2)/2.0 : 
			  0.0;
		
	if( voronoi.multiply_by_F1 )	c *= F1;
	if( voronoi.inverse )			c = 1.0 - c;
	
    return c;
}

void main(void)
{
    float voronoiValue = voronoiNoise(noisePos);

	float combinedNoise = voronoiValue * voronoi.blend;

    fragColors = vec4(vec3(combinedNoise), 1.0);
}