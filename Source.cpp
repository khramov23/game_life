#include <windows.h>
#include <conio.h>
#include <iostream>
#include <ctime>
using namespace std;

// ��������� �������
void setcur(int x, int y)//��������� ������� �� �������  x y 
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};

// ��������� ���������� �����
unsigned int random(int tmp)
{
	static unsigned int seed = tmp;
	seed = (8253729 * seed + 2396403);
	return seed % 32768;
}

// ������� ����
enum Rools
{
	rool1, // ����� �����, ���� 2-3 ������, ����� �������. ��������, ���� 3 ������
	rool2, // ����� �����, ���� 2-4 ������, ����� �������. ��������, ���� 3 ������
	rool3  // ����� �����, ���� ������� �� 4 �� 10, ����� �������. �������� �� 9 �� 10.
};

struct iView
{
	char livingCell; // ������ "�����" ������
	char emptyCell; // ������ "�������" ������
	iView(char _livingCell = '#', char _emptyCell = '.') {
		livingCell = _livingCell;
		emptyCell = _emptyCell;
	}
};
struct iGame
{
	virtual void runGame(int numIt, bool active = 0) = 0;
	int seed; // ��������� �������� ��� ����������
	double probability; // ����������� ����, ��� ������ �����
	Rools rools; // ������� ���� (��� ������ ��������� �� ���������� �����/������� �������)
};

struct View2d : iView
{
	char** fieldToDraw;

	View2d(char _livingCell = '#', char _emptyCell = '.') : iView(_livingCell, _emptyCell) {
		bool choise;
		cout << endl << "������ ������� ���������� ����? (1 - ��, 0 - �������� �� ���������): ";
		cin >> choise;
		if (choise) {
			cout << "������ ����� ������: ";
			cin >> livingCell;
			cout << "������ ������ ������: ";
			cin >> emptyCell;
		}
	}

	void setField(char** field, int sizeX, int sizeY) {
		fieldToDraw = new char* [sizeX];
		for (int i = 0; i < sizeX; i++) {
			fieldToDraw[i] = new char[sizeY];
			for (int j = 0; j < sizeY; j++)
				fieldToDraw[i][j] = field[i][j];
		}

	};
	void draw(int sizeX, int sizeY)
	{	
		for (int i = 0; i < sizeX; i++) {
			for (int j = 0; j < sizeY; j++)
				cout << fieldToDraw[i][j] << " ";
			cout << endl;
		}
	}
};
struct Game2d : iGame
{
	char** fieldPresented;
	char** fieldBack;
	int sizeX, sizeY; // ������� �����
	int numIt; // ���������� ���������
	bool active; // �������� ������� ���� ��� �� ��������� � �������� ���������
	View2d view;

	Game2d() {
		int choise;
		cout << endl << "�������� ������� ����(1 ��� 2): " << endl;
		cout << "1) ����� ������ �����, ���� 2-3 ������, ����� �������. ������� �������, ���� 3 ������" << endl;
		cout << "2) ����� ������ �����, ���� 2-4 ������, ����� �������. ������� �������, ���� 3 ������" << endl;
		cin >> choise;
		if (choise - 1 == Rools::rool1) rools = Rools::rool1;
		else if (choise - 1 == Rools::rool2) rools = Rools::rool2;
		else rools = Rools::rool1;
		system("cls");

		cout << "������� ����: ";
		cin >> sizeX >> sizeY;
		cout << "��������� (�� 0 �� 1): ";
		cin >> probability;
		cout << "����� ������������� ����� ��� ��������� ����: ";
		cin >> seed;
		cout << "���������� ���������: ";
		cin >> numIt;
		cout << "�������� ���� ������� ����? (1 - ��, 0 - ���): ";
		cin >> active;
		system("cls");

		fieldPresented = new char* [sizeX];
		fieldBack = new char* [sizeX];

		createField(fieldPresented);
		createEmptyField(fieldBack);
		copyField(fieldBack, fieldPresented);

		view.setField(fieldPresented, sizeX, sizeY);
	}

