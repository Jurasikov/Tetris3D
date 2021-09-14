	/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	*/


	#define GLM_FORCE_RADIANS

	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
	#include <glm/glm.hpp>
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <stdlib.h>
	#include <stdio.h>
	#include "constants.h"
	#include "allmodels.h"
	#include "lodepng.h"
	#include "shaderprogram.h"
	#include <iostream>
	#include <time.h>
	#include <math.h>
	float speed = 0;//[radians/s]

	float camera_speed = 0;
	float camera_speed_x = 0;

	float gravity = -1.0f;




	//Error processing callback procedure
	void error_callback(int error, const char* description) {
		fputs(description, stderr);
	}



	class klocek {
	public:
		float pos_x[30];
		float pos_y[30];
		float pos_z[30];
		int czy_stop = 0;
		float angle = 0.0f;
		float angle_z = 0.0f;
		int czy_istnieje = 0;
		int ilosc;
		int rodzaj;    // 1 = I,  2 = T, 3 = cube, 4 = L

		float gravity = -0.02f;

		void zapelnij(){
			for (int i = 0; i < 30; i++) {
				pos_x[i] = 0.0f;
				pos_y[i] = 6.0f;
				pos_z[i] = 0.0f;
				rodzaj = 1 + (std::rand()%(4));

			}
		}
	}klocki[1000];

float odleglosc(float x1, float x2, float y1, float y2, float z1, float z2){



	return sqrt(pow(x1-x2,2)+pow(y1-y2,2)+pow(z1-z2,2));
}

	//Procedura obs�ugi klawiatury
	void key_callback(GLFWwindow* window, int key,
		int scancode, int action, int mods) {

		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_LEFT)
			{
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].angle -= PI/2;
					}
				}
			}
			if (key == GLFW_KEY_RIGHT) {
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].angle += PI/2;
					}
				}
			}

			if (key == GLFW_KEY_UP) {
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].angle_z += PI / 2;
					}
				}
			}
			if (key == GLFW_KEY_DOWN) {
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].angle_z -= PI / 2;
					}
				}
			}

			if (key == GLFW_KEY_A) camera_speed = PI / 2;
			if (key == GLFW_KEY_D) camera_speed = -PI / 2;
			if (key == GLFW_KEY_W) camera_speed_x = PI / 2;
			if (key == GLFW_KEY_S) camera_speed_x = -PI / 2;
			if (key == GLFW_KEY_J) {
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].pos_x[0] += 1.0f;
					}
				}
			}
			if (key == GLFW_KEY_L) {
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].pos_x[0] -= 1.0f;
					}
				}
			}
			if (key == GLFW_KEY_K) {
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].pos_z[0] -= 1.0f;
					}
				}
			}
			if (key == GLFW_KEY_I) {
				for (int i = 0; i < 1000; i++) {
					if (klocki[i].czy_stop == 0) {
						klocki[i].pos_z[0] += 1.0f;
					}
				}
			}

		}

		if (action == GLFW_RELEASE) {
			if (key == GLFW_KEY_A || key == GLFW_KEY_D) camera_speed = 0;
			if (key == GLFW_KEY_W || key == GLFW_KEY_S) camera_speed_x = 0;
		}
	}




	//Initialization code procedure
	void initOpenGLProgram(GLFWwindow* window) {
		initShaders();
		//************Place any code here that needs to be executed once, at the program start************
		glClearColor(0, 0, 0, 1); //Set color buffer clear color
		glEnable(GL_DEPTH_TEST); //Turn on pixel depth test based on depth buffer
		glfwSetKeyCallback(window, key_callback);
	}

	//Release resources allocated by the program
	void freeOpenGLProgram(GLFWwindow* window) {
		freeShaders();
		//************Place any code here that needs to be executed once, after the main loop ends************
	}

	void drawGear(glm::mat4 Mt, float angleDelta) {
		//Load model matrix and draw the torus
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mt));
		glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		Models::cube.drawSolid();
	}

	glm::mat4 drawLeft(glm::mat4 Mt, float angleDelta,int k, int i, int j) {
			//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
		glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
		//...translation, to move the cube to the edge...
		Mk = glm::translate(Mk, glm::vec3(2.0f, 0.0f, 0.0f));
		//...and scaling to reduce the size of the cube
		//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();
		klocki[i].pos_x[j] = Mk[3].x;
		klocki[i].pos_y[j] = Mk[3].y;
		klocki[i].pos_z[j] = Mk[3].z;
		return Mk;
	}

	glm::mat4 drawRight(glm::mat4 Mt, float angleDelta, int k, int i, int j) {
		//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
		glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
		//...translation, to move the cube to the edge...
		Mk = glm::translate(Mk, glm::vec3(-2.0f, 0.0f, 0.0f));
		//...and scaling to reduce the size of the cube
		//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();
		klocki[i].pos_x[j] = Mk[3].x;
		klocki[i].pos_y[j] = Mk[3].y;
		klocki[i].pos_z[j] = Mk[3].z;
		return Mk;
	}




	int kolor(std::string k) {
		if (k == "z") return 0;
		if (k == "n") return 1;
		if (k == "f") return 2;
		if (k == "p") return 3;
		if (k == "cz") return 4;
	}


