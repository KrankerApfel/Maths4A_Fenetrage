#pragma once

#include <glm/glm.hpp>
#include "Point.h"
#include "../rendering/Mesh.h"
#include <vector>
#include "../../../stdafx.h"
#include <map>
#include "../../Maillion.h"

using namespace std;
namespace PaintSlayer {


	class Polygon {

	public:
		Polygon(Shader& s);
		Polygon(Shader& s, vector<Point>& );
		std::array<float, 4> getColor() const;
		void setColor(std::array<float, 4>);
		vector<Point> getPoints() const;
		void addPoint(double x, double y);
		void removePoint();
		void draw();
		Shader& getShader();
		void clear();
		void terminate();
		bool isInside(Point point, vector<Point> polygone);
		vector<Point>polyRectangle();
		void fill(std::map<double, Maillion> SI);
		void drawPoint(float x, float y);
		void drawLine(float x1, float y1, float x2, float y2);
	private:
		vector<Point> points;
		Mesh mesh;
		Shader shader;
		std::array<float, 4> color;

	};
}