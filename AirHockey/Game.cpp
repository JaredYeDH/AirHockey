#include "stdafx.h"


#include "Game.h"

//���㼸�β���

//��� aXb
inline double Cross(const Point &a, const Point &b)
{
	return imag(conj(a) * b);
}
//��� (a-o)X(b-o)
inline double Cross(const Point &a, const Point &o, const Point &b)
{
	return Cross(a - o, b - o);
}
//������ʱ����ת90��
inline Point Rotate(const Point &a)
{
	return a * Point(0, 1);
}
//����Χ���жϴ���С�ڵ���
inline int dblcmp(const double &x)
{
	if (x > G_eps) return 1;
	if (x < -G_eps) return -1;
	return 0;
}
//��ֱ��ab��cd����
inline Point QueryLinesIntersection(const Point &a, const Point &b, const Point &c, const Point &d)
{
	double S1 = Cross(c, a, b);
	double S2 = Cross(b, a, d);
	//�����
	return c + (d - c) * S1 / (S1 + S2);
}
//���c��ֱ��ab�ϵ�λ�ã���c=a+(b-a)*ans
inline double QueryPointInLine(const Point &a, const Point &b, const Point &c)
{
	Point ans = (c - a) / (b - a);
	return real(ans) > 0 ? abs(ans) : -abs(ans);
}

//��ֱ�ߺ�Բ�Ľ��㣬�����Ƿ�<�ཻ������1������2>������˳����a��b����ȥ��������ʱ����
inline std::tuple<bool,Point,Point> QueryLineCircleIntersection(const Point &a, const Point &b, const Point &o, double r)
{
	Point h = QueryLinesIntersection(a, b, o, o + Rotate(b - a));//����
	double perpendicularDis2 = norm(h - o);//Բ�Ĵ������
	double dis2 = r * r - perpendicularDis2;//���㴹�����
	if (dis2 < G_eps*G_eps) return make_tuple(false, Point(), Point());
	Point disVec = (b - a) / abs(b - a) * sqrt(dis2);
	return make_tuple(true, h - disVec, h + disVec);
}

//��Բ��ֱ�ߵĵ�һ������ֱ���ϵ�λ��
//���������ֱ���������򷵻�λ�ã����򷵻�1
inline double QueryLineCircleIntersectionPos(const Point &a, const Point &b, const Point &o, double r)
{
	Point firstIntersection, secondIntersection;
	bool ifIntersect;
	std::tie(ifIntersect, firstIntersection, secondIntersection) =
		QueryLineCircleIntersection(a, b, o, r);
	if (ifIntersect) {
		double pos = QueryPointInLine(a, b, firstIntersection);
		if (pos > G_eps) {
			return pos;
		}else if (pos > -G_eps) {	//��һ������a��
			if (QueryPointInLine(a, b, secondIntersection) > G_eps)//�������㲻�غϣ�����Ϊ���������
				return 0;
		}
	}
	return 1;
}

//��ֱ�ߺ�ֱ�ߵĽ�����ֱ��ab�ϵ�λ��
//���������ֱ��������ͷ���λ�ã����򷵻�1
inline double QueryLinesIntersectionPos(const Point &a, const Point &b, const Point &c, const Point &d)
{
	if (dblcmp(Cross(b - a, d - c)) <= 0) return 1;
	//�����
	double S1 = Cross(a, c, d);
	double S2 = Cross(d, c, b);
	double ans = S1 / (S1 + S2);
	if (dblcmp(ans) < 0) return 1;
	return ans;
}


//��s��e�������ϵ��߽�ľ��루����ab�ı���Ϊ��λ��������res
void Game::QueryBoardDistance(const Point & s, const Point & e, const Point p[4], double res[4])
{
	for (int i = 0;i < 4;i++) {
		res[i] = QueryLinesIntersectionPos(s, e, p[i], p[(i+1)%4]);
	}

}

//��s��e�������ϵ��߽�ľ��루����ab�ı���Ϊ��λ���е���Сֵ
double Game::QueryBoardDistance(const Point & s, const Point & e, const Point p[4])
{
	double res[4];
	QueryBoardDistance(s, e, p, res);
	return std::min(std::min(res[0], res[1]), std::min(res[2], res[3]));
}

