#pragma once

/* ��Ϸ������ */
enum WinModeType {
	WIN_MAIN_WIN, // ������
	WIN_LEVEL_CHOOSE, // �Ѷ�ѡ��
	WIN_GAME, // ��Ϸ����
	WIN_HELP, // ����
	WIN_GOAL, // �÷ֽ���
	WIN_END // ��Ϸ��������
};

class WinCtrl
{
public:
	/* ���ص�ǰ��Ϸ������ */
	WinModeType mode() const;
	/* �������ѡ��������꣬����ѡ�еİ�ť��-1��ʾδѡ���κΰ�ť */
	int getChoose(double x, double y) const;
	/* ѡ��ĳ��Ű�ť */
	void choose(int label);
	/* ��ȡ��ǰ�ȷ� */
	void getScore(int &player1, int &player2) const;
	/* ���õ÷ַ� */
	void setGoal(int player);
public:
	WinCtrl();
private:
	WinModeType _mode;
	int _score1, _score2;
};