	// ����������� ����
	void copyField(char** into, char** from) {
		for (int i = 0; i < sizeX; i++)
			for (int j = 0; j < sizeY; j++)
				into[i][j] = from[i][j];
	}

	// ��������� ���������� �������
	int getNeighbours(int x, int y)
	{
		int count = 0;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++) {
				if (x + i >= 0 && x + i <= (sizeX - 1) && y + j >= 0 && y + j <= (sizeY - 1))
					if (fieldBack[x + i][y + j] == view.livingCell)
						count++;
			}
		if (fieldBack[x][y] == view.livingCell) return count - 1;
		else return count;
	}

	// �������� ������� ����
	void createEmptyField(char** fieldName) {
		for (int i = 0; i < sizeX; i++)
			fieldName[i] = new char[sizeY];
	}

	// �������� ����, ������������ ��������
	void createField(char** fieldName) {
		int tmp;
		for (int i = 0; i < sizeX; i++) {
			fieldName[i] = new char[sizeY];
			for (int j = 0; j < sizeY; j++)
			{
				tmp = random(seed) % 10000;
				if (tmp <= 10000 * probability)
					fieldName[i][j] = view.livingCell;
				else
					fieldName[i][j] = view.emptyCell;
			}
		}
	}

	// ������������ ������ ���������
	void newGeneration(Rools rools) {
		int tmp;
		if (rools == rool1) {
			for (int i = 0; i < sizeX; i++) {
				for (int j = 0; j < sizeY; j++) {
					tmp = getNeighbours(i, j);
					if (fieldBack[i][j] == view.livingCell && tmp != 3 && tmp != 2)
						fieldPresented[i][j] = view.emptyCell;
					else if (fieldBack[i][j] == view.emptyCell && tmp == 3)
						fieldPresented[i][j] = view.livingCell;
				}
			}
		}
		else if (rools == rool2) {
			for (int i = 0; i < sizeX; i++) {
				for (int j = 0; j < sizeY; j++) {
					tmp = getNeighbours(i, j);
					if (fieldBack[i][j] == view.livingCell && tmp != 4 && tmp != 3 && tmp != 2)
						fieldPresented[i][j] = view.emptyCell;
					else if (fieldBack[i][j] == view.emptyCell && tmp == 3)
						fieldPresented[i][j] = view.livingCell;
				}
			}
		}
	}

	// ������ ����
	void runGame(int numIt, bool active = 0)
	{
		COORD position;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		position.X = 0;
		position.Y = 0;

		if (active) {
			setcur(0, 0);
			for (int i = 0; i < 5; i++)
				cout << endl;
			view.draw(sizeX, sizeY);
			cout << "------------  Press any button to start  --------------- " << endl;
			_getch();
			system("cls");
			int i = 0;
			while (i++ < numIt) {
				newGeneration(rools);
				copyField(fieldBack, fieldPresented);
				view.setField(fieldPresented, sizeX, sizeY);
				for (int i = 0; i < 5; i++)
					cout << endl;
				view.draw(sizeX, sizeY);
				setcur(0, 0);
			}
		}
		else {
			cout << endl;
			cout << "------------  Press any button to start  --------------- " << endl;
			_getch();
			system("cls");

			cout << "��������� ���������:" << endl;
			view.draw(sizeX, sizeY);

			int i = 0;
			while (i++ < numIt)
			{
				// ������������ ������ ���������
				newGeneration(rools);
				copyField(fieldBack, fieldPresented);
			}
			cout << endl << endl << "�������� ���������:" << endl;
			view.setField(fieldPresented, sizeX, sizeY);
			view.draw(sizeX, sizeY);
		}
	}
};

struct View3d : iView
{
	char*** fieldToDraw;

	View3d(char _livingCell = '#', char _emptyCell = '.') : iView(_livingCell, _emptyCell) {
		bool choise;
		cout << endl << "������ ������� ���������� ����? (1 - ��, 0 - �������� �� ���������): ";
		cin >> choise;
		if (choise) {
			cout << "������ ����� ������: ";
			cin >> livingCell;
			cout << "������ ������ ������: ";
			cin >> emptyCell;
		}
	}