//��������s�ƶ���e�Ƿ��ײ������
//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
//������ײ��������ͱ����Ӱ�캯��
std::tuple<double, std::function<void(Point&)>> Game::TestMalletCollisionWithPuck(const Point & s, const Point & e)
{
	double puckDistance = QueryLineCircleIntersectionPos(s, e, _puckPos, G_malletRadius + G_puckRadius);
	if (dblcmp(puckDistance - 1) < 0) {
		//����λ�ñ仯����ײλ��
		Point malletPos = s + (e - s) * puckDistance;
		//�����ܵ�����
		Point puckDirection = _puckDirection;
		if (QueryLineCircleIntersectionPos(_puckPos, _puckPos + puckDirection, malletPos, G_malletRadius + G_puckRadius)) {
			puckDirection += e - s;
			puckDirection /= abs(puckDirection);
		}

		return std::make_tuple(puckDistance, [=](Point &_malletPos) {
			_malletPos = malletPos;
			_puckDirection = puckDirection;
		});
		
	}
	return std::make_tuple(1, std::function<void(Point&)>());
}

//��������s�ƶ���e�Ƿ��ײ���߽�
//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
//������ײ���������Ӱ�캯��
std::tuple<double, std::function<void(Point&)>> Game::TestMalletCollisionWithBoard(const Point & s, const Point & e, const Point p[4])
{
	double boardDistance = QueryBoardDistance(s, e, p);
	if (dblcmp(boardDistance - 1) < 0) {
		//�����磬�����ؽ���
		Point malletPos = e;
		if (malletPos.x() < p[0].x()) malletPos += p[0].x() - malletPos.x();
		if (malletPos.y() < p[0].y()) malletPos += Point(0, p[0].y() - malletPos.y());
		if (malletPos.x() > p[2].x()) malletPos += p[2].x() - malletPos.x();
		if (malletPos.y() > p[2].y()) malletPos += Point(0, p[2].y() - malletPos.y());
		return std::make_tuple(boardDistance, [=](Point &_malletPos) {
			_malletPos = malletPos;
		});
	}
	return std::make_tuple(1, std::function<void(Point&)>());
}

//�ƶ�������s��e��pΪ�߽磬malletPosΪ�ƶ��������λ�ã����������
void Game::MoveMallet(const Point & s, const Point & e, const Point p[4], Point &malletPos)
{
	//�����������򲻹�
	if (norm(s - e) < G_eps * G_eps) return;

	Point ee;
	//�����߽罻��
	//�Ƚ������Ŀ������������
	auto boardDistance = TestMalletCollisionWithBoard(s, e, p);
	if (dblcmp(std::get<0>(boardDistance) - 1) < 0) {
		std::get<1>(boardDistance)(ee);
	} else {
		ee = e;
	}

	//Mallet��Puck����
	auto puckDistance = TestMalletCollisionWithPuck(s, ee);
	if (dblcmp(std::get<0>(puckDistance) - 1) < 0) {
		std::get<1>(puckDistance)(malletPos);
	} else {
		malletPos = ee;
	}

}

//������s�ƶ���e�Ƿ��ײ���߽�
//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
//������ײ��Ա����Ӱ�졢ʤ�����жϺ���
std::tuple<double, std::function<void()>> Game::TestPuckCollisionWithBoard(const Point & s, const Point & e, const Point p[4])
{
	double res[4];
	QueryBoardDistance(s, e, p, res);
	int minPos = 0;
	double minVal = res[0];
	for (int i = 1; i < 4;i++) {
		if (minVal > res[i]) minVal = res[i], minPos = i;
	}

	//s��e����ײ
	if (dblcmp(minVal - 1) >= 0) {
		return std::make_tuple(1, std::function<void()>());
	}
	
	//���㷴��
	Point reflectVec = p[(minPos + 1) % 4] - p[minPos];
	reflectVec /= abs(reflectVec);
	Point puckDirection = reflectVec * reflectVec / _puckDirection;
	Point puckPos = s + (e - s) * minVal;

	return std::make_tuple(minVal, [=]() {
		_puckDirection = puckDirection;
		_puckPos = puckPos;
		//��߽���ײ���ʤ��
		if (minPos == 1 || minPos == 3) TestWin(minPos);
	}
	);
}


