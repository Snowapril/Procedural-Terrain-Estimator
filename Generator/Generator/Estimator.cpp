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
#include "GLResources.hpp"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb/stb_image_write.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

std::shared_ptr<Estimator> Singleton<Estimator>::instance(new Estimator());

Estimator::Estimator(vector<unsigned short>& data, int _height, int _width) : height(_height), width(_width) {
	HmapData.resize(height, vector<unsigned short>(width, 0));
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
	width = _width;
	height = _height;

	std::vector<unsigned short> data(width * height);

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_SHORT, (void*)&data[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	HmapData.resize(height, vector<unsigned short>(width));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			HmapData[i][j] = data[i*width + j];
		}
	}
}

void Estimator::generateHeightMap(const char* path, int _width, int _height) {
	//TODO :
	std::vector<unsigned short> data(width * height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			data[i * width + j] = HmapData[i][j];
		}
	}

	Mat src = Mat(cv::Size(width, height), CV_16UC1, (void*)&data[0]), dst;
	resize(src, dst, cv::Size(_width, _height));
	imwrite(path, dst);
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
unsigned int Estimator::getBlendMapTexture(void)
{
	vector<unsigned char> data;
	data.reserve(width * height);

	for (const auto& row : BmapData) {
		for (const auto& element : row) {
			data.push_back(element.r);
			data.push_back(element.g);
			data.push_back(element.b);
		}
	}

	if (blendMapTexture) glDeleteTextures(1, &blendMapTexture);
	blendMapTexture = GLResources::CreateTexture2D(data, width, height, 3, false);

	return blendMapTexture;
}

pss Estimator::descent(short y, short x) {

	pss& ans = descentTable[y][x];

	if (ans.first != -1) return ans;

	for (int i = 0; i < 4; i++) {
		int Y = y + DY[i], X = x + DX[i];
		if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
		if (HmapData[Y][X] <= HmapData[y][x]) {
			pss temp = descent(Y, X);
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

	descentTable.resize(height, vector<pss>(width, { -1,-1 }));

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pss temp = descent(i, j);
		}
	}
}

vector<unsigned short> Estimator::getHeightMap() {
	vector <unsigned short> ret;
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

pixel Estimator::randFill(int dryDistance, short y,short x) {

	const int DRY_LOWER_BOUND = (width / 20) * (width / 20);
	const int DRY_UPPER_BOUND = (width / 5) * (width / 5);

	unsigned short elevation = HmapData[y][x] % 256;

	const unsigned int DATA_NUM = 4;
	pixel tile[DATA_NUM];
	pixel ret;

	tile[0] = { 0,0,255,0 };// 0 0 255 0 : MUD	
	tile[1] = { 0,255,0,0 };// 0 255 0 0 : DIRT
	tile[2] = { 0,0,0,255 };// 0 0 0 255 : SAND
	tile[3] = { 255,0,0,0 };// 255 0 0 0 : ROCK

	unsigned int elevationTable[DATA_NUM] = { DEFAULT_SEA_LEVEL * 6 / 10, DEFAULT_SEA_LEVEL, DEFAULT_SEA_LEVEL * 2, DEFAULT_SEA_LEVEL * 4 };

	int select = 0;
	for (int i = 0; i < DATA_NUM; i++) {
		if (elevation <= elevationTable[i]) {
			select = i;
			break;
		}
	}
	if (dryDistance < DRY_LOWER_BOUND && select > 0) {
		select--;
	}
	else if (dryDistance > DRY_UPPER_BOUND && select < 3) {
		select++;
	}

	ret = tile[select];

	return ret;
}


void Estimator::blendmapColoring() {
	bfsCoastlineOptimization();
	descentTabling();

	srand(time(NULL));

	const pixel ROCK = { 255,0,0,0 };// 255 0 0 0 : ROCK
	const pixel DIRT = { 0,255,0,0 };	// 0 255 0 0 : DIRT
	const pixel MUD = { 0,0,255,0 };// 0 0 255 0 : MUD
	const pixel SAND = { 0,0,0,255 };// 0 0 0 255 : SAND

	BmapData.resize(height, vector<pixel>(width, { 0,0,0,0 }));
	unsigned short _max = 0;
	
	for (short i = 0; i < height; i++) {
		for (short j = 0; j < width; j++) {

			unsigned short des_y = descentTable[i][j].first, des_x = descentTable[i][j].second;
			int wet_dist = (int)((i - des_y) * (i - des_y) + (j - des_x) * (j - des_x));

			_max = max(HmapData[i][j], _max);
			BmapData[i][j] = randFill(wet_dist, i, j);
		}
	}
	printf("%d\n", _max % 256);
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
	vector < vector < unsigned short > > converted(height, vector<unsigned short>(width, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			int res = 0;
			int div = 0;

			for (int k = 0; k < 8; k++) {
				int Y = i + DY[k], X = j + DX[k];
				if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
				div++;
				res += HmapData[Y][X];
			}

			converted[i][j] = (HmapData[i][j] + res / div) / 2;
		}
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			HmapData[i][j] = converted[i][j];
		}
	}
}