	void setField(char*** field, int sizeX, int sizeY, int sizeZ) {
		fieldToDraw = new char** [sizeX];
		for (int i = 0; i < sizeX; i++) {
			fieldToDraw[i] = new char*[sizeY];
			for (int j = 0; j < sizeY; j++) {
				fieldToDraw[i][j] = new char[sizeZ];
				for (int l = 0; l < sizeZ; l++)
					fieldToDraw[i][j][l] = field[i][j][l];
			}
		}

	};
	void draw(int sizeX, int sizeY, int sizeZ)
	{
		for (int i = 0; i < sizeX; i++)
		{
			for (int l = 0; l < sizeZ; l++)
			{
				for (int j = 0; j < sizeY; j++)
					cout << fieldToDraw[i][j][l] << " ";
				cout << "   ";
			}
			cout << endl;
		}
	}
};
struct Game3d : iGame
{
	View3d view;
	char*** fieldPresented;
	char*** fieldBack;
	int sizeX, sizeY, sizeZ; // ������� �����
	int numIt; // ���������� ���������
	bool active;

	Game3d() {
		int choise;
		cout << endl << "�������� ������� ���� (�����): " << endl;
		cout << "1) ����� �����, ���� 2 - 3 ������, ����� �������.��������, ���� 3 ������" << endl;
		cout << "2) ����� �����, ���� 2 - 4 ������, ����� �������. ��������, ���� 3 ������" << endl;
		cout << "3) ����� �����, ���� ������� �� 4 �� 10, ����� �������. �������� �� 9 �� 10." << endl;

		cin >> choise;
		if (choise - 1 == Rools::rool1) rools = Rools::rool1;
		else if (choise - 1 == Rools::rool2) rools = Rools::rool2;
		else if (choise - 1 == Rools::rool3) rools = Rools::rool3;
		else rools = Rools::rool1;
		system("cls");

		cout << "������� ����: ";
		cin >> sizeX >> sizeY >> sizeZ;
		cout << "��������� (�� 0 �� 1): ";
		cin >> probability;
		cout << "����� ������������� ����� ��� ��������� ����: ";
		cin >> seed;
		cout << "���������� ���������: ";
		cin >> numIt;
		cout << "�������� ���� ������� ����? (1 - ��, 0 - ���)? ";
		cin >> active;
		system("cls");

		fieldPresented = new char** [sizeX];
		fieldBack = new char** [sizeX];

		createField(fieldPresented);
		createEmptyField(fieldBack);
		copyField(fieldBack, fieldPresented);

		view.setField(fieldPresented, sizeX, sizeY, sizeZ);
	}

	// ����������� ����
	void copyField(char*** into, char*** from) {
		for (int i = 0; i < sizeX; i++)
			for (int j = 0; j < sizeY; j++)
				for (int l = 0; l < sizeZ; l++)
					into[i][j][l] = from[i][j][l];
	}

	// ��������� ���������� �������
	int getNeighbours(int x, int y, int z)
	{
		int count = 0;
		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				for (int l = -1; l <= 1; l++)
					if (x + i >= 0 && x + i <= (sizeX - 1) && y + j >= 0 && y + j <= (sizeY - 1) && z + l >= 0 && z + l <= (sizeZ - 1))
						if (fieldBack[x + i][y + j][z + l] == view.livingCell)
							count++;
				
		if (fieldBack[x][y][z] == view.livingCell) return count - 1;
		else return count;
	}

	// �������� ������� ����
	void createEmptyField(char*** fieldName) {
		for (int i = 0; i < sizeX; i++) {
			fieldName[i] = new char* [sizeY];
			for (int j = 0; j < sizeY; j++)
				fieldName[i][j] = new char[sizeZ];
		}
	}

