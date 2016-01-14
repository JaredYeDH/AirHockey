//#pragma comment(linker, "/subsystem:\"windows\"   /entry:\"mainCRTStartup\"")

#include "stdafx.h"
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "openglConst.h"
#include "Constants.h"
#include "Game.h"

/* ��Ϸ��ǰģʽ */
enum GameModeType {
	MainWindow, // ������
	Gaming, // ��Ϸ����
	Goal, // �÷ֽ���
	Win, // ʤ������
	Pause // ��ͣ����
};

Game *game; // ��Ϸ�߼�ģ��
GameModeType gameMode; // ��ǰ��Ϸģʽ
int score1, score2; // ˫���÷֣�1��ң�2AI
int flashCount; // �������������ͣ�������˸
int waitingTime; // ÿ����Ϸ��ʼ�ӳٵȴ�ʱ��ʣ��
int roundSet; // ʤ����������
int aiLevel; // AI�Ѷ�
int winner; // ʤ����

GLuint floorTexcture; // �ذ���ͼ���

GLfloat look_eye_dis; // �ӵ����ԭ�����
GLfloat look_eye[3]; // �ӵ�λ��
GLfloat look_up[3]; // �ӵ㷨��
GLfloat look_center[3]; // ����е�
GLfloat lookAngle; // �ӽ�

int mouseXrec, mouseYrec; // ���λ��

void Setup(); // ��ʼ������openGL��������
void Display(); // ��ʾ
void Reshape(int w, int h); // ���ڴ�С����

void LookPosMaintain(GLfloat delta, GLfloat shift); // �����ӽǺ;�ͷλ��
bool GetOGLPos(int x, int y, GLdouble &rx, GLdouble &ry); // ӳ�����������openGL����

void OnMouseMove(int x, int y); // ��׽����ƶ�
void Mouse(int button, int state, int x, int y); // ��׽��갴����Ϣ

void Keyboard(unsigned char key, int x, int y); // ��׽����������Ϣ
void SpecialKeys(int key, int x, int y); // ��׽���ⰴ����Ϣ

void Update(int value); // ���ˢ�º���


int main(int argc, char **argv)
{
	/* ��ʼ��openGL */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(MYGL_WINDOW_W, MYGL_WINDOW_H);
	glutInitWindowPosition(MYGL_WINDOW_POS_X, MYGL_WINDOW_POS_Y);
	glutCreateWindow(MY_GL_WINDOW_NAME);
	Setup();

	/* ע��ص����� */
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(Mouse);
	glutPassiveMotionFunc(OnMouseMove);
	glutMotionFunc(OnMouseMove);

	/* ���ˢ�º��� */
	glutTimerFunc(GAME_DELTA_TIME, Update, 0);

	/* main loop */
	glutMainLoop();
	return 0;
}

/* ʹ��windowAPI��ȡbmpͼƬ��������ͼ
 * ��ȡbmp���ֿ���ʹ��������������
 */
bool LoadTexture(LPTSTR szFileName, GLuint &texid)   // Creates Texture From A Bitmap File
{
	HBITMAP hBMP;       // Handle Of The Bitmap
	BITMAP BMP;       // Bitmap Structure
	glGenTextures(1, &texid);      // Create The Texture
	hBMP = (HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	if (!hBMP)        // Does The Bitmap Exist?
		return FALSE;       // If Not Return False
	GetObject(hBMP, sizeof(BMP), &BMP);     // Get The Object
											// hBMP:    Handle To Graphics Object
											// sizeof(BMP): Size Of Buffer For Object Information
											// &BMP:    Buffer For Object Information
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);    // Pixel Storage Mode (Word Alignment / 4 Bytes)
											  // Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texid);     // Bind To The Texture ID
											 //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Mag Filter
																	  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
	DeleteObject(hBMP);       // Delete The Object
	return TRUE;       // Loading Was Successful
}

GLuint WordShowLists = 0; // openGL��ʾ�ַ��б�selectFontѡ������ʱ�޸ģ�drawStringʱʹ��

/* �������� */
void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_BOLD, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);

	// ����MAX_CHAR����������ʾ�б���
	if (WordShowLists == 0)
		WordShowLists = glGenLists(128);
	// ��ÿ���ַ��Ļ������װ����Ӧ����ʾ�б���
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 128, WordShowLists);
}

/* �����ַ��� */
void drawString(const char* str)
{
	// ����ÿ���ַ���Ӧ����ʾ�б�����ÿ���ַ�
	for (; *str != '\0'; ++str)
		glCallList(WordShowLists + *str);
}


void Setup()
{
	game = new Game();
	gameMode = MainWindow;
	flashCount = 0;
	roundSet = Game_Round_Init;
	aiLevel = Game_AI_INIT;
	mouseXrec = mouseYrec = 0;
	waitingTime = 0;

	// light on
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// light setup
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	// material setup
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	// texcture
	LoadTexture("floor.bmp", floorTexcture);

	// other init
	lookAngle = look_init_angle;
	look_eye_dis = look_eye_dis_init;
	LookPosMaintain(0, 0);

	glutGet(GLUT_ELAPSED_TIME);
}

void LookPosMaintain(GLfloat delta, GLfloat shift)
{
	lookAngle += delta;
	if (lookAngle < 0.01) lookAngle += 360;
	if (lookAngle > 359.99) lookAngle -= 360;
	look_eye_dis += shift;
	if (look_eye_dis < look_eye_dis_min) look_eye_dis = look_eye_dis_min;
	if (look_eye_dis > look_eye_dis_max) look_eye_dis = look_eye_dis_max;
	// �ӵ�λ��
	look_eye[0] = look_eye_dis * cos(look_dep_angle * PI / 180) * cos(lookAngle * PI / 180);
	look_eye[1] = look_eye_dis * cos(look_dep_angle * PI / 180) * sin(lookAngle * PI / 180);
	look_eye[2] = look_eye_dis * sin(look_dep_angle * PI / 180);
	// �ӵ㷨��
	look_up[0] = look_eye_dis * sin(look_dep_angle * PI / 180) * -cos(lookAngle * PI / 180);
	look_up[1] = look_eye_dis * sin(look_dep_angle * PI / 180) * -sin(lookAngle * PI / 180);
	look_up[2] = look_eye_dis * cos(look_dep_angle * PI / 180);
	// ����е�
	GLfloat tmp = look_eye_dis - look_scn_dis;
	look_center[0] = tmp * cos(look_dep_angle * PI / 180) * cos(lookAngle * PI / 180);
	look_center[1] = tmp * cos(look_dep_angle * PI / 180) * sin(lookAngle * PI / 180);
	look_center[2] = tmp * sin(look_dep_angle * PI / 180);
	//look_center[0] = 0, look_center[1] = 0, look_center[2] = 0;
}

/* ����ʵ��Բ�� */
void glutSolidCylinder(GLfloat red, GLfloat green, GLfloat blue,
	GLdouble x, GLdouble y, GLdouble z, GLdouble h, GLdouble r,
	GLint slices, GLint stacks)
{
	glPushMatrix();
	// Cylinder
	glColor3f(red - 0.02, green, blue);
	glTranslatef(x, y, z);
	GLUquadricObj *objCylinder = gluNewQuadric();
	gluCylinder(objCylinder, r, r, h, slices, stacks);
	// top
	glColor3f(red, green, blue);
	glTranslatef(0, 0, h);
	GLUquadricObj *objDisk = gluNewQuadric();
	gluDisk(objDisk, 0, r, slices, 10);
	// delete
	gluDeleteQuadric(objDisk);
	gluDeleteQuadric(objCylinder);
	glPopMatrix();
}

