/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "arena.h"
#include <time.h>
#include <math.h>

float speed = 0;//[radians/s]

float aspectRatio = 1;

//zmienne opowiedzialne za porzuaniem kamerą
float camera_speed = 0;
float camera_speed_x = 0;

float gravity = -1.0f;

ShaderProgram *sp;

//uchwyty na tekstury
GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;
GLuint tex4;
GLuint tex5;
GLuint tex6;

class klocek {
public:
	float pos_x[30];
	float pos_y[30];
	float pos_z[30];
	int czy_stop = 0;
	float angle = 0.0f;
	float angle_z = 0.0f;
	float angle_y = 0.0f;
	int czy_istnieje = 0;
	int ilosc;
	int rodzaj;    // 1 = I,  2 = T, 3 = cube, 4 = L

	float gravity = -0.02f;

	void zapelnij() {
		for (int i = 0; i < 30; i++) {
			pos_x[i] = 0.0f;
			pos_y[i] = h/2;
			pos_z[i] = 0.0f;
			rodzaj = 1 + (std::rand() % (6));

		}
	}
}klocki[1000];

int spadajacyKlocek = 0;

float odleglosc(float x1, float x2, float y1, float y2, float z1, float z2)
{



	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
}

int kolizja(klocek a, klocek b) {
	using namespace std;
	for (int i = 0; i < a.ilosc;i++) {
		for (int j = 0; j < b.ilosc; j++) {
			//cout<<odleglosc(a.pos_x[i],b.pos_x[j],a.pos_y[i],b.pos_y[j],a.pos_z[i],b.pos_z[j])<<endl;
			if (odleglosc(a.pos_x[i], b.pos_x[j], a.pos_y[i], b.pos_y[j], a.pos_z[i], b.pos_z[j]) < 1) {

				return 1;
			}
		}
	}
	return 0;
}

glm::mat4 drawLeft(glm::mat4 Mt, float angleDelta, int i, int j) {
	//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
	glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
	//...translation, to move the cube to the edge...
	Mk = glm::translate(Mk, glm::vec3(2.0f, 0.0f, 0.0f));
	//...and scaling to reduce the size of the cube
	//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount); //Narysuj obiekt
	klocki[i].pos_x[j] = Mk[3].x;
	klocki[i].pos_y[j] = Mk[3].y;
	klocki[i].pos_z[j] = Mk[3].z;
	return Mk;
}

glm::mat4 drawRight(glm::mat4 Mt, float angleDelta, int i, int j) {
	//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
	glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
	//...translation, to move the cube to the edge...
	Mk = glm::translate(Mk, glm::vec3(-2.0f, 0.0f, 0.0f));
	//...and scaling to reduce the size of the cube
	//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].pos_x[j] = Mk[3].x;
	klocki[i].pos_y[j] = Mk[3].y;
	klocki[i].pos_z[j] = Mk[3].z;
	return Mk;
}

glm::mat4 drawTop(glm::mat4 Mt, float angleDelta, int i, int j) {
	//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
	glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
	//...translation, to move the cube to the edge...
	Mk = glm::translate(Mk, glm::vec3(0.0f, 2.0f, 0.0f));
	//...and scaling to reduce the size of the cube
	//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].pos_x[j] = Mk[3].x;
	klocki[i].pos_y[j] = Mk[3].y;
	klocki[i].pos_z[j] = Mk[3].z;
	return Mk;
}


glm::mat4 drawBottom(glm::mat4 Mt, float angleDelta, int i, int j) {
	//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
	glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
	//...translation, to move the cube to the edge...
	Mk = glm::translate(Mk, glm::vec3(0.0f, -2.0f, 0.0f));
	//...and scaling to reduce the size of the cube
	//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].pos_x[j] = Mk[3].x;
	klocki[i].pos_y[j] = Mk[3].y;
	klocki[i].pos_z[j] = Mk[3].z;
	return Mk;
}

glm::mat4 drawBehind(glm::mat4 Mt, float angleDelta, int i, int j) {
	//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
	glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
	//...translation, to move the cube to the edge...
	Mk = glm::translate(Mk, glm::vec3(0.0f, 0.0f, 2.0f));
	//...and scaling to reduce the size of the cube
	//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].pos_x[j] = Mk[3].x;
	klocki[i].pos_y[j] = Mk[3].y;
	klocki[i].pos_z[j] = Mk[3].z;
	return Mk;

}

glm::mat4 drawInfront(glm::mat4 Mt, float angleDelta, int i, int j) {
	//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
	glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
	//...translation, to move the cube to the edge...
	Mk = glm::translate(Mk, glm::vec3(0.0f, 0.0f, -2.0f));
	//...and scaling to reduce the size of the cube
	//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].pos_x[j] = Mk[3].x;
	klocki[i].pos_y[j] = Mk[3].y;
	klocki[i].pos_z[j] = Mk[3].z;
	return Mk;
}

void drawStraight(float angle, float angleDelta, float gravity, int i) {

	using namespace std;
	glm::mat4 I = glm::mat4(1.0f);
	float grav = gravity;
	int j = 0;
	float pos;
	if (klocki[i].czy_stop == 0) {
		pos = klocki[i].pos_y[0] + klocki[i].gravity;
	}
	else {
		pos = klocki[i].pos_y[0];
	}
	klocki[i].rodzaj = 1;

	int s = 0;

	while (klocki[s].czy_istnieje == 1 && klocki[i].czy_stop == 0) {
		if (s != i) {
			if (kolizja(klocki[i], klocki[s]) == 1) {
				//cout<<s<<" "<<i<<endl
				klocki[i].czy_stop = 1;

			}
		}
		//cout<<s<<endl;
		s++;
	}

	for (int z = 0;z < klocki[i].ilosc;z++) {


		if (klocki[i].pos_y[z] - 0.5 <= -h / 2) {

			klocki[i].czy_stop = 1;
		}
	}

	glm::mat4 Mt1 = glm::translate(I, glm::vec3(klocki[i].pos_x[0], pos, klocki[i].pos_z[0]));

	Mt1 = glm::rotate(Mt1, klocki[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_z, glm::vec3(1.0f, 0.0f, 0.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_y, glm::vec3(0.0f, 1.0f, 0.0f));

	Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 Mk = Mt1;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cubeVertices);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cubeNormals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cubeTexCoords);
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex5);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].ilosc = 5;
	klocki[i].pos_y[j] = pos;
	j++;


	glm::mat4 Mk1 = drawLeft(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk2 = drawRight(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk3 = drawLeft(Mk1, angleDelta, i, j);
	j++;
	glm::mat4 Mk4 = drawRight(Mk2, angleDelta, i, j);
	j++;

}

void drawL(float angle, float angleDelta, float gravity, int i) {
	using namespace std;
	glm::mat4 I = glm::mat4(1.0f);
	float grav = gravity;
	int j = 0;
	float pos;
	if (klocki[i].czy_stop == 0) {
		pos = klocki[i].pos_y[0] + klocki[i].gravity;
	}
	else {
		pos = klocki[i].pos_y[0];
	}
	klocki[i].rodzaj = 4;
	klocki[i].ilosc = 5;
	for (int z = 0;z < klocki[i].ilosc;z++) {


		if (klocki[i].pos_y[z] - 0.5f <= -h/2) {

			klocki[i].czy_stop = 1;
		}
	}

	int s = 0;

	while (klocki[s].czy_istnieje == 1 && klocki[i].czy_stop == 0) {
		if (s != i) {
			if (kolizja(klocki[i], klocki[s]) == 1) {

				klocki[i].czy_stop = 1;

			}
		}
		//cout<<s<<endl;
		s++;
	}
	glm::mat4 Mt1 = glm::translate(I, glm::vec3(klocki[i].pos_x[0], pos, klocki[i].pos_z[0]));

	Mt1 = glm::rotate(Mt1, klocki[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_z, glm::vec3(1.0f, 0.0f, 0.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_y, glm::vec3(0.0f, 1.0f, 0.0f));

	Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 Mk = Mt1;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cubeVertices);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cubeNormals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cubeTexCoords);
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex3);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);

	klocki[i].pos_y[j] = pos;
	j++;
	glm::mat4 Mk1 = drawLeft(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk2 = drawLeft(Mk1, angleDelta, i, j);
	j++;
	glm::mat4 Mk3 = drawTop(Mk2, angleDelta, i, j);
	j++;
	glm::mat4 Mk4 = drawTop(Mk3, angleDelta, i, j);
	j++;

}

void drawT(float angle, float angleDelta, float gravity, int i) {
	using namespace std;
	glm::mat4 I = glm::mat4(1.0f);
	float grav = gravity;
	int j = 0;
	float pos;
	if (klocki[i].czy_stop == 0) {
		pos = klocki[i].pos_y[0] + klocki[i].gravity;
	}
	else {
		pos = klocki[i].pos_y[0];
	}
	klocki[i].rodzaj = 2;
	klocki[i].ilosc = 6;
	for (int z = 0;z < klocki[i].ilosc;z++) {


		if (klocki[i].pos_y[z] - 0.5f <= -h / 2) {

			klocki[i].czy_stop = 1;
		}
	}
	int s = 0;

	while (klocki[s].czy_istnieje == 1 && klocki[i].czy_stop == 0) {
		if (s != i) {
			if (kolizja(klocki[i], klocki[s]) == 1) {

				klocki[i].czy_stop = 1;

			}
		}
		//cout<<s<<endl;
		s++;
	}


	glm::mat4 Mt1 = glm::translate(I, glm::vec3(klocki[i].pos_x[0], pos, klocki[i].pos_z[0]));

	Mt1 = glm::rotate(Mt1, klocki[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_z, glm::vec3(1.0f, 0.0f, 0.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_y, glm::vec3(0.0f, 1.0f, 0.0f));

	Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));


	glm::mat4 Mk = Mt1;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cubeVertices);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cubeNormals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cubeTexCoords);
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex4);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);

	klocki[i].pos_y[j] = pos;
	j++;
	glm::mat4 Mk1 = drawLeft(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk2 = drawTop(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk3 = drawRight(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk4 = drawBehind(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk5 = drawInfront(Mk, angleDelta, i, j);

	//cout << Mk2[3].x << " " << Mk2[3].y<<" "<<Mk2[3].z<<endl; //pozycja y


}

void drawKostka(float angle, float angleDelta, float gravity, int i) {

	using namespace std;
	glm::mat4 I = glm::mat4(1.0f);
	float grav = gravity;
	int j = 0;
	float pos;
	if (klocki[i].czy_stop == 0) {
		pos = klocki[i].pos_y[0] + klocki[i].gravity;
	}
	else {
		pos = klocki[i].pos_y[0];
	}
	klocki[i].rodzaj = 3;
	for (int z = 0;z < klocki[i].ilosc;z++) {


		if (klocki[i].pos_y[z] - 0.5f <= -h / 2) {

			klocki[i].czy_stop = 1;
		}
	}
	int s = 0;

	while (klocki[s].czy_istnieje == 1 && klocki[i].czy_stop == 0) {
		if (s != i) {
			if (kolizja(klocki[i], klocki[s]) == 1) {

				klocki[i].czy_stop = 1;

			}
		}
		//cout<<s<<endl;
		s++;
	}
	glm::mat4 Mt1 = glm::translate(I, glm::vec3(klocki[i].pos_x[0], pos, klocki[i].pos_z[0]));

	Mt1 = glm::rotate(Mt1, klocki[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_z, glm::vec3(1.0f, 0.0f, 0.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_y, glm::vec3(0.0f, 1.0f, 0.0f));

	Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 Mk = Mt1;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cubeVertices);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cubeNormals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cubeTexCoords);
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex2);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].ilosc = 27;
	klocki[i].pos_y[j] = pos;
	j++;

	glm::mat4 Mk1 = drawLeft(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk2 = drawRight(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk3 = drawTop(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk4 = drawLeft(Mk3, angleDelta, i, j);
	j++;
	glm::mat4 Mk5 = drawRight(Mk3, angleDelta, i, j);
	j++;
	glm::mat4 Mk6 = drawBottom(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk7 = drawLeft(Mk6, angleDelta, i, j);
	j++;
	glm::mat4 Mk8 = drawRight(Mk6, angleDelta, i, j);
	j++;
	glm::mat4 Mk9 = drawBehind(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk10 = drawTop(Mk9, angleDelta, i, j);
	j++;
	glm::mat4 Mk11 = drawLeft(Mk9, angleDelta, i, j);
	j++;
	glm::mat4 Mk12 = drawRight(Mk9, angleDelta, i, j);
	j++;
	glm::mat4 Mk13 = drawLeft(Mk10, angleDelta, i, j);
	j++;
	glm::mat4 Mk14 = drawRight(Mk10, angleDelta, i, j);
	j++;
	glm::mat4 Mk15 = drawBottom(Mk9, angleDelta, i, j);
	j++;
	glm::mat4 Mk16 = drawLeft(Mk15, angleDelta, i, j);
	j++;
	glm::mat4 Mk17 = drawRight(Mk15, angleDelta, i, j);
	j++;
	glm::mat4 Mk18 = drawInfront(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk19 = drawTop(Mk18, angleDelta, i, j);
	j++;
	glm::mat4 Mk20 = drawBottom(Mk18, angleDelta, i, j);
	j++;
	glm::mat4 Mk21 = drawLeft(Mk19, angleDelta, i, j);
	j++;
	glm::mat4 Mk22 = drawRight(Mk19, angleDelta, i, j);
	j++;
	glm::mat4 Mk23 = drawLeft(Mk20, angleDelta, i, j);
	j++;
	glm::mat4 Mk24 = drawRight(Mk20, angleDelta, i, j);
	j++;
	glm::mat4 Mk25 = drawRight(Mk18, angleDelta, i, j);
	j++;
	glm::mat4 Mk26 = drawLeft(Mk18, angleDelta, i, j);
	j++;

}

void drawCos(float angle, float angleDelta, float gravity, int i) {

	using namespace std;
	glm::mat4 I = glm::mat4(1.0f);
	float grav = gravity;
	int j = 0;
	float pos;
	if (klocki[i].czy_stop == 0) {
		pos = klocki[i].pos_y[0] + klocki[i].gravity;
	}
	else {
		pos = klocki[i].pos_y[0];
	}
	klocki[i].rodzaj = 5;
	for (int z = 0; z < klocki[i].ilosc; z++) {


		if (klocki[i].pos_y[z] - 0.5f <= -h / 2) {

			klocki[i].czy_stop = 1;
		}
	}
	int s = 0;

	while (klocki[s].czy_istnieje == 1 && klocki[i].czy_stop == 0) {
		if (s != i) {
			if (kolizja(klocki[i], klocki[s]) == 1) {

				klocki[i].czy_stop = 1;

			}
		}
		//cout<<s<<endl;
		s++;
	}
	glm::mat4 Mt1 = glm::translate(I, glm::vec3(klocki[i].pos_x[0], pos, klocki[i].pos_z[0]));

	Mt1 = glm::rotate(Mt1, klocki[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_z, glm::vec3(1.0f, 0.0f, 0.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_y, glm::vec3(0.0f, 1.0f, 0.0f));

	Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 Mk = Mt1;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cubeVertices);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cubeNormals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cubeTexCoords);
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex6);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].ilosc = 4;
	klocki[i].pos_y[j] = pos;
	j++;
	glm::mat4 Mk1 = drawLeft(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk2 = drawBottom(Mk1, angleDelta, i, j);
	j++;
	glm::mat4 Mk3 = drawTop(Mk, angleDelta, i, j);
	j++;

}


void drawCosReversed(float angle, float angleDelta, float gravity, int i) {

	using namespace std;
	glm::mat4 I = glm::mat4(1.0f);
	float grav = gravity;
	int j = 0;
	float pos;
	if (klocki[i].czy_stop == 0) {
		pos = klocki[i].pos_y[0] + klocki[i].gravity;
	}
	else {
		pos = klocki[i].pos_y[0];
	}
	klocki[i].rodzaj = 6;
	for (int z = 0; z < klocki[i].ilosc; z++) {


		if (klocki[i].pos_y[z] - 0.5f <= -h / 2) {

			klocki[i].czy_stop = 1;
		}
	}
	int s = 0;

	while (klocki[s].czy_istnieje == 1 && klocki[i].czy_stop == 0) {
		if (s != i) {
			if (kolizja(klocki[i], klocki[s]) == 1) {

				klocki[i].czy_stop = 1;

			}
		}
		//cout<<s<<endl;
		s++;
	}
	glm::mat4 Mt1 = glm::translate(I, glm::vec3(klocki[i].pos_x[0], pos, klocki[i].pos_z[0]));

	Mt1 = glm::rotate(Mt1, klocki[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
	Mt1 = rotate(Mt1, klocki[i].angle_z, glm::vec3(1.0f, 0.0f, 0.0f));

	Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));

	glm::mat4 Mk = Mt1;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(Mk));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cubeVertices);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cubeNormals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, cubeTexCoords);
	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex6);
	glDrawArrays(GL_TRIANGLES, 0, cubeVertexCount);
	klocki[i].ilosc = 4;
	klocki[i].pos_y[j] = pos;
	j++;
	glm::mat4 Mk1 = drawLeft(Mk, angleDelta, i, j);
	j++;
	glm::mat4 Mk2 = drawTop(Mk1, angleDelta, i, j);
	j++;
	glm::mat4 Mk3 = drawBottom(Mk, angleDelta, i, j);
	j++;

}

//wyliczanie kiedy kamera jest pod podstawą
float floorCamAngle(glm::mat4 M, glm::mat4 V)
{
	glm::vec4 x = V*M*(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) - glm::vec4(0.0f, -h/2, 0.0f,  1.0f));
	glm::vec4 y = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) - V * M * glm::vec4(0.0f, -h / 2, 0.0f, 1.0f);
	glm::vec4 z = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) - V * M * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	float X = sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
	float Y = sqrt(y.x * y.x + y.y * y.y + y.z * y.z);
	float Z = sqrt(z.x * z.x + z.y * z.y + z.z * z.z);

	float cosinus = (X * X + Y * Y - (Z * Z)) / 2 * X * Y;

	return cosinus;
}

int granica_j(klocek klocek)
{
	int k = 0;
	for (int i = 0; i < klocek.ilosc; i++)
	{
		if (klocek.pos_x[i] >= a / 2 - 0.5f) k++;
	}
	return k;
}

int granica_l(klocek klocek)
{
	int k = 0;
	for (int i = 0; i < klocek.ilosc; i++)
	{
		if (klocek.pos_x[i] <= -a/2 + 0.5f) k++;
	}
	return k;
}

int granica_i(klocek klocek)
{
	int k = 0;
	for (int i = 0; i < klocek.ilosc; i++)
	{
		if (klocek.pos_z[i] >= a / 2 - 0.5f) k++;
	}
	return k;
}

int granica_k(klocek klocek)
{
	int k = 0;
	for (int i = 0; i < klocek.ilosc; i++)
	{
		if (klocek.pos_z[i] <= - a / 2 + 0.5f) k++;
	}
	return k;
}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) {

			if (granica_j(klocki[spadajacyKlocek]) == 0) {

				klocek kloc_temp = klocki[spadajacyKlocek];
				for (int k = 0; k<kloc_temp.ilosc;k++){
					kloc_temp.pos_x[k]+=1.0f;
				}
				int j = 0;

				while (klocki[j].czy_stop == 1){

					if (kolizja(kloc_temp,klocki[j]) == 1){

						//std::cout<<j<<std::endl;
						for (int k = 0; k<kloc_temp.ilosc;k++){
							kloc_temp.pos_x[k]-=1.0f;
						}

					}
					j++;
				}
				klocki[spadajacyKlocek] = kloc_temp;

			}


		}
		if (key == GLFW_KEY_RIGHT) {

			if (granica_l(klocki[spadajacyKlocek]) == 0) {
				klocek kloc_temp = klocki[spadajacyKlocek];
				for (int k = 0; k<kloc_temp.ilosc;k++){
					kloc_temp.pos_x[k]-=1.0f;
				}
				int j = 0;

				while (klocki[j].czy_stop == 1){

					if (kolizja(kloc_temp,klocki[j]) == 1){

						//std::cout<<j<<std::endl;
						for (int k = 0; k<kloc_temp.ilosc;k++){
							kloc_temp.pos_x[k]+=1.0f;
						}

					}
					j++;
				}
				klocki[spadajacyKlocek] = kloc_temp;
			}

		}
		if (key == GLFW_KEY_UP) {
			if (granica_i(klocki[spadajacyKlocek]) == 0) {
				klocek kloc_temp = klocki[spadajacyKlocek];
				for (int k = 0; k<kloc_temp.ilosc;k++){
					kloc_temp.pos_z[k]+=1.0f;
				}
				int j = 0;

				while (klocki[j].czy_stop == 1){

					if (kolizja(kloc_temp,klocki[j]) == 1){

						//std::cout<<j<<std::endl;
						for (int k = 0; k<kloc_temp.ilosc;k++){
							kloc_temp.pos_z[k]-=1.0f;
						}

					}
					j++;
				}
				klocki[spadajacyKlocek] = kloc_temp;


		}

	}
		if (key == GLFW_KEY_DOWN) {

			if (granica_k(klocki[spadajacyKlocek]) == 0) {
				klocek kloc_temp = klocki[spadajacyKlocek];
				for (int k = 0; k<kloc_temp.ilosc;k++){
					kloc_temp.pos_z[k]-=1.0f;
				}
				int j = 0;

				while (klocki[j].czy_stop == 1){

					if (kolizja(kloc_temp,klocki[j]) == 1){

						//std::cout<<j<<std::endl;
						for (int k = 0; k<kloc_temp.ilosc;k++){
							kloc_temp.pos_z[k]+=1.0f;
						}

					}
					j++;
				}
				klocki[spadajacyKlocek] = kloc_temp;
			}

		}
		if (key == GLFW_KEY_A) camera_speed = PI / 2;
		if (key == GLFW_KEY_D) camera_speed = -PI / 2;
		if (key == GLFW_KEY_W) camera_speed_x = PI / 2;
		if (key == GLFW_KEY_S) camera_speed_x = -PI / 2;

		if (key == GLFW_KEY_J) {
			klocki[spadajacyKlocek].angle -= PI / 2;
		}
		if (key == GLFW_KEY_L) {
			klocki[spadajacyKlocek].angle += PI / 2;

		}
		if (key == GLFW_KEY_K) {
			klocki[spadajacyKlocek].angle_z -= PI / 2;

		}
		if (key == GLFW_KEY_I) {
			klocki[spadajacyKlocek].angle_z += PI / 2;
		}

		if (key == GLFW_KEY_U) {
			klocki[spadajacyKlocek].angle_y += PI / 2;
		}
		if (key == GLFW_KEY_O) {
			klocki[spadajacyKlocek].angle_y += PI / 2;
		}
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) camera_speed = 0;
		if (key == GLFW_KEY_W || key == GLFW_KEY_S) camera_speed_x = 0;
	}
}

