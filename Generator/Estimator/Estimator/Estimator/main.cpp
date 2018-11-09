#include "Estimator.hpp"
#include <cstdio>
using namespace std;

int main() {
	freopen("outHeight.txt", "r", stdin);
	int width, height;
	scanf("%d %d", &height, &width);
	vector < unsigned char > M(height*width, 0);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			scanf("%d", &M[i*width + j]);
		}
	}

	Estimator E(M,height,width);

	freopen("outHeight2.txt", "w", stdout);
	E.dumpHeightMapData();

	E.blendmapColoring();

	freopen("outDescent.txt", "w", stdout);
	E.dumpDescentMapData();

	freopen("outBlend.txt", "w", stdout);
	E.dumpBlendMapData();

	return 0;
}

