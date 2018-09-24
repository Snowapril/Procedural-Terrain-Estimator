#include "Estimator.hpp"
#include <queue>
#include <map>
#include <vector>

using namespace std;



Estimator::Estimator(vector<unsigned char>& data,int _height, int _width) : height(_height), width(_width) {
	mapData.resize(height,vector<unsigned char>(width,0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			mapData[i][j] = data[i*width + j];
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


pii Estimator::descent(int y,int x) {
	pii& ans = descentTable[y][x];

	if (ans.first != -1) return ans;

	const int dy[8] = { -1, -1, -1, 0, 1, 1, 1, 0 }, dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1};

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
	vector < vector < int > > cnt(height, vector<int>(width, 0));
	int curMax = 0;
	pii target;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pii temp = descent(i, j);
			cnt[temp.first][temp.second]++;
		}
	}		
	return true;
}

void Estimator::makeCoast(bool needCoast) {
	int coastLine = width / 10;


	
}