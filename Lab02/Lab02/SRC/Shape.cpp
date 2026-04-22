#include "GameUtil.h"
#include "Shape.h"

using namespace std;

Shape::Shape()
{
}

Shape::Shape(const string& shape_filename)
{
	LoadShape(shape_filename);
}

Shape::~Shape()
{
}

void Shape::Render(void)
{
}

void Shape::LoadShape(const string& shape_filename)
{
}