/* �������� */
void glutSolidMallet(GLfloat red, GLfloat green, GLfloat blue,
	GLdouble x, GLdouble y, GLdouble z, GLdouble h, GLdouble r,
	GLint slices, GLint stacks)
{
	glPushMatrix();
	// Cylinder
	glColor3f(red - 0.02, green, blue);
	glTranslatef(x, y, z);
	GLUquadricObj *objCylinder = gluNewQuadric();
	gluCylinder(objCylinder, r, r, h, slices, stacks);
	// ball
	glColor3f(red, green, blue);
	glTranslatef(0, 0, h);
	GLUquadricObj *objSphere = gluNewQuadric();
	gluSphere(objSphere, r / 2, slices, slices);
	// top
	glColor3f(red, green, blue);
	GLUquadricObj *objDisk = gluNewQuadric();
	gluDisk(objDisk, 0, r, slices, 10);
	// delete
	gluDeleteQuadric(objSphere);
	gluDeleteQuadric(objDisk);
	gluDeleteQuadric(objCylinder);
	glPopMatrix();
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(look_eye[0], look_eye[1], look_eye[2],
		look_center[0], look_center[1], look_center[2],
		look_up[0], look_up[1], look_up[2]);

	// ���Ƶ��沢��ͼ
	{
		//glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glNormal3f(0, 0, 1);
		glTexCoord2f(0, 0);
		glVertex3f(G_FloorPlaneSize, -G_FloorPlaneSize, -G_TableAltitude);
		glNormal3f(0, 0, 1);
		glTexCoord2f(0, TileCount);
		glVertex3f(G_FloorPlaneSize, G_FloorPlaneSize, -G_TableAltitude);
		glNormal3f(0, 0, 1);
		glTexCoord2f(TileCount, TileCount);
		glVertex3f(-G_FloorPlaneSize, G_FloorPlaneSize, -G_TableAltitude);
		glNormal3f(0, 0, 1);
		glTexCoord2f(TileCount, 0);
		glVertex3f(-G_FloorPlaneSize, -G_FloorPlaneSize, -G_TableAltitude);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		//glPopMatrix();
	}

	// ���������Ե�����ӱ߿�
	{
		//glPushMatrix();
		glColor3f(0.93, 0.93, 0.93);
		// long side
		glPushMatrix();
		glTranslatef(0, G_tableHeight / 2 + G_TablePly / 2, -G_TableAltitude / 2 + G_TablePly / 2);
		glScalef(G_tableWidth + 2 * G_TablePly, G_TablePly, G_TableAltitude + G_TablePly);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, -G_tableHeight / 2 - G_TablePly / 2, -G_TableAltitude / 2 + G_TablePly / 2);
		glScalef(G_tableWidth + 2 * G_TablePly, G_TablePly, G_TableAltitude + G_TablePly);
		glutSolidCube(1);
		glPopMatrix();
		// short side
		glPushMatrix();
		glTranslatef(G_tableWidth / 2 + G_TablePly / 2, 0, -G_TableAltitude / 2);
		glScalef(G_TablePly, G_tableHeight, G_TableAltitude);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-G_tableWidth / 2 - G_TablePly / 2, 0, -G_TableAltitude / 2);
		glScalef(G_TablePly, G_tableHeight, G_TableAltitude);
		glutSolidCube(1);
		glPopMatrix();
		// door edge
		glPushMatrix();
		glTranslatef(G_tableWidth / 2 + G_TablePly / 2, (G_tableHeight + G_goalWidth) / 4, G_TablePly / 2);
		glScalef(G_TablePly, (G_tableHeight - G_goalWidth) / 2, G_TablePly);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(G_tableWidth / 2 + G_TablePly / 2, -(G_tableHeight + G_goalWidth) / 4, G_TablePly / 2);
		glScalef(G_TablePly, (G_tableHeight - G_goalWidth) / 2, G_TablePly);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-G_tableWidth / 2 - G_TablePly / 2, (G_tableHeight + G_goalWidth) / 4, G_TablePly / 2);
		glScalef(G_TablePly, (G_tableHeight - G_goalWidth) / 2, G_TablePly);
		glutSolidCube(1);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(-G_tableWidth / 2 - G_TablePly / 2, -(G_tableHeight + G_goalWidth) / 4, G_TablePly / 2);
		glScalef(G_TablePly, (G_tableHeight - G_goalWidth) / 2, G_TablePly);
		glutSolidCube(1);
		glPopMatrix();
		//glPopMatrix();
	}

	// ��������
	{
		//glPushMatrix();
		glBegin(GL_QUADS);
		//glColor3f(0.4392, 0.7255, 0.3137);
		glColor3f(0.2902, 0.498, 0.2);
		glNormal3f(0, 0, 1);
		glVertex3f(G_tableWidth / 2, -G_tableHeight / 2, 0);
		glNormal3f(0, 0, 1);
		glVertex3f(G_tableWidth / 2, G_tableHeight / 2, 0);
		glNormal3f(0, 0, 1);
		glVertex3f(-G_tableWidth / 2, G_tableHeight / 2, 0);
		glNormal3f(0, 0, 1);
		glVertex3f(-G_tableWidth / 2, -G_tableHeight / 2, 0);
		glEnd();
		glColor3f(0.0863, 0.2549, 0.0118);
		glLineWidth(2);
		glBegin(GL_LINE_STRIP);
		glVertex3f(G_tableWidth * (0.5 - GL_MalletRange), G_tableHeight / 2, 0.005);
		glVertex3f(G_tableWidth * (0.5 - GL_MalletRange), -G_tableHeight / 2, 0.005);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3f(-G_tableWidth * (0.5 - GL_MalletRange), G_tableHeight / 2, 0.005);
		glVertex3f(-G_tableWidth * (0.5 - GL_MalletRange), -G_tableHeight / 2, 0.005);
		glEnd();
		glBegin(GL_LINE_LOOP);
		double R = GL_KickOffCircle_R * G_tableWidth;
		for (int i = 0; i < 32; ++i)
			glVertex3f(R * cos(2 * PI / 32 * i), R * sin(2 * PI / 32 * i), 0.005);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3f(0, G_tableHeight / 2, 0.005);
		glVertex3f(0, R, 0.005);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3f(0, -R, 0.005);
		glVertex3f(0, -G_tableHeight / 2, 0.005);
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		R = G_goalWidth / 2 + 0.01;
		for (int i = 8; i <= 24; ++i) {
			glVertex3f(G_tableWidth / 2, 0, 0.005);
			glVertex3f(G_tableWidth / 2 + R * cos(2 * PI / 32 * i), R * sin(2 * PI / 32 * i), 0.005);
		}
		glEnd();
		glBegin(GL_TRIANGLE_STRIP);
		R = G_goalWidth / 2 + 0.01;
		for (int i = -8; i <= 8; ++i) {
			glVertex3f(-G_tableWidth / 2, 0, 0.005);
			glVertex3f(-G_tableWidth / 2 + R * cos(2 * PI / 32 * i), R * sin(2 * PI / 32 * i), 0.005);
		}
		glEnd();
		//glPopMatrix();
	}

	// ���Ʊ�������
	{
		Point pt;
		pt = game->GetPuckPosition();
		glutSolidCylinder(ColorRed[0], ColorRed[1], ColorRed[2],
			pt.x(), pt.y(), 0, G_PuckHeight, G_PuckDiameter / 2, 32, 1);
		pt = game->GetPlayerMalletPosition();
		//std::cout << pt.x() << " " << pt.y() << std::endl;
		glutSolidMallet(ColorBlue[0], ColorBlue[1], ColorBlue[2],
			pt.x(), pt.y(), 0, G_MalletHeight, G_MalletDiameter / 2, 32, 1);
		pt = game->GetOpponentMalletPosition();
		glutSolidMallet(ColorViolet[0], ColorViolet[1], ColorViolet[2],
			pt.x(), pt.y(), 0, G_MalletHeight, G_MalletDiameter / 2, 32, 1);
	}

	if (gameMode == MainWindow) { // ��������ʾ
		// title
		double k = 1 / sqrt(look_eye[0] * look_eye[0] + look_eye[1] * look_eye[1]), t;
		selectFont(80, ANSI_CHARSET, "System");
		//glPushMatrix();
		glColor3f(0.2706, 0.1451, 0.0118);
		t = look_eye_dis / look_eye_dis_init * 1.2;
		glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 1.5 * t);
		drawString(MY_GL_WINDOW_NAME);
		//glPopMatrix();
		// press enter
		if (flashCount < 0.6 * Word_Flash_Time) {
			//glPushMatrix();
			selectFont(45, ANSI_CHARSET, "System");
			glColor3f(0.15, 0.15, 0.15);
			t = look_eye_dis / look_eye_dis_init * 1.145;
			glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 2.2,
				-look_eye[0] * k * t + look_eye[1] * k * t * 2.2, 1 * t);
			drawString("press ENTER to start");
			//glPopMatrix();
		}
		// round set
		//glPushMatrix();
		selectFont(45, ANSI_CHARSET, "System");
		glColor3f(0.2, 0.2, 0.2);
		t = look_eye_dis / look_eye_dis_init * 1.72;
		glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 0.4 * t);
		char buf[35];
		sprintf_s(buf, "reach %2d round to win", roundSet);
		drawString(buf);
		//glPopMatrix();
		// ai choose
		//glPushMatrix();
		selectFont(45, ANSI_CHARSET, "System");
		glColor3f(0.2, 0.2, 0.2);
		t = look_eye_dis / look_eye_dis_init * 1.37;
		glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 1.2,
			-look_eye[0] * k * t + look_eye[1] * k * t * 1.2, 1 * t);
		sprintf_s(buf, "you will meet %s AI", AILevelName[aiLevel]);
		drawString(buf);
		//glPopMatrix();
	}
	else if (gameMode == Gaming) { // ��Ϸ������ʾ
		// board
		char buf[5];
		double k = 1 / sqrt(look_eye[0] * look_eye[0] + look_eye[1] * look_eye[1]);
		double t = look_eye_dis / look_eye_dis_init * 0.7;
		
		selectFont(48, ANSI_CHARSET, "System");
		//glPushMatrix();
		glColor3f(ColorBlue[0], ColorBlue[1], ColorBlue[2]);
		glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 3 * t);
		sprintf_s(buf, "%d", score1);
		drawString(buf);
		//glPopMatrix();

		//glPushMatrix();
		glColor3f(ColorViolet[0], ColorViolet[1], ColorViolet[2]);
		glRasterPos3f(-look_eye[1] * k * t, look_eye[0] * k * t, 3 * t);
		sprintf_s(buf, "%d", score2);
		drawString(buf);
		//glPopMatrix();

		//glPushMatrix();
		glColor3f(0, 0, 0);
		glRasterPos3f(0, 0, 3 * t);
		drawString(":");
		//glPopMatrix();

		if (waitingTime > 0) {
			//glPushMatrix();
			selectFont(90, ANSI_CHARSET, "System");
			glColor3f(0.9098, 0.6784, 0.4314);
			t = look_eye_dis / look_eye_dis_init * 0.2;
			glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 0.7 * t);
			char buf[5];
			if (waitingTime > GameStart_ShowingTime)
				sprintf_s(buf, "%d", (waitingTime - GameStart_ShowingTime) / 1000 + 1);
			else
				sprintf_s(buf, "%s", "Go!");
			drawString(buf);
			//glPopMatrix();
		}
	}
	else if (gameMode == Goal) { // ���������ʾ
		double k = 1 / sqrt(look_eye[0] * look_eye[0] + look_eye[1] * look_eye[1]), t;
		selectFont(80, ANSI_CHARSET, "System");
		//glPushMatrix();
		if (winner == 1) {
			glColor3f(0.0902, 0.0902, 0.6980);
			t = look_eye_dis / look_eye_dis_init * 1.3;
			glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 1.4 * t);
			drawString("You Goaled");
		}
		else {
			glColor3f(0.4039, 0.2588, 0.7725);
			t = look_eye_dis / look_eye_dis_init * 1.4;
			glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 1.3 * t);
			drawString("You Fumbled");
		}
		//glPopMatrix();

		//glPushMatrix();
		selectFont(45, ANSI_CHARSET, "System");
		glColor3f(0.15, 0.15, 0.15);
		t = look_eye_dis / look_eye_dis_init * 1.22;
		glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 2.2,
			-look_eye[0] * k * t + look_eye[1] * k * t * 2.2, 1 * t);
		drawString("press ENTER to continue");
		//glPopMatrix();

		//glPushMatrix();
		selectFont(42, ANSI_CHARSET, "System");
		glColor3f(0.15, 0.15, 0.15);
		t = look_eye_dis / look_eye_dis_init * 0.7;
		glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 3.6,
			-look_eye[0] * k * t + look_eye[1] * k * t * 3.6, 0.85 * t);
		drawString("press R to return");
		//glPopMatrix();
	}
	else if (gameMode == Win) { // ʤ��������ʾ
		double k = 1 / sqrt(look_eye[0] * look_eye[0] + look_eye[1] * look_eye[1]), t;
		selectFont(80, ANSI_CHARSET, "System");
		//glPushMatrix();
		if (winner == 1) {
			glColor3f(0.0902, 0.0902, 0.6980);
			t = look_eye_dis / look_eye_dis_init * 1.1;
			glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 1.4 * t);
			drawString("You Win !");
		}
		else {
			glColor3f(0.4039, 0.2588, 0.7725);
			t = look_eye_dis / look_eye_dis_init * 1.06;
			glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 1.3 * t);
			drawString("You Lose");
		}
		//glPopMatrix();

		//glPushMatrix();
		selectFont(45, ANSI_CHARSET, "System");
		glColor3f(0.15, 0.15, 0.15);
		t = look_eye_dis / look_eye_dis_init * 1.22;
		glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 2.2,
			-look_eye[0] * k * t + look_eye[1] * k * t * 2.2, 1 * t);
		drawString("press ENTER to restart");
		//glPopMatrix();

		//glPushMatrix();
		selectFont(42, ANSI_CHARSET, "System");
		glColor3f(0.15, 0.15, 0.15);
		t = look_eye_dis / look_eye_dis_init * 0.7;
		glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 3.6,
			-look_eye[0] * k * t + look_eye[1] * k * t * 3.6, 0.85 * t);
		drawString("press R to return");
		//glPopMatrix();
	}
	else if (gameMode == Pause) { //��ͣ������ʾ
		double k = 1 / sqrt(look_eye[0] * look_eye[0] + look_eye[1] * look_eye[1]), t;
		if (flashCount < 0.6 * Word_Flash_Time) {
			selectFont(80, ANSI_CHARSET, "System");
			//glPushMatrix();
			glColor3f(0.25, 0.25, 0.25);
			t = look_eye_dis / look_eye_dis_init * 0.8;
			glRasterPos3f(look_eye[1] * k * t, -look_eye[0] * k * t, 1.7 * t);
			drawString("Pause");
			//glPopMatrix();
		}

		//glPushMatrix();
		selectFont(45, ANSI_CHARSET, "System");
		glColor3f(0.15, 0.15, 0.15);
		t = look_eye_dis / look_eye_dis_init * 1.22;
		glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 2.2,
			-look_eye[0] * k * t + look_eye[1] * k * t * 2.2, 1 * t);
		drawString("press ENTER to continue");
		//glPopMatrix();

		//glPushMatrix();
		selectFont(42, ANSI_CHARSET, "System");
		glColor3f(0.15, 0.15, 0.15);
		t = look_eye_dis / look_eye_dis_init * 0.7;
		glRasterPos3f(look_eye[1] * k * t + look_eye[0] * k * t * 3.6,
			-look_eye[0] * k * t + look_eye[1] * k * t * 3.6, 0.85 * t);
		drawString("press R to return");
		//glPopMatrix();
	}

	glFlush();
	glutSwapBuffers();
}

