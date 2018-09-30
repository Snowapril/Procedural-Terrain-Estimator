#include "Estimator.hpp"
#include <queue>
#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;

bool visit[1024][1024];

const int DY[8] = { -1, -1, -1, 0, 1, 1, 1, 0 }, DX[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };

const unsigned char NEAR_GAP_ALLOWED = 3;
const unsigned char TOTAL_GAP_ALLOWED = 5;
const int MINIMUM_BASIN_AREA = 25;

Estimator::Estimator(vector<unsigned char>& data,int _height, int _width) : height(_height), width(_width) {
	HmapData.resize(height,vector<unsigned char>(width,0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			HmapData[i][j] = data[i*width + j];
		}
	}
}

void Estimator::dumpMapData() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%d ", HmapData[i][j]);
		}
		printf("\n");
	}
}

pii Estimator::descent(int y,int x) {
	pii& ans = descentTable[y][x];

	if (ans.first != -1) return ans;

	for (int i = 0; i < 4; i++) {
		int Y = y + DY[i], X = x + DX[i];
		if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
		if (HmapData[Y][X] <= HmapData[y][x]) {
			pii temp = descent(Y, X);
			if (ans.first == -1) {
				ans = temp;
			}
			else {
				if (HmapData[ans.first][ans.second] >= HmapData[temp.first][temp.second]) {
					ans = temp;
				}
			}
		}
	}
	if (ans.first == -1) ans = { y,x };

	return ans;
}

int Estimator::descentTabling() {
	descentTable.resize(height, vector<pii>(width, { -1,-1 }));
	vector < vector < int > > cnt(height, vector<int>(width, 0));
	vector < pii> localMinima;
	int curMax = 0;
	int ret = 0;
	pii target;

	// �����Ұ� : local minima�� �ƴ϶� global minima�� ���ϰ� �����Ƿ� descent table�� �� ä�� ���� for�� ���鼭 �ֺ��� �ڱ⺸�� ū ���� ���� �ָ� local minima�� ����־����

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pii temp = descent(i, j);
			cnt[temp.first][temp.second]++;
			int curVal = cnt[temp.first][temp.second];
			if ( curVal > curMax) {
				localMinima.clear();
				localMinima.push_back(temp);
				curMax = cnt[temp.first][temp.second];
			}
			else if (curVal == curMax) {
				localMinima.push_back(temp);
			}
		}
	}		
	
	memset(visit, 0, sizeof(visit));

	for (const pii curPoint : localMinima) {
		int sy = curPoint.first;
		int sx = curPoint.second;
		if (visit[sy][sx]) continue;
		int cnt = 0;
		visit[sy][sx] = true;
		queue < pii > Q;
		Q.push({ sy,sx });
		while (!Q.empty()) {
			cnt++;
			int y = Q.front().first, x = Q.front().second;
			Q.pop();
			for (int i = 0; i < 8; i++) {
				int Y = y + DY[i], X = x + DX[i];
				if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
				if (!visit[Y][X] && abs(HmapData[Y][X] - HmapData[y][x]) < NEAR_GAP_ALLOWED && abs(HmapData[Y][X] - HmapData[sy][sx]) < TOTAL_GAP_ALLOWED) {
					Q.push({ Y,X });
					visit[Y][X] = true;
				}
			}
		}
		if (cnt > MINIMUM_BASIN_AREA) ret++;
	}
	return ret;
}

void Estimator::makeCoast(const bool needCoast) {
	if (!needCoast) return;
	srand(time(NULL));
	int curCoast = width - width / 10;
	int probLeft = 40, probStay = 60;
	for (int i = 0; i < height; i++) {
		// printf("%d loop\n", i);
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
		for (int j = curCoast - 1; j > curCoast - 10; j--) {
			HmapData[i][j] = HmapData[i][j] / 10 * (curCoast - j);
		}
		for (int j = curCoast; j < width; j++) {
			HmapData[i][j] = 0;
		}
	}	
}

void Estimator::makeIsland(const bool needIsland, int radius = 100) {
	if (!needIsland) return;
	int probUp = 20, probDown = 40;
	int cut = radius / 10;
	srand(time(NULL));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int y = i - height / 2, x = j - width / 2;
			if (y*y + x * x > radius*radius) {
				HmapData[i][j] = 0;
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
					HmapData[k][j] = 0;
				}
			}
		}
	}
}

vector<unsigned char> Estimator::getHeightMap() {
	vector <unsigned char> ret;
	for (auto curVec : HmapData) {
		for (auto p : curVec) {
			ret.push_back(p); // R channel
			ret.push_back(p); // G channel
			ret.push_back(p); // B channel
		}
	}
	return ret;
}

vector<unsigned char> Estimator::getBlendMap() {
	vector <unsigned char> ret;
	for (auto curVec : BmapData) {
		for (auto p : curVec) {
			ret.push_back(p.r); // R channel
			ret.push_back(p.g); // G channel
			ret.push_back(p.b); // B channel
			ret.push_back(p.a); // A channel
		}
	}
	return ret;
}

void Estimator::blendmapColoring() {

	const pixel ROCK = { 255,0,0,0 };// 255 0 0 0 : ROCK
	const pixel DIRT = { 0,255,0,0 };	// 0 255 0 0 : DIRT
	const pixel MUD = { 0,0,255,0 };// 0 0 255 0 : MUD
	const pixel SAND = { 0,0,0,255 };// 0 0 0 255 : SAND

	BmapData.resize(height, vector<pixel>(width, { 0,0,0,0 }));
	
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			//��ó�� local minima���� �Ÿ��� ���� ���� ���̴� ������ �޶���
			int des_y = descentTable[i][j].first, des_x = descentTable[i][j].second;
			int wet_dist = (i - des_y) * (i - des_y) + (j - des_x) * (j - des_x);

			//��� �ĺ� : �������, local minima���� �Ÿ�, Ư�� ���� ���� ������ �� Ÿ��
		}
	}

}