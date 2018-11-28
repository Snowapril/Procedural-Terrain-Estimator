#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP

#include <vector>
#include "Singleton.hpp"

using namespace std;

typedef pair<unsigned short,unsigned short> pss;

struct pixel {
	unsigned char r, g, b, a;
};

class Estimator : public Singleton<Estimator>
{
private:
	const int TERRAIN_SINK_DIVIDER = 4;
	const int DY[8] = { -1, -1, -1, 0, 1, 1, 1, 0 }, DX[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
	int DEFAULT_SEA_LEVEL = 128;
	
	vector < vector < unsigned short > > HmapData;
	vector < vector < pixel > > BmapData;
	vector < vector < pss > > descentTable;
	int height;
	int width;

public:
	Estimator() = default;
	Estimator(vector<unsigned short>& data, int _height, int _width);

	// methods
public:
	void blendmapColoring();
	// descentTabling을 실행하여 지형 타일값을 배정하는 함수

	void dumpDescentMapData();

	vector <unsigned short> getHeightMap();
	vector <unsigned char> getBlendMap();
	//HeightMap / BlendMap을 png 출력을 위해 vector of unsigned short로 반환하는 함수

	void dumpHeightMapData();
	// HeightMap을 stdout에 출력

	void dumpBlendMapData();
	// BlendMap을 stdout에 출력

	void initHMapData(unsigned int texture, int _width, int _height);
	void generateHeightMap(const char* path, int _width, int _height);
	void generateBlendMap(const char* path, int width, int height);
	unsigned int getBlendMapTexture(void) const;
private:
	pss descent(int y, int x);
	// 점(y,x)에 대해서 gradient descending을 실행하는 함수

	void descentTabling();
	// 모든 점에 대해서 gradient descending을 실행하고 분지의 개수를 반환하는 함수

	pixel randFill(int elevation, int dryDistance, int y, int x);
	// descentTabling(); 함수로 얻은 Local Minima(물이 고이는 곳)와의 거리 등의 정보를 인수로 받아서 지형 타일을 배정하는 함수(난수 적용 예정)

	void normalize(int minimumHeight = 0, int maximumHeight = 512);
	// minimumHeight ~ maximumHeight 값으로 재조정

	void smoothness();

	void bfsCoastlineOptimization();
	
	void linearCoastlineOptimization();
};


#endif