void Estimator::bfsCoastlineOptimization() {
	queue <pss> Q;
	vector < vector < unsigned char > > visit(height, vector< unsigned char>(width, 0));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (!visit[i][j] && HmapData[i][j] <= DEFAULT_SEA_LEVEL) {
				visit[i][j] = true;
				Q.push({ i,j });
				while (!Q.empty()) {
					int y = Q.front().first, x = Q.front().second;
					Q.pop();
					for (int k = 0; k < 8; k++) {
						int Y = y + DY[k], X = x + DX[k];
						if (Y < 0 || Y >= height || X < 0 || X >= width) continue;
						if (visit[Y][X]) continue;
						if (HmapData[Y][X] <= DEFAULT_SEA_LEVEL) {
							visit[Y][X] = true;
							Q.push({ Y,X });
						}
						else {
							unsigned short prob = rand() % 100;
							double logVal = log(height) / log(DEFAULT_SEA_LEVEL + 10) * 100;
							unsigned short transitionProbability = (int)(100 - (int)(logVal)) / 2;
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

	const short DY1[4] = { -1,-1,-1,0 };
	const short DX1[4] = { -1,0,1,1 };
	const short DY2[4] = { 1,1,1,0 };
	const short DX2[4] = { 1,0,-1,-1 };
	const short MEASURE = 6;
	int ydir, xdir;

	vector < vector < unsigned char > > visit(height, vector< unsigned char>(width, 0));

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

				//방향 전환
				ydir = -ydir;
				xdir = -xdir;


				int d = (int)sqrt(xdir * xdir + ydir * ydir);
				int noiseDistance = SN_Rombauts::noise((float)(i + j));

				/*
					* 방향 벡터 : (ydir , xdir)
					* 방향 거리 : noiseDistance
					* SN_Rombauts::noise(x) 의 최대 value : 2.53125
					* ydir / d 는 1보다 작음
					* -> (ydir / d * noiseDistance, xdir / d * noiseDistance)의 범위
					*/

				unsigned short COASTLINE_CONST = 2;

				ydir = ydir * noiseDistance * COASTLINE_CONST / d;
				xdir = xdir * noiseDistance * COASTLINE_CONST / d;

				short ydiff = ydir / MEASURE;
				short xdiff = xdir / MEASURE;

				for (int y = i, x = j; y != i + ydir && x != j + xdir; y += ydiff, x += xdiff) {
					if (y < 0 || y >= height || x < 0 || x >= width) continue;
					if (visit[y][x]) continue;
					visit[y][x] = true;
					HmapData[y][x] = (abs(y - i) + abs(x - j)) / 2;
				}

			}
		}
	}
}