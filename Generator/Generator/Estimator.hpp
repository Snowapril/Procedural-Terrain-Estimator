#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP

#include <vector>
using namespace std;

typedef pair<int, int> pii;

class Estimator
{
private:
	vector < vector < unsigned char > > mapData;
	vector < vector < pii > > descentTable;
	int height, width;
public:
	Estimator(vector<unsigned char>& data, int _height, int _width);
	// operator overloading
	vector < unsigned char >& operator [](int i);
	vector < unsigned char > const& operator[](int i) const;
	// methods
	void mapDataInit(vector<unsigned char>& data, const int height, const int width);
	void dumpMapData();
	pii descent(int y, int x);
	bool hasCrator();
};
#endif