void windowResizeCallback(GLFWwindow* window,int width,int height) {
    if (height==0) return;
    aspectRatio=(float)width/(float)height;
    glViewport(0,0,width,height);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0.5,0.5,0.5,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window,windowResizeCallback);
	glfwSetKeyCallback(window,keyCallback);

	sp=new ShaderProgram("v_simplest.glsl",NULL,"f_simplest.glsl");
	//tex0 = readTexture("tex0.png");
	tex1 = readTexture("tex1.png");
	tex2 = readTexture("tex2.png");
	tex3 = readTexture("tex3.png");
	tex4 = readTexture("tex4.png");
	tex5 = readTexture("tex5.png");
	tex6 = readTexture("tex6.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

    delete sp;
}




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle, float camera_angle, float camera_angle_x, float gravity) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix
	//glm::vec3 x_axis = (V * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)).xyz;
//	glm::vec3 y_axis = (V * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)).xyz;
	glm::vec3 x_axis = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 y_axis = glm::vec3(0.0f, 1.0f, 0.0f);
	V = glm::rotate(V, camera_angle_x, x_axis);
	V = glm::rotate(V, camera_angle, y_axis);

    glm::mat4 P=glm::perspective(50.0f*PI/180.0f, aspectRatio, 0.01f, 50.0f); //Wylicz macierz rzutowania

    glm::mat4 M=glm::mat4(1.0f);

    sp->use();//Aktywacja programu cieniującego
    //Przeslij parametry programu cieniującego do karty graficznej
    glUniformMatrix4fv(sp->u("P"),1,false,glm::value_ptr(P));
    glUniformMatrix4fv(sp->u("V"),1,false,glm::value_ptr(V));
    glUniformMatrix4fv(sp->u("M"),1,false,glm::value_ptr(M));
	glUniform4fv(sp->u("lp"), 1, glm::value_ptr(lp));
	glUniform4fv(sp->u("lp1"), 1, glm::value_ptr(lp1));

    glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glEnableVertexAttribArray(sp->a("texCoord0"));

	//rysowanie podłogi
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, floorVertices);
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, floorNormals);
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, floorTexCoords);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	if (floorCamAngle(M, V) >= 0) {
		glDrawArrays(GL_TRIANGLES, 0, floorVertexCount); //Narysuj obiekt
	}
	//glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	int i = 0;
	while (klocki[i].czy_stop == 1) {
		if (klocki[i].rodzaj == 2) drawT(angle, 0, gravity, i);
		if (klocki[i].rodzaj == 4) drawL(angle, 0, gravity, i);
		if (klocki[i].rodzaj == 1) drawStraight(angle, 0, gravity, i);
		if (klocki[i].rodzaj == 3) drawKostka(angle, 0, gravity, i);
		if (klocki[i].rodzaj == 5) drawCos(angle, 0, gravity, i);
		if (klocki[i].rodzaj == 6) drawCosReversed(angle, 0, gravity, i);
		klocki[i].czy_istnieje = 1;
		//std::cout<<klocki[i].rodzaj <<std::endl;
		i++;
	}
	//std::cout << klocki[i].rodzaj << std::endl;
	if (klocki[i].rodzaj == 2 && klocki[i].czy_stop == 0) drawT(angle, 0, gravity, i);
	if (klocki[i].rodzaj == 4 && klocki[i].czy_stop == 0) drawL(angle, 0, gravity, i);
	if (klocki[i].rodzaj == 1 && klocki[i].czy_stop == 0) drawStraight(angle, 0, gravity, i);
	if (klocki[i].rodzaj == 3 && klocki[i].czy_stop == 0) drawKostka(angle, 0, gravity, i);
	if (klocki[i].rodzaj == 5) drawCos(angle, 0, gravity, i);
	if (klocki[i].rodzaj == 6) drawCosReversed(angle, 0, gravity, i);
	klocki[i].czy_istnieje = 1;

    glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	srand(time(NULL));
	for (int i = 0; i < 1000; i++) {
		klocki[i].zapelnij();
	}

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle = 0; //declare variable for storing current rotation angle
	float camera_angle = 0;
	float camera_angle_y = 0;
	float gravitation = 0;
	glfwSetTime(0); //clear internal timer
	while (!glfwWindowShouldClose(window)) //As long as the window shouldnt be closed yet...
	{
		angle = speed; // //Compute an angle by which the object was rotated during the previous frame
		camera_angle += camera_speed * glfwGetTime(); //Compute an angle by which the object was rotated during the previous frame
		camera_angle_y += camera_speed_x * glfwGetTime();
		gravitation += gravity * glfwGetTime();
		if (klocki[spadajacyKlocek].czy_stop == 1)
		{
			spadajacyKlocek++;
		}
		glfwSetTime(0); //clear internal timer
		drawScene(window, angle, camera_angle, camera_angle_y, gravitation); //Execute drawing procedure
		glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
