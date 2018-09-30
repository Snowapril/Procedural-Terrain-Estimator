#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <chrono>
#include <iostream>

inline glm::vec2 fract2(const glm::vec2& x)
{
	return glm::vec2(x[0] - static_cast<int>(x[0]), x[1] - static_cast<int>(x[1]));
}

float vnoise(float x, float y)
{
	glm::vec2 n(x, y);
	
	float F1 = 8.0f;
	for (int i = -1; i <= 1; ++i)
		for (int j = -1; j <= 1; ++j)
		{
			glm::vec2 g(i, j);
			glm::vec2 p(n + g);
			glm::vec2 o(fract2(glm::sin(glm::vec2(glm::dot(p, glm::vec2(127.1f, 311.7f)), glm::dot(p, glm::vec2(269.5f, 183.3f)))) * 43758.5453f));
			o = glm::vec2(0.5f, 0.5f) + 0.41f * glm::sin(glm::vec2(6.2831f, 6.2831f) * o);
			glm::vec2 r(g + o);

			float d = glm::dot(r, r);

			if (d < F1)
				F1 = d;
		}

	return F1;
}

float rand(glm::vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(glm::vec2 p, float freq ){
	float unit = screenWidth/freq;
	glm::vec2 ij = floor(p/unit);
	glm::vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(PI*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(float x,float y, int res){
    glm::vec2 p(x,y)
	float persistance = 0.5;
	float n = 0.0;
	float normK = 0.0;
	float f = 4.0;
	float amp = 1.0;
	int iCount = 0;
	for (int i = 0; i<50; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}

int main(void)
{
	auto start = std::chrono::system_clock::now();

	for (int i = 0; i < 1024; ++i)
		for (int j = 0; j < 1024; ++j)
			vnoise(i / 100.0f, j / 100.0f);

	auto end = std::chrono::system_clock::now();

	auto duration = std::chrono::duration<float>(end - start);
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
	
	std::cout << "Elapsed time : " << milliseconds.count() << " ms" << std::endl;

	return 0;
}