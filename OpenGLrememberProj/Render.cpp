#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	

	double A1[] = { -8,  1, 0 };		// A1-D8 : Walls
	double B1[] = { -4,  6, 0 };
	double C1[] = { -4, 6, 2 };
	double D1[] = { -8, 1, 2 };

	double A2[] = { -4, 6, 0 };
	double B2[] = { 0, 1, 0 };
	double C2[] = { 0, 1, 2 };
	double D2[] = { -4, 6, 2 };

	double A3[] = { 0, 1, 0 };
	double B3[] = { 7, 4, 0 };
	double C3[] = { 7, 4, 2 };
	double D3[] = { 0, 1, 2 };

	double A4[] = { 7,  4, 0 };
	double B4[] = { 9, -1, 0 };
	double C4[] = { 9, -1, 2 };
	double D4[] = { 7,  4, 2 };

	double A5[] = { 9, -1, 0 };
	double B5[] = { 3, -5, 0 };
	double C5[] = { 3, -5, 2 };
	double D5[] = { 9, -1, 2 };

	double A6[] = { 3, -5, 0 };
	double B6[] = { 0, -1, 0 };
	double C6[] = { 0, -1, 2 };
	double D6[] = { 3, -5, 2 };

	double A7[] = { 0,  -1, 0 };
	double B7[] = { -5, -4, 0 };
	double C7[] = { -5, -4, 2 };
	double D7[] = { 0,  -1, 2 };

	double A8[] = { -5, -4, 0 };
	double B8[] = { -8,  1, 0 };
	double C8[] = { -8,  1, 2 };
	double D8[] = { -5, -4, 2 };

	double AA[] = { -8,  1, 0 };		// AA-DC : Base basement
	double BA[] = { -4,  6, 0 };
	double CA[] = { 0, -1, 0 };
	double DA[] = { -5, -4, 0 };

	double AB[] = { -4, 6, 0 };
	double BB[] = { 0, 1, 0 };
	double CB[] = { 3, -5, 0 };
	double DB[] = { 0, -1, 0 };

	double AC[] = { 0,  1, 0 };
	double BC[] = { 7,  4, 0 };
	double CC[] = { 9, -1, 0 };
	double DC[] = { 3, -5, 0 };

	double AAA[] = { -8,  1, 2 };		// AAA-DCC : Base attic
	double BAA[] = { -4,  6, 2 };
	double CAA[] = { 0, -1, 2 };
	double DAA[] = { -5, -4, 2 };

	double ABB[] = { -4, 6, 2 };
	double BBB[] = { 0,  1, 2 };
	double CBB[] = { 3, -5, 2 };
	double DBB[] = { 0, -1, 2 };

	double ACC[] = { 0,  1, 2 };
	double BCC[] = { 7,  4, 2 };
	double CCC[] = { 9, -1, 2 };
	double DCC[] = { 3, -5, 2 };

	

	glBegin(GL_QUADS);					// Base basement 
		glNormal3d(0, 0, -1);
		glVertex3dv(AA);
		glVertex3dv(BA);
		glVertex3dv(CA);
		glVertex3dv(DA);

		glVertex3dv(AB);
		glVertex3dv(BB);
		glVertex3dv(CB);
		glVertex3dv(DB);

		glVertex3dv(AC);
		glVertex3dv(BC);
		glVertex3dv(CC);
		glVertex3dv(DC);

	glEnd();

	glBegin(GL_QUADS);					// Walls

		glColor3d(0, 0, 1);
		glNormal3d(-0.78, 0.63, 0);
		glVertex3dv(A1);
		glVertex3dv(B1);
		glVertex3dv(C1);
		glVertex3dv(D1);

		glColor3d(0, 1, 0);
		glNormal3d(0.78, 0.63, 0);
		glVertex3dv(A2);
		glVertex3dv(B2);
		glVertex3dv(C2);
		glVertex3dv(D2);

		glColor3d(1, 0, 0);
		glNormal3d(-0.39, 0.92, 0);
		glVertex3dv(A3);
		glVertex3dv(B3);
		glVertex3dv(C3);
		glVertex3dv(D3);

		glColor3d(1, 0, 1);
		glNormal3d(0.93, 0.37, 0);
		glVertex3dv(A4);
		glVertex3dv(B4);
		glVertex3dv(C4);
		glVertex3dv(D4);

		glColor3d(1, 1, 0);
		glNormal3d(0.56, -0.83, 0);
		glVertex3dv(A5);
		glVertex3dv(B5);
		glVertex3dv(C5);
		glVertex3dv(D5);

		glColor3d(0.5, 0.2, 0.8);
		glNormal3d(-0.8, -0.6, 0);
		glVertex3dv(A6);
		glVertex3dv(B6);
		glVertex3dv(C6);
		glVertex3dv(D6);

		glColor3d(0.8, 0.5, 0.2);
		glNormal3d(0.52, -0.86, 0);
		glVertex3dv(A7);
		glVertex3dv(B7);
		glVertex3dv(C7);
		glVertex3dv(D7);

		glColor3d(0.2, 0.8, 0.5);
		glNormal3d(-0.86, -0.52, 0);
		glVertex3dv(A8);
		glVertex3dv(B8);
		glVertex3dv(C8);
		glVertex3dv(D8);

	glEnd();

	glBegin(GL_QUADS);					//Base attic
	glNormal3d(0, 0, 1);
	glVertex3dv(AAA);
	glVertex3dv(BAA);
	glVertex3dv(CAA);
	glVertex3dv(DAA);

	glVertex3dv(ABB);
	glVertex3dv(BBB);
	glVertex3dv(CBB);
	glVertex3dv(DBB);

	glColor4d(0, 1, 0, 0.7);
	glVertex3dv(ACC);
	glVertex3dv(BCC);
	glVertex3dv(CCC);
	glVertex3dv(DCC);

	glEnd();
	


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}