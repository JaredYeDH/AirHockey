#pragma once

#include "Constants.h"

//������������
class Point : public std::complex<double>
{
public:
	Point() : std::complex<double>(0, 0) {}
	Point(double x, double y) : std::complex<double>(x, y) {}
	Point(const std::complex<double> &p) : std::complex<double>(p) {}
	double x() const
	{
		return real();
	}
	double y() const
	{
		return imag();
	}
	std::tuple<double, double> pack() const
	{
		return std::make_tuple(real(), imag());
	}

	template<class T>
	Point& operator=(const T &p)
	{
		std::complex<double>::operator=(p);
		return *this;
	}

	/*Point& operator=(const std::complex<double> &p)
	{
		std::complex<double>::operator=(p);
		return *this;
	}*/
};

//ǰ������
class Game;

//AI����
class AI
{
protected:
	//AI����Ե�Game����
	Game* _game;
public:
	AI(Game* game) : _game(game) {}
	virtual ~AI() {}
	//ѯ��AI�����Ľӿڣ�����Ϊ����ʱ�䣨��λms��
	virtual Point QueryAction(int times) = 0;
};

//��AI
class SimpleAI : public AI
{
private:
	//Ѳ�����
	const Point C_s = Point(-G_malletStartDistance, -G_tableHeight / 4);
	//Ѳ���յ�
	const Point C_e = Point(-G_malletStartDistance, G_tableHeight / 4);
	//Ѳ��ʱ��
	const int C_step = 1000;
	//��Ѳ��ʱ��
	int _nowStep;

public:
	SimpleAI(Game* game) : AI(game) { _nowStep = C_step / 2; }
	virtual Point QueryAction(int times);
};

//�е�AI
class NormalAI : public AI
{
private:
	//AI�ٶȣ�ÿms�ƶ�����
	const double C_speed = 0.002;
public:
	NormalAI(Game* game) : AI(game) {}
	virtual Point QueryAction(int times);
};

//���AI
class CrazyAI : public AI
{
private:
	const double C_slowSpeed = 0.0005;	//�����ٶ�
	const double C_speed = 0.0015;	//��ͨ�ٶ�
	const double C_highSpeed = 0.003;	//�����ٶ�
	const double C_superSpeed = 0.004;	//�����ٶ�
public:
	CrazyAI(Game* game) : AI(game) {}
	virtual Point QueryAction(int times);
};

//��Ϸ�߼���
class Game
{
private:
	//���������߽磬�з������߽磬����߽�
	Point _playerBoard[4], _opponentBoard[4], _board[4];

	//��������λ�ã��з�����λ��
	Point _playerPos, _opponentPos;
	//����λ�ã������򣨵�λ������
	Point _puckPos, _puckDirection;

	
	int _gameStatus; //��ǰ��Ϸʤ��״̬(0������1Ӯ��2�䣩
	int _aiLevel; //AI����

	
	AI *_ai;	//AI����
	friend class NormalAI;
	friend class CrazyAI;

	
	//const double C_maxTimeInterval = 1;

	//��s��e�������ϵ��߽�ľ��루����ab�ı���Ϊ��λ��������res
	void QueryBoardDistance(const Point &s, const Point &e, const Point p[4], double res[4]);
	//��s��e�������ϵ��߽�ľ��루����ab�ı���Ϊ��λ���е���Сֵ
	double QueryBoardDistance(const Point &s, const Point &e, const Point p[4]);

	//��������s�ƶ���e�Ƿ��ײ������
	//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
	//������ײ��������ͱ����Ӱ�캯��
	std::tuple<double, std::function<void(Point&)>> TestMalletCollisionWithPuck(const Point &s, const Point &e);
	//��������s�ƶ���e�Ƿ��ײ���߽�
	//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
	//������ײ���������Ӱ�캯��
	std::tuple<double, std::function<void(Point&)>> TestMalletCollisionWithBoard(const Point &s, const Point &e, const Point p[4]);
	//�ƶ�������s��e��pΪ�߽磬malletPosΪ�ƶ��������λ�ã����������
	void MoveMallet(const Point &s, const Point &e, const Point p[4], Point &malletPos);

	//������s�ƶ���e�Ƿ��ײ���߽�
	//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
	//������ײ��Ա����Ӱ�졢ʤ�����жϺ���
	std::tuple<double, std::function<void()>> TestPuckCollisionWithBoard(const Point & s, const Point & e, const Point p[4]);
	//������s�ƶ���e�Ƿ��ײ������
	//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
	//������ײ��Ա����Ӱ�캯��
	std::tuple<double, std::function<void()>> TestPuckCollisionWithMallet(const Point &s, const Point &e, const Point &o);
	//�ƶ�����������ģ������ʱ��
	//������ײ��������ײ��������ײʱ��
	//����ײ��������ʱ��
	double MovePuck(double maxTime);
	
	//��ײ�����Ƿ�ʤ��
	void TestWin(int pos);

public:
	Game(int aiLevel) {
		_ai = 0;
		_aiLevel = aiLevel;

		//��ʼ���߽�λ��
		_opponentBoard[0] = Point(-G_tableWidth / 2 + G_malletRadius, -G_tableHeight / 2 + G_malletRadius);
		_opponentBoard[1] = Point(-G_tableWidth / 6 - G_malletRadius, -G_tableHeight / 2 + G_malletRadius);
		_opponentBoard[2] = Point(-G_tableWidth / 6 - G_malletRadius, +G_tableHeight / 2 - G_malletRadius);
		_opponentBoard[3] = Point(-G_tableWidth / 2 + G_malletRadius, +G_tableHeight / 2 - G_malletRadius);

		_playerBoard[0] = Point(+G_tableWidth / 6 + G_malletRadius, -G_tableHeight / 2 + G_malletRadius);
		_playerBoard[1] = Point(+G_tableWidth / 2 - G_malletRadius, -G_tableHeight / 2 + G_malletRadius);
		_playerBoard[2] = Point(+G_tableWidth / 2 - G_malletRadius, +G_tableHeight / 2 - G_malletRadius);
		_playerBoard[3] = Point(+G_tableWidth / 6 + G_malletRadius, +G_tableHeight / 2 - G_malletRadius);

		_board[0] = Point(-G_tableWidth / 2 + G_puckRadius, -G_tableHeight / 2 + G_puckRadius);
		_board[1] = Point(+G_tableWidth / 2 - G_puckRadius, -G_tableHeight / 2 + G_puckRadius);
		_board[2] = Point(+G_tableWidth / 2 - G_puckRadius, +G_tableHeight / 2 - G_puckRadius);
		_board[3] = Point(-G_tableWidth / 2 + G_puckRadius, +G_tableHeight / 2 - G_puckRadius);

		Restart();
	}
	~Game() {
		if(_ai) delete _ai;
	};

	//��ȡ�Լ�����λ��
	Point GetPlayerMalletPosition() const;
	//��ȡ��������λ��
	Point GetOpponentMalletPosition() const;
	//��ȡ����λ��
	Point GetPuckPosition() const;

	//����timesʱ��(ms)����������Ϊָ��x,y)
	//���룺����ʱ�䣬�������x,y
	//���أ�״̬ ����0��Ӯ1����2
	int Run(int times, double x, double y);

	//���¿�ʼ
	void Restart();
};