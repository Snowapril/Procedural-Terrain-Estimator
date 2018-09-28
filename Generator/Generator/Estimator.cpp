#include "Estimator.hpp"
#include <queue>
#include <map>

using namespace std;

Estimator::Estimator(vector<unsigned char>& data, int _height, int _width) 
	: height(_height), width(_width) 
{
	mapDataInit(data, height, width);
}
// operator overloading
vector < unsigned char >& Estimator::operator [](int i) {
	return mapData[i];
}
vector < unsigned char > const& Estimator::operator[](int i) const {
	return mapData[i];
}

// methods
void Estimator::mapDataInit(vector<unsigned char>& data, const int height, const int width) {
	mapData.resize(height);
	for (int i = 0; i < height; i++) {
		mapData[i].resize(width);
		for (int j = 0; j < width; j++) {
			mapData[i][j] = data[i*width + j * 3];
		}
	}
}

void Estimator::dumpMapData() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%d ", mapData[i][j]);
		}
		printf("\n");
	}
}


pii Estimator::descent(int y, int x) {
	pii& ans = descentTable[y][x];

	if (ans.first != -1) return ans;

	const int dy[4] = { -1,1,0,0 }, dx[4] = { 0,0,-1,1 };

	for (int i = 0; i < 4; i++) {
		int Y = y + dy[i], X = x + dx[i];
		if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
		if (mapData[Y][X] <= mapData[y][x]) {
			pii temp = descent(Y, X);
			if (ans.first == -1) {
				ans = temp;
			}
			else {
				if (mapData[ans.first][ans.second] >= mapData[temp.first][temp.second]) {
					ans = temp;
				}
			}
		}
	}
	if (ans.first == -1) ans = { y,x };

	return ans;
}

bool Estimator::hasCrator() {
	descentTable.resize(height, vector<pii>(width, { -1,-1 }));
	map <pii, int> cnt;
	int curMax = 0;
	pii target;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int temp = cnt[descent(i, j)]++;
			if (temp > curMax) {
				curMax = temp;
				target = { i,j };
			}
		}
	}
	return true;
}