//������s�ƶ���e�Ƿ��ײ������
//������ײ���루����se�ı���Ϊ��λ����������ײ����1��
//������ײ��Ա����Ӱ�캯��
std::tuple<double, std::function<void()>> Game::TestPuckCollisionWithMallet(const Point & s, const Point & e, const Point & o)
{
	double malletDistance = QueryLineCircleIntersectionPos(s, e, o, G_malletRadius + G_puckRadius);
	if (dblcmp(malletDistance-1) < 0) {
		//���㷴��
		Point puckPos = s + (e - s) * malletDistance;
		Point reflectVec = Rotate(o - puckPos);
		reflectVec /= abs(reflectVec);
		Point puckDirection = reflectVec * reflectVec / _puckDirection;
		return std::make_tuple(malletDistance, [=]() {
			_puckDirection = puckDirection;
			_puckPos = puckPos;
		});
	}
	return std::make_tuple(1, std::function<void()>());
}

//�ƶ�����������ģ������ʱ��
//������ײ��������ײ��������ײʱ��
//����ײ��������ʱ��
double Game::MovePuck(double maxTime)
{
	Point s = _puckPos;
	Point e = _puckPos + _puckDirection * maxTime * G_puckSpeed;

	//Puck��Mallet����
	auto mallet1Distance = TestPuckCollisionWithMallet(s, e, _playerPos);
	auto mallet2Distance = TestPuckCollisionWithMallet(s,e, _opponentPos);

	//Puck��board����
	auto boardDistance = TestPuckCollisionWithBoard(s, e, _board);

	//���һ��ײ��
	auto minDis = mallet1Distance;
	if (std::get<0>(mallet2Distance) < std::get<0>(minDis)) minDis = mallet2Distance;
	if (std::get<0>(boardDistance) < std::get<0>(minDis)) minDis = boardDistance;
	
	//������ײ�������з��صĺ�����������ײ����
	if (dblcmp(std::get<0>(minDis) - 1) < 0) {
		std::get<1>(minDis)();
		return std::get<0>(minDis) * maxTime;
	}

	_puckPos = e;

	return maxTime;
}

//��ײ�����Ƿ�ʤ��
void Game::TestWin(int pos)
{
	if (abs(_puckPos.y()) + G_puckRadius < G_goalWidth / 2) {
		_gameStatus = pos == 1 ? 2 : 1;
	}
}

Point Game::GetPlayerMalletPosition() const
{
	return _playerPos;
}

Point Game::GetOpponentMalletPosition() const
{
	return _opponentPos;
}

Point Game::GetPuckPosition() const
{
	return _puckPos;
}

//����timesʱ��(ms)����������Ϊָ��x,y)
int Game::Run(int times, double x, double y)
{
	Point playerTarget = Point(x, y);
	//��ȡAI����
	Point opponentTarget = _ai->QueryAction(times);

	/*_puckDirection = Point(-1, 0);
	_puckPos = Point(x, y);*/


	//���ƶ�һ��mallet
	 MoveMallet(_playerPos, playerTarget, _playerBoard, _playerPos);
	 MoveMallet(_opponentPos, opponentTarget, _opponentBoard, _opponentPos);

	//�ƶ�puck��������ײ���
	double now = 0;
	int calculateCount = 0;
	//�ظ���ײ��������һֱ��ģ�⵽ָ��ʱ�䣬����ײ��������
	while (dblcmp(now - times) < 0 && ++calculateCount < G_calculateMaxTimes) {
		//double passTime = MovePuck(std::min(times - now, C_maxTimeInterval));
		double passTime = MovePuck(times - now);
		now += passTime;

		if (_gameStatus > 0) return _gameStatus;
	}

	//�ٴ��ƶ�mallet
	 MoveMallet(_playerPos, playerTarget, _playerBoard, _playerPos);
	 MoveMallet(_opponentPos, opponentTarget, _opponentBoard, _opponentPos);
	return 0;
}

//���¿�ʼ��Ϸ
void Game::Restart()
{
	//��ʼ��λ�á�״̬
	_gameStatus = 0;
	_playerPos = Point(G_malletStartDistance, 0);
	_opponentPos = Point(-G_malletStartDistance, 0);
	_puckPos = Point(0, 0);

	//�������Ƕ�
	srand((int)time(0));
	_puckDirection = Point(G_tableWidth / 2, rand() * G_tableHeight / RAND_MAX - G_tableHeight / 2);
	_puckDirection /= abs(_puckDirection);
	if (rand() & 1) _puckDirection = -_puckDirection;

	//��������AI
	if (_ai) delete _ai;
	if(_aiLevel  == 0)
		_ai = new SimpleAI(this);
	else if(_aiLevel == 1)
		_ai = new NormalAI(this);
	else if (_aiLevel == 2)
		_ai = new CrazyAI(this);
}


