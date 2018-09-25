#include "Estimator.hpp"
#include <queue>
#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
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

vector <pii> gscan() {
	vector <pii> stk;
	int ss = stk.size();
	for (int i = 0; i < piis.size(); i++) {
		while (ss >= 2 && ccw(stk[ss - 2], stk.back(), piis[i]) <= 0) {
			stk.pop_back();
			ss--;
		}
		stk.push_back(piis[i]);
	}
	return stk;
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
	if (!needCoast) return;
	srand(time(NULL));
	int coastLine = width / 10;
	int curCoast = coastLine;
	int probLeft = 40, probStay = 60;
	for (int i = 0; i < height; i++) {
		int prob = rand() % 100;
		if (prob < probLeft) {
			probLeft -= 8;
			probStay += 4;
			curCoast--;
		}
		else if (prob < probStay) {
			probLeft += 4;
			probStay -= 8;
		}
		else {
			probLeft += 4;
			probStay += 4;
			curCoast++;
		}
		for (int j = curCoast - 1; j > curCoast - 10; j++) {
			mapData[i][j] = mapData[i][j] / 10 * (curCoast - j);
		}
		for (int j = curCoast; j < width; j++) {
			mapData[i][j] = 0;
		}
	}	
}

void Estimator::makeIsland(bool needIsland, int radius = 0) {
	if (!needIsland) return;
	int probUp = 20, probDown = 40;
	int cut = radius / 10;
	srand(time(NULL));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int y = i - height / 2, x = j - width / 2;
			if (i*i + j * j > radius*radius) {
				mapData[i][j] = 0;
				int prob = rand() % 100;
				if (prob < probUp) {
					cut++;
					probUp -= 4;
				}
				else if (prob < probDown) {
					cut--;
					probUp += 4;
				}

				for (int k = i - cut; k <= i + cut; k++) {
					if (k < 0 || k >= height) continue;
					mapData[k][j] = 0;
				}
			}
		}
	}
}