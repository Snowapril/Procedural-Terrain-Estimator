#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP
#pragma once
#include <vector>
using namespace std;

typedef pair<int, int> pii;


class Estimator
{
public:
	vector < vector < unsigned char > > mapData;
	vector < vector < pii > > descentTable;
	int height;
	int width;
	// operator overloading
	vector < unsigned char >& operator [](int i);
	vector < unsigned char > const& operator[](int i) const;
	// methods

	Estimator(vector<unsigned char>& data, int _height, int _width);

	// operator overloading
	/*
	vector < unsigned char >& operator [](int i) {
		return mapData[i];
	}
	vector < unsigned char > const& operator[](int i) const {
		return mapData[i];
	}
	*/
	
	void dumpMapData();

	pii descent(int y, int x);

	bool hasCrator();

	void makeCoast(bool needCoast);
};
#endif