	// �������� ����, ������������ ��������
	void createField(char*** fieldName) {
		int tmp;
		for (int i = 0; i < sizeX; i++) {
			fieldName[i] = new char* [sizeY];
			for (int j = 0; j < sizeY; j++) {
				fieldName[i][j] = new char[sizeY];
				for (int l = 0; l < sizeZ; l++)
				{
					tmp = random(seed) % 10000;
					if (tmp <= 10000 * probability)
						fieldName[i][j][l] = view.livingCell;
					else
						fieldName[i][j][l] = view.emptyCell;
				}
			}
		}
	}

	// ������������ ������ ���������
	void newGeneration(Rools rools) {
		int tmp;
		if (rools == rool1) {
			for (int i = 0; i < sizeX; i++) {
				for (int j = 0; j < sizeY; j++) {
					for (int l = 0; l < sizeZ; l++)
					{
						tmp = getNeighbours(i, j, l);
						if (fieldBack[i][j][l] == view.livingCell && tmp != 3 && tmp != 2)
							fieldPresented[i][j][l] = view.emptyCell;
						else if (fieldBack[i][j][l] == view.emptyCell && tmp == 3)
							fieldPresented[i][j][l] = view.livingCell;
					}
				}
			}
		}
		else if (rools == rool2) {
			for (int i = 0; i < sizeX; i++) {
				for (int j = 0; j < sizeY; j++) {
					for (int l = 0; l < sizeZ; l++)
					{
						tmp = getNeighbours(i, j, l);
						if (fieldBack[i][j][l] == view.livingCell && tmp != 4 &&tmp != 3 && tmp != 2)
							fieldPresented[i][j][l] = view.emptyCell;
						else if (fieldBack[i][j][l] == view.emptyCell && tmp == 3)
							fieldPresented[i][j][l] = view.livingCell;
					}
				}
			}
		}
		else if (rools == rool3) {
			for (int i = 0; i < sizeX; i++) {
				for (int j = 0; j < sizeY; j++) {
					for (int l = 0; l < sizeZ; l++)
					{
						tmp = getNeighbours(i, j, l);
						if (fieldBack[i][j][l] == view.livingCell && (tmp < 4 || tmp > 10))
							fieldPresented[i][j][l] = view.emptyCell;
						else if (fieldBack[i][j][l] == view.emptyCell && (tmp == 9 || tmp == 10))
							fieldPresented[i][j][l] = view.livingCell;
					}
				}
			}
		}
	}

	// ������ ����
	void runGame(int numIt, bool active = 0)
	{
		COORD position;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		position.X = 0;
		position.Y = 0;

		if (active) {
			setcur(0, 0);
			for (int i = 0; i < 5; i++)
				cout << endl;
			view.draw(sizeX, sizeY, sizeZ);
			cout << "------------  Press any button to start  --------------- " << endl;
			_getch();
			system("cls");
			int i = 0;
			while (i++ < numIt) {
				newGeneration(rools);
				copyField(fieldBack, fieldPresented);
				view.setField(fieldPresented, sizeX, sizeY, sizeZ);
				for (int i = 0; i < 5; i++)
					cout << endl;
				view.draw(sizeX, sizeY, sizeZ);
				setcur(0, 0);
			}
		}
		else {
			cout << endl;
			cout << "------------  Press any button to start  --------------- " << endl;
			_getch();
			system("cls");
			cout << "��������� ���������:" << endl;
			view.draw(sizeX, sizeY, sizeZ);
			cout << endl;

			int i = 0;
			while (i++ < numIt)
			{
				// ������������ ������ ���������
				newGeneration(rools);
				copyField(fieldBack, fieldPresented);
			}

			cout << endl << endl << "�������� ���������:" << endl;
			view.setField(fieldPresented, sizeX, sizeY, sizeZ);
			view.draw(sizeX, sizeY, sizeZ);
		}
	}
};


int main() {
	setlocale(LC_ALL, "Rus");
	int choise;

	cout << "���� ����� ����������� ���������? (2 ��� 3): ";
	cin >> choise;

	if (choise == 2) {
		Game2d game;
		game.runGame(game.numIt, game.active);
	}
	else if (choise == 3){
		Game3d game;
		game.runGame(game.numIt, game.active);
	}



	return 0;
}

