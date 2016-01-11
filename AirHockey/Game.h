#pragma once

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
		std::complex<double>::operator=(*p);
		return *this;
	}
};


class Game
{
private:
	const double C_malletStartDistance = 1;

	Point _playerPos, _opponentPos;
	Point _puckPos, _puckDirection;

public:
	Game() {
		Restart();
	}
	~Game() {};

	Point GetPlayerMalletPosition() const;
	Point GetOpponentMalletPosition() const;
	Point GetPuckPosition() const;

	//���룺����ʱ�䣬�������x,y
	//���أ�״̬ ����0��Ӯ1����2
	int Run(int times, double x, double y);

	void Restart();
};