//��AI����·��Ѳ��
Point SimpleAI::QueryAction(int times)
{
	//std::cerr << times << " " << _nowStep << std::endl;
	_nowStep += times;
	if ((_nowStep / C_step) & 1) {
		return C_s + (C_e - C_s) * (double)(_nowStep % C_step) / (double)C_step;
	}else {
		return C_e + (C_s - C_e) * (double)(_nowStep % C_step) / (double)C_step;
	}
}

//��ͨAI��������ƶ�
Point NormalAI::QueryAction(int times)
{
	Point mallet = _game->GetOpponentMalletPosition();
	Point puck = _game->GetPuckPosition();

	//�����򱻱Ƶ����䣬���⿨�������������ƶ�
	if(abs(puck - _game->_board[0]) < G_malletRadius * 2 || 
		abs(puck - _game->_board[3]) < G_malletRadius * 2)
		puck = Point(-G_tableWidth/2, 0);

	//��̫�ӽ��򲻱�
	if (abs(mallet - puck) < G_eps) return mallet;

	return mallet + (puck - mallet) / abs(puck - mallet) * (double)times * C_speed;
}

//���AI��������·�ж�
Point CrazyAI::QueryAction(int times)
{
	Point mallet = _game->GetOpponentMalletPosition();
	Point puck = _game->GetPuckPosition();
	Point target;
	
	double speed = C_speed;

	
	if (puck.x() < -G_tableWidth / 6 - G_malletRadius) {
		//��������AI�ܻ�ķ�Χ��
		
		Point goal = Point(-G_tableWidth/2 + G_malletRadius, 0);//AI����������λ��

		if (_game->_puckDirection.x() > 0) {
			//�����������ƶ�
			if (abs(puck - goal) < abs(mallet - goal)) {
				//���AI�������ȱ���������Զ����㿪�����⵲ס����
				Point a = puck;
				Point b = puck + _game->_puckDirection;
				if (Cross(a, b, mallet) < 0) target = mallet + Rotate(b - a);
				else target = mallet + Rotate(a - b);
				//std::cerr << "4" << std::endl;
			} else {
				//���������ײȥ���ٶȸ�Ϊ����
				target = puck;
				speed = C_highSpeed;
				//std::cerr << "3" << std::endl;
			}
		} else {
			//�����������˶�
			if (abs(puck - goal) > abs(mallet - goal)) {	
				//���AI�������ȱ��������Ž���������Ž��з��أ��ٶȸ�Ϊ����
				//std::cerr << "1" << std::endl;
				target = goal;
				speed = C_highSpeed;
			} else {
				//����������˶������⵲ס�����ı���
				//std::cerr << "2" << std::endl;
				target = mallet + (mallet - puck) * 10.;
				speed = C_slowSpeed;
				/*if (abs(puck - _game->_board[0]) < G_malletRadius * 2 ||
					abs(puck - _game->_board[3]) < G_malletRadius * 2)
					puck = Point(-G_tableWidth, 0);*/
			}
		}

	} else {
		//std::cerr << "5" << std::endl;
		//��������AI�ܻ�ķ�Χ��

		//�жϱ�����·�뼺�����ߵĽ���
		Point p = QueryLinesIntersection(puck, puck + _game->_puckDirection, _game->_board[0], _game->_board[3]);
		if (QueryPointInLine(puck, puck + _game->_puckDirection, p) > 0) {
			double y = p.y();
			//�������һ�η�����λ��
			if (y > _game->_board[3].y()) y = _game->_board[3].y() * 2 - y;
			if (y < _game->_board[0].y()) y = _game->_board[0].y() * 2 - y;
			//������ܽ����ٶȸ�Ϊ����
			if (y > -G_goalWidth / 2 && y < G_goalWidth / 2) speed = C_superSpeed;
		}

		//������ƶ�
		target = puck;
	}

	if (abs(mallet - target) < times * speed) return target;

	return mallet + (target - mallet) / abs(target - mallet) * (double)times * speed;
}