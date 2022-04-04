#ifndef ARENA_H_INCLUDED
#define ARENA_H_INCLUDED

//wymiary
int a = 10.0f; //podastawa a*a
int h = 16.0f; //wysokoœæ

//Ÿród³a œwiat³la
glm::vec4 lp = glm::vec4(-a/2, 0, -a/2, 1);
glm::vec4 lp1 = glm::vec4(a/2, 0, a/2, 1);

//pod³oga

int floorVertexCount = 6;

float floorVertices[] =
{
	-a / 2, -h / 2, -a / 2, 1.0f,
	-a / 2, -h / 2, a / 2, 1.0f,
	a / 2, -h / 2, -a / 2, 1.0f,

	-a / 2, -h / 2, a / 2, 1.0f,
	a / 2, -h / 2, -a / 2, 1.0f,
	a / 2, -h / 2, a / 2, 1.0f
};

float floorNormals[] =
{
	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,

	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,
	0.0f, 1.0f, 0.0f,0.0f,
};

float floorTexCoords[] =
{
	0.0f,0.0f, 0.0f,1.0f, 1.0f,0.0f,
	0.0f,1.0f, 1.0f,0.0f, 1.0f,1.0f
};

#endif 
