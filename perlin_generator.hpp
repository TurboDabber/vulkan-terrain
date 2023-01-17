#pragma once
#include <vector>
// JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE - COPYRIGHT 2002 KEN PERLIN. 
//THIS *java* reference implementation was reimplemented in c++ 2k22 
namespace app {
	class PerlinGenerator {
		// The permutation vector
		std::vector<int> p;
	public:
		PerlinGenerator();
		PerlinGenerator(unsigned int seed);
		double GetNoiseValue(double x, double y, double z);
	private:
		double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); };
		double lerp(double t, double a, double b) { return a + t * (b - a); };
		double grad(int hash, double x, double y, double z) {
			int h = hash & 15;
			double u = h < 8 ? x : y,
				v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		};
	};
}