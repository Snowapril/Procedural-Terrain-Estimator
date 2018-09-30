#include "Estimator.hpp"
#include <cstdio>
using namespace std;

int main() {
	freopen("Mapdata.pte", "r", stdin);
	int width, height;
	scanf("%d %d", &height, &width);
	printf("%d %d\n", height, width);
	vector < unsigned char > M(height*width, 0);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			scanf("%d", &M[i*width + j]);
		}
	}

	Estimator E(M,height,width);

	E.makeCoast(true);
	freopen("outHeight.txt", "w", stdout);

	E.dumpMapData();

	return 0;
}

