#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP
#pragma once
#include <vector>
using namespace std;

typedef pair<int,int> pii;

struct pixel {
	unsigned char r, g, b, a;
};

class Estimator
{
public:

	const int TERRAIN_SINK_DIVIDER = 4;
	const int DY[8] = { -1, -1, -1, 0, 1, 1, 1, 0 }, DX[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
	
	vector < vector < unsigned char > > HmapData;
	vector < vector < pixel > > BmapData;
	vector < vector < pii > > descentTable;
	int height;
	int width;


	// methods

	Estimator(vector<unsigned char>& data, int _height, int _width);

	
	void dumpHeightMapData();
	// HeightMap을 stdout에 출력

	void dumpBlendMapData();
	// BlendMap을 stdout에 출력

	pii descent(int y, int x);
	// 점(y,x)에 대해서 gradient descending을 실행하는 함수

	int descentTabling();
	// 모든 점에 대해서 gradient descending을 실행하고 분지의 개수를 반환하는 함수

	void makeCoast(bool needCoast);
	// 해안선 만들기

	void makeIsland(bool needIsland, int radius);
	// 섬 만들기

	pixel randFill(int areaHeight, int wetDistance, int wetHeightGap, int y, int x);
	// descentTabling(); 함수로 얻은 Local Minima(물이 고이는 곳)와의 거리 등의 정보를 인수로 받아서 지형 타일을 배정하는 함수(난수 적용 예정)

	void blendmapColoring();
	// descentTabling을 실행하여 지형 타일값을 배정하는 함수

	vector <unsigned char> getHeightMap();
	vector <unsigned char> getBlendMap();

	//HeightMap / BlendMap을 png 출력을 위해 vector of unsigned char로 반환하는 함수

};
#endif