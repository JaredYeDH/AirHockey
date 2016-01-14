#pragma once

#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>

const double PI = 3.14159265358979323;

const int MYGL_WINDOW_W = 800; // ���ڿ��
const int MYGL_WINDOW_H = 600; // ���ڸ߶�
const int MYGL_WINDOW_POS_X = 200; // ����λ��X
const int MYGL_WINDOW_POS_Y = 200; // ����λ��Y
const char MY_GL_WINDOW_NAME[] = "AirHockey"; // ��Ϸ����

const GLfloat look_init_angle = 30; // ��ʼ�ӽ�45deg
const GLfloat look_delta_angle = 1; // ��ת�ӽǼ��1deg
const GLfloat look_delta_angle_shift = 0.016; // ��������ת�ӽǼ��0.016deg

/* �ӵ����ԭ�����
 * �仯��Χ 5.5~10.1
 * �仯��� 0.2
 * ��ʼ���� 6.5
 * ����Ϊ���泤 4
 */
const GLfloat look_eye_dis_init = 6.5;
const GLfloat look_eye_dis_base = 4;
const GLfloat look_eye_dis_max = 10.1;
const GLfloat look_eye_dis_min = 5.5;
const GLfloat look_eye_dis_delta = 0.2;
const GLfloat look_scn_dis = 1; // �ӵ㷽��ĳһ���룬�����峣��

const GLfloat look_dep_angle = 45; // �ӵ㸩��45deg

/* ���ռ��������ó��� */
const GLfloat light_pos[] = { 0, 5, 30, 1.0 };
const GLfloat light_ambient[] = { 0.1f, 0.2f, 0.2f, 1.0f };
const GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat light_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
const GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
const GLfloat mat_emission[] = { 0.0, 0.0, 0.0, 1.0 };

/* ��Ϸѭ��ʱ���� */
const int GAME_DELTA_TIME = 10; // ms

const GLint TileCount = 70; // Ӱ��ذ�ƽ���ܶ�

const GLfloat ColorRed[] = { 0.8157f, 0.0157f, 0.0157f }; // ��ɫ
const GLfloat ColorBlue[] = { 0.1765f, 0.1765f, 0.9765f }; // ��ɫ
const GLfloat ColorViolet[] = { 0.5686f, 0.0039f, 0.9490f }; // ��ɫ

//const int WIN_MAIN_WIN_START = 0;
//const int WIN_MAIN_WIN_END = 2;
//const int WIN_LEVEL_CHOOSE_START = 5;
//const int WIN_LEVEL_CHOOSE_END = 8;
//const int WIN_GOAL_START = 8;
//const int WIN_GOAL_END = 10;
//const int WIN_END_START = 10;
//const int WIN_END_END = 12;
//const char LabelString[][15] = {
//	"AirHockey",
//	"single player",
//	"double player",
//	"auto",
//	"help",
//	"sample",
//	"hard",
//	"ferocious",
//	"continue",
//	"return",
//	"restart",
//	"return"
//};

/* ��Ϸһ��ʤ������÷���
 * ��ΧΪ 1~99
 * ��ʼΪ 3 
 */
const int Game_Round_Init = 3;
const int Game_Round_MIN = 1;
const int Game_Round_MAX = 99;

/* AI�Ѷ�
 * ��Ϊ3��
 */
const int Game_AI_INIT = 1;
const int Game_AI_NUM = 3;
const char AILevelName[][10] = {
	"EASY",
	"NORMAL",
	"CRAZY"
};

const int Word_Flash_Time = 1400; // (ms) ������˸ѭ��ʱ�䣬����ΪGAME_DELTA_TIME�ı���

const double GL_MalletRange = 0.33333333; // =1/3 �����ƶ���Χ�����λ��
const double GL_KickOffCircle_R = 0.1; // =1/10 ����Ȧ�뾶��Դ�С

const int GameStart_WaitingSecond = 3; // (s) ÿ����Ϸ��ʼ�ӳٵȴ�����
const int GameStart_ShowingTime = 500; // (ms) ��Ϸ��ʼ��ʾ��ʾʱ��
const int GameStart_WaitingTime = GameStart_WaitingSecond * 1000 + GameStart_ShowingTime; // (ms) ÿ����Ϸ��ʼ�ӳٵȴ�ʱ��
