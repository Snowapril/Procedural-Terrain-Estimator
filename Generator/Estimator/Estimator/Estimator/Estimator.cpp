#include "Estimator.hpp"
#include <queue>
#include <map>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
using namespace std;

Estimator::Estimator(vector<unsigned char>& data,int _height, int _width) : height(_height), width(_width) {
	HmapData.resize(height,vector<unsigned char>(width,0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			HmapData[i][j] = data[i*width + j];
		}
	}
}

void Estimator::dumpHeightMapData() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%d ", HmapData[i][j]);
		}
		printf("\n");
	}
}

void Estimator::dumpBlendMapData() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("%d ", BmapData[i][j].r);
			printf("%d ", BmapData[i][j].g);
			printf("%d ", BmapData[i][j].b);
			printf("%d ", BmapData[i][j].a);
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

	const unsigned char NEAR_GAP_ALLOWED = 3;
	const unsigned char TOTAL_GAP_ALLOWED = 5;
	const int MINIMUM_BASIN_AREA = 25;

	descentTable.resize(height, vector<pii>(width, { -1,-1 }));
	vector < vector < int > > cnt(height, vector<int>(width, 0));
	vector < vector <bool> > visit(height, vector<bool>(width, false));
	vector < pii> localMinima;
	int curMax = 0;
	int ret = 0;
	pii target;

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

	const int PROBLEFT_INIT = 40, PROBSTAY_INIT = 60;
	const int PROB_DIFF = 2, COAST_DIFF = 3;

	if (!needCoast) return;
	srand(time(NULL));
	int curCoast = width - width / 10;
	int probLeft = PROBLEFT_INIT, probStay = PROBSTAY_INIT;
	for (int i = 0; i < height; i++) {
		int prob = rand() % 100;
		if (prob < probLeft) {
			probLeft -=2 * PROB_DIFF;
			probStay += PROB_DIFF;
			curCoast-=COAST_DIFF;
		}
		else if (prob < probStay) {
			probLeft+= PROB_DIFF;
			probStay -= 2 * PROB_DIFF;
		}
		else {
			probLeft+= PROB_DIFF;
			probStay+= PROB_DIFF;
			curCoast+=COAST_DIFF;
		}
		for (int j = TERRAIN_SINK_DIVIDER; j > 0; j--) {
			int k = curCoast - j;
			HmapData[i][k] = HmapData[i][k] * j / TERRAIN_SINK_DIVIDER;
		}
		for (int j = curCoast; j < width; j++) {
			HmapData[i][j]/=TERRAIN_SINK_DIVIDER;
		}
	}	
}

void Estimator::makeIsland(const bool needIsland, int radius = 100) {

	const int PROBUP_INIT = 20, PROBDOWN_INIT = 40;
	const int CUT_INIT = radius / 10;
	const int CUT_DIFF = 1, PROB_DIFF = 1;

	if (!needIsland) return;
	int probUp = PROBDOWN_INIT, probDown = PROBDOWN_INIT;
	int cut = CUT_INIT;
	vector < vector <bool> > visited(height, vector<bool>(width, false));
	srand(time(NULL));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int y = i - height / 2, x = j - width / 2;
			if (y*y + x * x > radius*radius) {
				HmapData[i][j]/=TERRAIN_SINK_DIVIDER;
				visited[i][j] = true;
				int prob = rand() % 100;
				if (prob < probUp) {
					cut+= CUT_DIFF;
					probUp -= PROB_DIFF;
				}
				else if (prob < probDown) {
					cut -= CUT_DIFF;
					probUp += CUT_DIFF;
				}

				for (int k = i - cut; k <= i + cut; k++) {
					if (k < 0 || k >= height) continue;
					int yy = k - height / 2;
					if (yy*yy + x * x > radius*radius) continue;
					if (visited[k][j]) continue;
					HmapData[k][j] /= TERRAIN_SINK_DIVIDER;
					visited[k][j] = true;
				}
				for (int k = j - cut; k <= j + cut; k++) {
					if (k < 0 || k >= width) continue;
					int xx = k - width / 2;
					if (y*y + xx * xx > radius*radius) continue;
					if (visited[i][k]) continue;
					HmapData[i][k] /= TERRAIN_SINK_DIVIDER;
					visited[i][k] = true;
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

pixel Estimator::randFill(int areaHeight, int wetDistance, int wetHeightGap, int y,int x) {

	const unsigned char DATA_NUM = 4;
	pixel tile[DATA_NUM];
	pixel ret;
	int prob[DATA_NUM] = { 0, };

	tile[0] = { 255,0,0,0 };// 255 0 0 0 : ROCK
	tile[1] = { 0,255,0,0 };// 0 255 0 0 : DIRT
	tile[2] = { 0,0,255,0 };// 0 0 255 0 : MUD
	tile[3] = { 0,0,0,255 };// 0 0 0 255 : SAND

	const int dryTable[DATA_NUM] = { 25,50,100,1e9 };

	int dryMeter = wetHeightGap * 8 / 10 + wetDistance * 2 / 10;

	for (int i = 0; i < DATA_NUM; i++) {
		if (dryMeter <= dryTable[i]) {
			ret = tile[i];
			break;
		}
	}

	return ret;
}


void Estimator::blendmapColoring() {

	descentTabling();

	srand(time(NULL));

	const pixel ROCK = { 255,0,0,0 };// 255 0 0 0 : ROCK
	const pixel DIRT = { 0,255,0,0 };	// 0 255 0 0 : DIRT
	const pixel MUD = { 0,0,255,0 };// 0 0 255 0 : MUD
	const pixel SAND = { 0,0,0,255 };// 0 0 0 255 : SAND

	BmapData.resize(height, vector<pixel>(width, { 0,0,0,0 }));
	
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			int des_y = descentTable[i][j].first, des_x = descentTable[i][j].second;
			int wet_dist = (int)sqrt((double) ((i - des_y) * (i - des_y) + (j - des_x) * (j - des_x)));
			int wet_height = HmapData[i][j] - HmapData[des_y][des_x];
			
			BmapData[i][j] = randFill(HmapData[i][j], wet_dist, wet_height,i, j);

		}
	}

}
