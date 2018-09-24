#ifndef ESTIMATOR_HPP
#define ESTIMATOR_HPP

#include <vector>
using namespace std;

typedef pair<int, int> pii;


class Estimator
{
private:
	vector < vector < unsigned char > > mapData;

public:
	// operator overloading
	vector < unsigned char >& operator [](int i);
	vector < unsigned char > const& operator[](int i) const;
	// methods
	void mapDataInit(vector<unsigned char>& data, const int height, const int width);

	void dumpMapData(const int height, const int width);

	pii gradientDescent();
};
#endif