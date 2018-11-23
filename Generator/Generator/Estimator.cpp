#include "Estimator.hpp"
#include <queue>
#include <map>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cstring>
#include <glad/glad.h>
#include "SN_Rombauts.h"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb/stb_image_write.h>
using namespace std;

std::shared_ptr<Estimator> Singleton<Estimator>::instance(new Estimator());

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

void Estimator::dumpDescentMapData() {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			printf("(%d,%d) ", descentTable[i][j].first, descentTable[i][j].second);
		}
		printf("\n");
	}
}

void Estimator::initHMapData(unsigned int texture, int _width, int _height) {
	width  = _width;
	height = _height;

	std::vector<unsigned char> data(width * height);

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, (void*)&data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	HmapData.resize(height, vector<unsigned char>(width, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			HmapData[i][j] = data[i*width + j];
		}
	}
}


void Estimator::generateBlendMap(const char* path, int width, int height) {

	vector<unsigned char> data;
	data.reserve(width * height * 4);

	for (const auto& row : BmapData) {
		for (const auto& element : row) {
			data.push_back(element.r);
			data.push_back(element.g);
			data.push_back(element.b);
			data.push_back(element.a);
		}
	}

	stbi_write_png(path, width, height, 4, &data[0], 0);
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

void Estimator::descentTabling() {

	descentTable.resize(height, vector<pii>(width, { -1,-1 }));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pii temp = descent(i, j);
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

pixel Estimator::randFill(int elevation, int dryDistance, int y,int x) {

	const unsigned static char DATA_NUM = 4;
	static pixel tile[DATA_NUM];
	static pixel ret;
	int prob[DATA_NUM] = { 0, };

	tile[0] = { 0,0,255,0 };// 0 0 255 0 : MUD	
	tile[1] = { 0,255,0,0 };// 0 255 0 0 : DIRT
	tile[2] = { 0,0,0,255 };// 0 0 0 255 : SAND
	tile[3] = { 255,0,0,0 };// 255 0 0 0 : ROCK

	const static int elevationTable[DATA_NUM] = { DEFAULT_SEA_LEVEL * 8 / 10, DEFAULT_SEA_LEVEL, DEFAULT_SEA_LEVEL * 12 / 10, 1e9 };

	int select = 0;
	for (int i = 0; i < DATA_NUM; i++) {
		if (elevation <= elevationTable[i]) {
			select = i;
			break;
		}
	}

	int transitionPercentage = 50 + exp(dryDistance * 50 / elevationTable[select]);
	
	int probabillity = rand()%100;

	if (probabillity > transitionPercentage) select++;

	ret = tile[select];

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
			
			BmapData[i][j] = randFill(HmapData[i][j], wet_dist,i, j);
		}
	}
}

void Estimator::normalize(int minimumHeight, int maximumHeight) {
	DEFAULT_SEA_LEVEL = (minimumHeight + maximumHeight) / 4;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			HmapData[i][j] = minimumHeight + HmapData[i][j] / 512 * (maximumHeight - minimumHeight);
		}
	}
}

void Estimator::smoothness() {
	vector < vector < short > > converted(height, vector<short>(width, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			int res = 0;
			int div = 0;

			for (int k = 0; k < 8; k++) {
				int Y = i+DY[k], X = j+DX[k];
				if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
				div++;
				res += HmapData[Y][X];
			}

			converted[i][j] = (HmapData[i][j] + res / div ) / 2;
		}
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			HmapData[i][j] = converted[i][j];
		}
	}
}

void Estimator::bfsCoastlineOptimization() {
	queue <pii> Q;
	vector < vector < bool > > visit(height, vector<bool>(width, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (!visit[i][j] && HmapData[i][j] <= DEFAULT_SEA_LEVEL) {
				visit[i][j] = true;
				Q.push({ i,j });
				while (!Q.empty()) {
					int y = Q.front().first, x = Q.front().second;
					Q.pop();
					for (int k = 0; k < 8; k++) {
						int Y = y + DY[k], X = x+ DX[k];
						if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
						if (visit[Y][X]) continue;
						if (HmapData[Y][X] <= DEFAULT_SEA_LEVEL) {
							visit[Y][X] = true;
							Q.push({ Y,X });
						}
						else {
							short prob = rand() % 100;
							double logVal = log(height) / log(DEFAULT_SEA_LEVEL + 10) * 100;
							short transitionProbability = (int)(100 - (int)(logVal))/2;
							if (prob > transitionProbability) {
								HmapData[Y][X] = (HmapData[Y][X] + HmapData[y][x]) / 2;
								visit[Y][X] = true;
								Q.push({ Y,X });
							}
						}
					}
				}
			}
		}
	}
}

void Estimator::linearCoastlineOptimization() {

	static const short DY1[4] = { -1,-1,-1,0 };
	static const short DX1[4] = { -1,0,1,1 };
	static const short DY2[4] = { 1,1,1,0 };
	static const short DX2[4] = { 1,0,-1,-1 };
	int ydir, xdir;

	vector < vector < bool > > visit(height, vector<bool>(width, 0));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (!visit[i][j] && HmapData[i][j] > DEFAULT_SEA_LEVEL) {
				visit[i][j] = true;
				for (int k = 0; k < 4; k++) {
					int Y1 = i + DY1[k], X1 = j + DX1[k];
					int Y2 = i + DY2[k], X2 = j + DY2[k];
					int direction = ((int)(HmapData[Y2][X2] > DEFAULT_SEA_LEVEL) - (int)(HmapData[Y1][X1] > DEFAULT_SEA_LEVEL));
					ydir += (DY1[k] - DY2[k]) / 2 * direction;
					xdir += (DX1[k] - DX2[k]) / 2 * direction;
				}
				if (ydir == 0 && xdir == 0) continue;
				ydir = -ydir;
				xdir = -xdir;
				int d = (int)sqrt(xdir * xdir + ydir * ydir);
				int noiseDistance = SN_Rombauts::noise((float)(i+j));
				/*
				for(int y = i; )
				*/
			}
		}
	}
}