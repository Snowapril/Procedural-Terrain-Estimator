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
	vector < vector < unsigned char > > HmapData;
	vector < vector < pixel > > BmapData;
	vector < vector < pii > > descentTable;
	int height;
	int width;


	// methods

	Estimator(vector<unsigned char>& data, int _height, int _width);

	
	void dumpMapData();

	pii descent(int y, int x);

	int descentTabling();

	void makeCoast(bool needCoast);

	void makeIsland(bool needIsland, int radius);

	void blendmapColoring();

	vector <unsigned char> getHeightMap();
	vector <unsigned char> getBlendMap();

};
#endif