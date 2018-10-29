#version 430 core

// glsl noise functions source code from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

in vec2 noisePos;
out vec4 fragColors;

uniform sampler2D voronoiBoard;
uniform sampler2D simplexBoard;
uniform sampler2D fbMBoard;

struct Voronoi {
	float frequency;
	float blend;
    float function 		;//	= mod(t,4.0);
    float distance_type	 ;//   = mod(t/16.0,4.0);
    bool  multiply_by_F1;//	= mod(t,8.0)  >= 4.0;       
    bool  inverse		;//	= mod(t,16.0) >= 8.0;
};  

struct Simplex {
	float frequency;
	float blend;
};

struct FBM {
	int num_octaves;
	float blend;
	float frequency;
} ;

uniform Voronoi voronoi;
uniform Simplex simplex;
uniform FBM fbM;

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

			o = 0.5 + 0.41*sin( 6.2831*o );	
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

vec3 permute(vec3 x) { return mod(((x*34.0) + 1.0)*x, 289.0); }

float snoise(vec2 v) {
	const vec4 C = vec4(0.211324865405187, 0.366025403784439,
		-0.577350269189626, 0.024390243902439);
	vec2 i = floor(v + dot(v, C.yy));
	vec2 x0 = v - i + dot(i, C.xx);
	vec2 i1;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	i = mod(i, 289.0);
	vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
		+ i.x + vec3(0.0, i1.x, 1.0));
	vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy),
		dot(x12.zw, x12.zw)), 0.0);
	m = m * m;
	m = m * m;
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	m *= 1.79284291400159 - 0.85373472095314 * (a0*a0 + h * h);
	vec3 g;
	g.x = a0.x  * x0.x + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float rand(vec2 n) {
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n) {
	const vec2 d = vec2(0.0, 1.0);
	vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float fbm(vec2 x) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100);
	// Rotate to reduce axial bias
	mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < fbM.num_octaves; ++i) {
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

vec2 calculateTexCoords(vec2 pos)
{
	return pos * 0.5 + 0.5;
}

const float EPSILON = 1e-8;

void main(void)
{
	vec2 texCoords = calculateTexCoords(noisePos);

	float voronoiBrush = texture(voronoiBoard, texCoords).r;
	float simplexBrush = texture(simplexBoard, texCoords).r;
	float fbMBrush = texture(fbMBoard, texCoords).r;

    float voronoiValue = voronoiNoise(noisePos * vec2(voronoi.frequency)) * voronoiBrush;
	float simplexValue = snoise(noisePos * vec2(simplex.frequency)) * simplexBrush;
	float fbMvalue = fbm(noisePos * vec2(fbM.frequency)) * fbMBrush;
	
	float blendSum = fbM.blend + simplex.blend + voronoi.blend + EPSILON;
	float combinedNoise = (voronoiValue * voronoi.blend + simplexValue * simplex.blend + fbMvalue * fbM.blend) / blendSum;

    fragColors = vec4(vec3(combinedNoise) , 1.0);
}