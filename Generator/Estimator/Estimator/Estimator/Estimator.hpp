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
	// HeightMap�� stdout�� ���

	void dumpBlendMapData();
	// BlendMap�� stdout�� ���

	pii descent(int y, int x);
	// ��(y,x)�� ���ؼ� gradient descending�� �����ϴ� �Լ�

	int descentTabling();
	// ��� ���� ���ؼ� gradient descending�� �����ϰ� ������ ������ ��ȯ�ϴ� �Լ�

	void makeCoast(bool needCoast);
	// �ؾȼ� �����

	void makeIsland(bool needIsland, int radius);
	// �� �����

	pixel randFill(int areaHeight, int wetDistance, int wetHeightGap, int y, int x);
	// descentTabling(); �Լ��� ���� Local Minima(���� ���̴� ��)���� �Ÿ� ���� ������ �μ��� �޾Ƽ� ���� Ÿ���� �����ϴ� �Լ�(���� ���� ����)

	void blendmapColoring();
	// descentTabling�� �����Ͽ� ���� Ÿ�ϰ��� �����ϴ� �Լ�

	vector <unsigned char> getHeightMap();
	vector <unsigned char> getBlendMap();

	//HeightMap / BlendMap�� png ����� ���� vector of unsigned char�� ��ȯ�ϴ� �Լ�

};
#endif