/* ӳ�����������openGL����
 * ����λ��Ϊ���ָ���������������
 * ������Z = 0ƽ��
 * ����ֵʼ��Ϊ�棬������
 */
bool GetOGLPos(int x, int y, GLdouble &rx, GLdouble &ry)
{
	static GLint viewport[4];
	static GLdouble modelview[16];
	static GLdouble projection[16];
	static GLfloat winX, winY, winZ;
	static GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	winX = (float)x, winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	//std::cout << "get " << posX << " " << posY << " " << posZ << std::endl;
	// ��������ڱ߽��Բ���ϵ����
	if (posZ > 1e-5 || posZ < -1e-5) {
		// ���������ϣ���Ҫ��������
		double k = posZ / (posZ - look_eye[2]);
		posX += k * (look_eye[0] - posX);
		posY += k * (look_eye[1] - posY);
		posZ = 0;
	}
	//std::cout << "real " << posX << " " << posY << " " << posZ << std::endl;
	rx = posX, ry = posY;
	return true;
}

void Reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(look_dep_angle, (GLfloat)w / h, 0.01, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

void OnMouseMove(int x, int y)
{
	mouseXrec = x, mouseYrec = y;
}

void Mouse(int button, int state, int x, int y)
{
	mouseXrec = x, mouseYrec = y;
}

void Keyboard(unsigned char key, int x, int y)
{
	if (gameMode == MainWindow) { // ��������Ӧ�س�
		switch (key) {
		case 13: // ENTER
			game->Restart();
			gameMode = Gaming;
			waitingTime = GameStart_WaitingTime;
			score1 = score2 = 0;
			break;
		}
	}
	else if (gameMode == Goal) { // �÷ֽ�����Ӧ�س���R
		switch (key) {
		case 13: // ENTER
			game->Restart();
			gameMode = Gaming;
			waitingTime = GameStart_WaitingTime;
			break;
		case 'r':
			game->Restart();
			gameMode = MainWindow;
			flashCount = 0;
			break;
		}
	}
	else if (gameMode == Win) { // ʤ��������Ӧ�س���R
		switch (key) {
		case 13: // ENTER
			game->Restart();
			gameMode = Gaming;
			waitingTime = GameStart_WaitingTime;
			score1 = score2 = 0;
			break;
		case 'r':
			game->Restart();
			gameMode = MainWindow;
			flashCount = 0;
			break;
		}
	}
	else if (gameMode == Gaming) { // ��Ϸ������ӦP
		switch (key) {
		case 'p':
			gameMode = Pause;
			flashCount = 0;
			break;
		}
	}
	else if (gameMode == Pause) { // ��ͣ������Ӧ�س���R
		switch (key) {
		case 13: // ENTER
			gameMode = Gaming;
			break;
		case 'r':
			game->Restart();
			gameMode = MainWindow;
			flashCount = 0;
			break;
		}
	}
}

void SpecialKeys(int key, int x, int y)
{
	if (gameMode == Gaming || gameMode == Goal || gameMode == Pause || gameMode == Win) {
		// �������棬���������Ӧ�������ҽ����ӵ����
		switch (key) {
		case GLUT_KEY_LEFT:
			//std::cout << "LEFT" << std::endl;
			LookPosMaintain(-look_delta_angle, 0);
			break;
		case GLUT_KEY_RIGHT:
			//std::cout << "RIGHT" << std::endl;
			LookPosMaintain(look_delta_angle, 0);
			break;
		case GLUT_KEY_UP:
			//std::cout << "UP" << std::endl;
			LookPosMaintain(0, -look_eye_dis_delta);
			break;
		case GLUT_KEY_DOWN:
			//std::cout << "DOWN" << std::endl;
			LookPosMaintain(0, look_eye_dis_delta);
			break;
		}
	}
	else if (gameMode == MainWindow) {
		// ��������Ӧ�������ҽ�����Ϸ�趨
		switch (key) {
		case GLUT_KEY_UP:
			++roundSet;
			if (roundSet > Game_Round_MAX)
				roundSet = Game_Round_MAX;
			break;
		case GLUT_KEY_DOWN:
			--roundSet;
			if (roundSet < Game_Round_MIN)
				roundSet = Game_Round_MIN;
			break;
		case GLUT_KEY_LEFT:
			--aiLevel;
			if (aiLevel < 0)
				aiLevel = 0;
			break;
		case GLUT_KEY_RIGHT:
			++aiLevel;
			if (aiLevel >= Game_AI_NUM)
				aiLevel = Game_AI_NUM - 1;
			break;
		}
	}
}

void Update(int value)
{
	int game_delta_time_real = glutGet(GLUT_ELAPSED_TIME);
	//int game_delta_time_real = GLUT_ELAPSED_TIME;
	if (game_delta_time_real > 2 * GAME_DELTA_TIME)
		game_delta_time_real = 2 * GAME_DELTA_TIME;
	//std::cout << game_delta_time_real << std::endl;

	if (gameMode == Gaming) {
		if (waitingTime > 0) {
			waitingTime -= game_delta_time_real;
		}
		else {
			/* call game with player mallet position & delta-time
			* the coord of mallets has to be transed into 2D
			* the center of plant is (0, 0)
			*/
			GLdouble x, y;
			if (GetOGLPos(mouseXrec, mouseYrec, x, y)) {
				int ret = game->Run(game_delta_time_real, x, y);
				if (ret) {
					if (ret == 1) ++score1, winner = 1; else ++score2, winner = 2;
					if (score1 == roundSet || score2 == roundSet)
						gameMode = Win;
					else
						gameMode = Goal;
				}
			}
		}
	}
	else if (gameMode == MainWindow) {
		// ��������˸��ʾ
		flashCount += game_delta_time_real;
		if (flashCount >= Word_Flash_Time)
			flashCount -= Word_Flash_Time;
		// �������ӽǻ�����ת
		LookPosMaintain(look_delta_angle_shift, 0);
	}
	else if (gameMode == Pause) {
		// ��ͣ������˸��ʾ
		flashCount += game_delta_time_real;
		if (flashCount >= Word_Flash_Time)
			flashCount -= Word_Flash_Time;
	}

	/* display */
	glutPostRedisplay();
	/* recursive call */
	glutTimerFunc(game_delta_time_real, Update, 0);
}