int kolizja(klocek a, klocek b){
	using namespace std;
	for (int i = 0; i<a.ilosc;i++){
		for (int j = 0; j<b.ilosc; j++){
			//cout<<odleglosc(a.pos_x[i],b.pos_x[j],a.pos_y[i],b.pos_y[j],a.pos_z[i],b.pos_z[j])<<endl;
			if (odleglosc(a.pos_x[i],b.pos_x[j],a.pos_y[i],b.pos_y[j],a.pos_z[i],b.pos_z[j]) < 1){

				cout<<"KOLIZJA!"<<endl<<endl<<endl;
				cout<<i<<" "<<j<<endl;
				cout<<(odleglosc(a.pos_x[i],b.pos_x[j],a.pos_y[i],b.pos_y[j],a.pos_z[i],b.pos_z[j]))<<endl;
				return 1;
			}
		}
	}
	return 0;
}





	glm::mat4 drawTop(glm::mat4 Mt, float angleDelta, int k,int i, int j) {



		//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
		glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
		//...translation, to move the cube to the edge...
		Mk = glm::translate(Mk, glm::vec3(0.0f, 2.0f, 0.0f));
		//...and scaling to reduce the size of the cube
		//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();
		klocki[i].pos_x[j] = Mk[3].x;
		klocki[i].pos_y[j] = Mk[3].y;
		klocki[i].pos_z[j] = Mk[3].z;
		return Mk;
	}


	glm::mat4 drawBottom(glm::mat4 Mt, float angleDelta, int k,int i, int j) {



		//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
		glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
		//...translation, to move the cube to the edge...
		Mk = glm::translate(Mk, glm::vec3(0.0f, -2.0f, 0.0f));
		//...and scaling to reduce the size of the cube
		//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();
		klocki[i].pos_x[j] = Mk[3].x;
		klocki[i].pos_y[j] = Mk[3].y;
		klocki[i].pos_z[j] = Mk[3].z;
		return Mk;
	}

	glm::mat4 drawBehind(glm::mat4 Mt, float angleDelta, int k,int i, int j) {

		//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
		glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
		//...translation, to move the cube to the edge...
		Mk = glm::translate(Mk, glm::vec3(0.0f, 0.0f, 2.0f));
		//...and scaling to reduce the size of the cube
		//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();
		klocki[i].pos_x[j] = Mk[3].x;
		klocki[i].pos_y[j] = Mk[3].y;
		klocki[i].pos_z[j] = Mk[3].z;
		return Mk;

	}

	glm::mat4 drawInfront(glm::mat4 Mt, float angleDelta, int k,int i,int j) {
		//Cube model matrix is composed of: rotation, to choose place of the cube on the edge of the torus
		glm::mat4 Mk = glm::rotate(Mt, glm::radians(angleDelta), glm::vec3(0.0f, 0.0f, 1.0f));
		//...translation, to move the cube to the edge...
		Mk = glm::translate(Mk, glm::vec3(0.0f, 0.0f, -2.0f));
		//...and scaling to reduce the size of the cube
		//Mk = glm::scale(Mk, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));

		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);

		klocki[i].pos_x[j] = Mk[3].x;
		klocki[i].pos_y[j] = Mk[3].y;
		klocki[i].pos_z[j] = Mk[3].z;
		Models::cube.drawSolid();
		return Mk;
	}


	void drawStraight(float angle, float angleDelta, int k, float gravity, float position, int i) {

		using namespace std;
		glm::mat4 I = glm::mat4(1.0f);
		float grav = gravity;
		int j = 0;
		float pos;
		if (klocki[i].czy_stop == 0){
		pos = klocki[i].pos_y[0] + klocki[i].gravity;}
		else{
			pos = klocki[i].pos_y[0];
		}
		klocki[i].rodzaj =1;

		int s=0;

		while (klocki[s].czy_istnieje==1 && klocki[i].czy_stop==0){
			if (s!=i){
			if (kolizja(klocki[i],klocki[s])==1){
				//cout<<s<<" "<<i<<endl;
				for (int m = 0; m<6; m++){
					cout<<klocki[i].pos_x[m]<<" "<<klocki[i].pos_y[m]<<" "<<klocki[i].pos_z[m]<<endl;
				}
				cout<<endl;
				for (int m = 0; m<klocki[i].ilosc; m++){
					cout<<klocki[s].pos_x[m]<<" "<<klocki[s].pos_y[m]<<" "<<klocki[s].pos_z[m]<<endl;
				}
				cout<<"KOLIZJA2!"<<endl<<endl<<endl;
				klocki[i].czy_stop = 1;

			}
		}
		//cout<<s<<endl;
		s++;
	}

	for (int z=0 ;z<klocki[i].ilosc;z++){


		if (klocki[i].pos_y[z] <= -4.0f) {

		klocki[i].czy_stop = 1;
	}
	}

		glm::mat4 Mt1 = glm::translate(I, glm::vec3(klocki[i].pos_x[0], pos, klocki[i].pos_z[0]));

		Mt1 = glm::rotate(Mt1, klocki[i].angle, glm::vec3(0.0f, 0.0f, 1.0f));
		Mt1 = rotate(Mt1, klocki[i].angle_z, glm::vec3(1.0f, 0.0f, 0.0f));

		Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));

		glm::mat4 Mk = Mt1;
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();
		klocki[i].ilosc = 5;
		klocki[i].pos_y[j] = pos;
		j++;


		glm::mat4 Mk1 = drawLeft(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk2 = drawRight(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk3 = drawLeft(Mk1, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk4 = drawRight(Mk2, angleDelta, k,i,j);
		j++;

	}


	void drawL(float angle, float angleDelta, int k, float gravity, float position,int i) {
		using namespace std;
		glm::mat4 I = glm::mat4(1.0f);
		float grav = gravity;
		int j = 0;
		float pos;
		if (klocki[i].czy_stop == 0){
		pos = klocki[i].pos_y[0] + klocki[i].gravity;}
		else{
			pos = klocki[i].pos_y[0];
		}
				klocki[i].rodzaj = 4;
		klocki[i].ilosc = 5;
		for (int z=0 ;z<klocki[i].ilosc;z++){


			if (klocki[i].pos_y[z] <= -4.0f) {

			klocki[i].czy_stop = 1;
		}
		}

		int s=0;

		while (klocki[s].czy_istnieje==1 && klocki[i].czy_stop==0){
			if (s!=i){
			if (kolizja(klocki[i],klocki[s])==1){
				//cout<<s<<" "<<i<<endl;
				for (int m = 0; m<6; m++){
					cout<<klocki[i].pos_x[m]<<" "<<klocki[i].pos_y[m]<<" "<<klocki[i].pos_z[m]<<endl;
				}
				cout<<endl;
				for (int m = 0; m<klocki[i].ilosc; m++){
					cout<<klocki[s].pos_x[m]<<" "<<klocki[s].pos_y[m]<<" "<<klocki[s].pos_z[m]<<endl;
				}
				cout<<"KOLIZJA2!"<<endl<<endl<<endl;
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
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();

		klocki[i].pos_y[j] = pos;
		j++;
		glm::mat4 Mk1 = drawLeft(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk2 = drawLeft(Mk1, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk3 = drawTop(Mk2, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk4 = drawTop(Mk3, angleDelta,k,i,j);
		j++;

	}

	void drawT(float angle, float angleDelta, int k, float gravity, float position,int i) {
		using namespace std;
		glm::mat4 I = glm::mat4(1.0f);
		float grav = gravity;
		int j = 0;
		float pos;
		if (klocki[i].czy_stop == 0){
		pos = klocki[i].pos_y[0] + klocki[i].gravity;}
		else{
			pos = klocki[i].pos_y[0];
		}
		klocki[i].rodzaj = 2;
		klocki[i].ilosc = 6;
		for (int z=0 ;z<klocki[i].ilosc;z++){


			if (klocki[i].pos_y[z] <= -4.0f) {

			klocki[i].czy_stop = 1;
		}
		}
		int s=0;

		while (klocki[s].czy_istnieje==1 && klocki[i].czy_stop==0){
			if (s!=i){
			if (kolizja(klocki[i],klocki[s])==1){
				//cout<<s<<" "<<i<<endl;
				for (int m = 0; m<6; m++){
					cout<<klocki[i].pos_x[m]<<" "<<klocki[i].pos_y[m]<<" "<<klocki[i].pos_z[m]<<endl;
				}
				cout<<endl;
				for (int m = 0; m<6; m++){
					cout<<klocki[s].pos_x[m]<<" "<<klocki[s].pos_y[m]<<" "<<klocki[s].pos_z[m]<<endl;
				}
				cout<<"KOLIZJA2!"<<endl<<endl<<endl;
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
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();

		klocki[i].pos_y[j] = pos;
		j++;
		glm::mat4 Mk1 = drawLeft(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk2 = drawTop(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk3 = drawRight(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk4 = drawBehind(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk5 = drawInfront(Mk, angleDelta, k,i,j);

		//cout << Mk2[3].x << " " << Mk2[3].y<<" "<<Mk2[3].z<<endl; //pozycja y


	}

	void drawKostka(float angle, float angleDelta, int k, float gravity, float position, int i) {

		using namespace std;
		glm::mat4 I = glm::mat4(1.0f);
		float grav = gravity;
		int j = 0;
		float pos;
		if (klocki[i].czy_stop == 0){
		pos = klocki[i].pos_y[0] + klocki[i].gravity;}
		else{
			pos = klocki[i].pos_y[0];
		}
		klocki[i].rodzaj = 3;
		for (int z=0 ;z<klocki[i].ilosc;z++){


			if (klocki[i].pos_y[z] <= -4.0f) {

			klocki[i].czy_stop = 1;
		}
		}
		int s=0;

		while (klocki[s].czy_istnieje==1 && klocki[i].czy_stop==0){
			if (s!=i){
			if (kolizja(klocki[i],klocki[s])==1){
				//cout<<s<<" "<<i<<endl;
				for (int m = 0; m<klocki[i].ilosc; m++){
					cout<<klocki[i].pos_x[m]<<" "<<klocki[i].pos_y[m]<<" "<<klocki[i].pos_z[m]<<endl;
				}
				cout<<endl;
				for (int m = 0; m<6; m++){
					cout<<klocki[s].pos_x[m]<<" "<<klocki[s].pos_y[m]<<" "<<klocki[s].pos_z[m]<<endl;
				}
				cout<<"KOLIZJA2!"<<endl<<endl<<endl;
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
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(Mk));
		if (k == 1) glUniform4f(spLambert->u("color"), 0, 1, 0, 1);
		if (k == 2) glUniform4f(spLambert->u("color"), 0, 0, 1, 1);
		if (k == 4) glUniform4f(spLambert->u("color"), 1, 0, 0, 1);
		Models::cube.drawSolid();
		klocki[i].ilosc = 27;
		klocki[i].pos_y[j] = pos;
		j++;

		glm::mat4 Mk1 = drawLeft(Mk, angleDelta, k, i, j);

		j++;
		glm::mat4 Mk2 = drawRight(Mk, angleDelta, k,i,j);
		j++;
		glm::mat4 Mk3 = drawTop(Mk, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk4 = drawLeft(Mk3, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk5 = drawRight(Mk3, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk6 = drawBottom(Mk, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk7 = drawLeft(Mk6, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk8 = drawRight(Mk6, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk9 = drawBehind(Mk, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk10 = drawTop(Mk9, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk11 = drawLeft(Mk9, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk12 = drawRight(Mk9, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk13 = drawLeft(Mk10, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk14 = drawRight(Mk10, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk15 = drawBottom(Mk9, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk16 = drawLeft(Mk15, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk17 = drawRight(Mk15, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk18 = drawInfront(Mk, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk19 = drawTop(Mk18, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk20 = drawBottom(Mk18, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk21 = drawLeft(Mk19, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk22 = drawRight(Mk19, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk23 = drawLeft(Mk20, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk24 = drawRight(Mk20, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk25 = drawRight(Mk18, angleDelta, k, i, j);
		j++;
		glm::mat4 Mk26 = drawLeft(Mk18, angleDelta, k, i, j);
		j++;



	}

	//void gears2(float angle, float gravity) {
	//	glm::mat4 I = glm::mat4(1.0f);
	//
	//	glm::mat4 Mt1 = glm::translate(I, glm::vec3(0.0f, 4.0f+gravity, 0.0f));
	//	Mt1 = glm::rotate(Mt1, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	//	Mt1 = glm::scale(Mt1, glm::vec3(0.5f, 0.5f, 0.5f));
	//
	//
	//	//Straight1(Mt1, 0);
	//
	//	//drawStraight(Mt1, 0, 4);
	//
	//	//drawT(Mt1, 0, 4);
	//
	//	drawKostka(Mt1, 0, 4);
	//
	//	//glm::mat4 Mt2 = glm::translate(I, glm::vec3(1.0f, 0.0f, 0.0f));
	//	//Mt2 = glm::scale(Mt2, glm::vec3(0.5f, 0.5f, 0.5f));
	//	//DrawStraight(Mt2, 0, 1);
	//	//glm::mat4 Mt2 = glm::translate(I, glm::vec3(1.05f, 0.0f, 0.0f));
	//	//Mt2 = glm::rotate(Mt2, -angle, glm::vec3(0.0f, 0.0f, 1.0f));
	//	//drawGear(Mt2, 11);
	//}



	//Drawing procedure
	void drawScene(GLFWwindow* window, float angle,float camera_angle, float camera_angle_x, float gravity) {
		//************Place any code here that draws something inside the window******************l
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear color and depth buffers
		using namespace std;
		glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Compute projection matrix
		glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Compute view matrix
		V = glm::rotate(V, camera_angle, glm::vec3(0.0f, 1.0f, 0.0f));
		V = glm::rotate(V, camera_angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
		spLambert->use();//Aktywacja programu cieniuj�cego
		glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P));
		glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V));


		int i = 0;
		while (klocki[i].czy_stop == 1) {
			if (klocki[i].rodzaj == 2) drawT(angle, 0, 4, gravity, 4.0f, i);
			if (klocki[i].rodzaj == 4) drawL(angle, 0, 4, gravity, 4.0f, i);
			if (klocki[i].rodzaj == 1) drawStraight(angle, 0, 4, gravity, 4.0f, i);
			if (klocki[i].rodzaj == 3) drawKostka(angle, 0, 4, gravity, 4.0f, i);
			klocki[i].czy_istnieje = 1;
			//cout<<klocki[i].rodzaj<<endl;
			i++;
		}

		if (klocki[i].rodzaj == 2 && klocki[i].czy_stop == 0) drawT(angle, 0, 4, gravity, 4.0f, i);
		if (klocki[i].rodzaj == 4 && klocki[i].czy_stop == 0) drawL(angle, 0, 4, gravity, 4.0f, i);
		if (klocki[i].rodzaj == 1 && klocki[i].czy_stop == 0) drawStraight(angle, 0, 4, gravity, 4.0f, i);
		if (klocki[i].rodzaj == 3 && klocki[i].czy_stop == 0) drawKostka(angle, 0, 4, gravity, 4.0f, i);
		klocki[i].czy_istnieje = 1;


		glfwSwapBuffers(window); //Copy back buffer to the front buffer
	}




	int main(void)
	{
		using namespace std;
		cout<<odleglosc(0,0,-4,1.63,0,0)<<endl;
		srand(time(NULL));
		GLFWwindow* window; //Pointer to object that represents the application window
		for (int i = 0; i < 1000; i++) {
			klocki[i].zapelnij();
		}
		glfwSetErrorCallback(error_callback);//Register error processing callback procedure

		if (!glfwInit()) { //Initialize GLFW library
			fprintf(stderr, "Can't initialize GLFW.\n");
			exit(EXIT_FAILURE);
		}

		window = glfwCreateWindow(1000, 1000, "OpenGL", NULL, NULL);  //Create a window 500pxx500px titled "OpenGL" and an OpenGL context associated with it.

		if (!window) //If no window is opened then close the program
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(window); //Since this moment OpenGL context corresponding to the window is active and all OpenGL calls will refer to this context.
		glfwSwapInterval(1); //During vsync wait for the first refresh

		GLenum err;
		if ((err = glewInit()) != GLEW_OK) { //Initialize GLEW library
			fprintf(stderr, "Can't initialize GLEW: %s\n", glewGetErrorString(err));
			exit(EXIT_FAILURE);
		}

		initOpenGLProgram(window); //Call initialization procedure

		//Main application loop
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
			glfwSetTime(0); //clear internal timer
			drawScene(window, angle,camera_angle,camera_angle_y,gravitation); //Execute drawing procedure
			glfwPollEvents(); //Process callback procedures corresponding to the events that took place up to now
		}
		freeOpenGLProgram(window);

		glfwDestroyWindow(window); //Delete OpenGL context and the window.
		glfwTerminate(); //Free GLFW resources
		exit(EXIT_SUCCESS);
	}
