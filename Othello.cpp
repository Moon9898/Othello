#include <bangtal>
using namespace bangtal;

ObjectPtr board[8][8];

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8];

enum class Turn {
	BLACK,
	WHITE
};

Turn turn = Turn::BLACK;

void setState(int x, int y, State)
{
	switch (state) {
	case State::BLANK:	board[y][x]->setImage("Images/blank.png");	break;
	case State::POSSIBLE:	board[y][x]->setImage(turn == Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
	case State::BLACK:	board[y][x]->setImage("Images/black.png");	break;
	case State::WHITE:	board[y][x]->setImage("Images/white.png");	break;
	}
	board_state[y][x] = state;
}

bool checkPossible(int x, int y, int dx, int dy)  // x, y는 위치 dx, dy는 이동 방향
{
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;  // ??? 검은 돌이 먼저 할 수 있도록

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {  // ??? 첫번째는 '~부터' 아닌가?
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			return possible;
		}
		else {
			return false;  // blank
		}
	}

	return false;
}

bool checkPossible(int x, int y)
{
	if (board_state[y][x] == State::BLACK) return false;
	if (board_state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK);

	// 여덟 방향으로 POSSIBLE 체크 함수 호출 (8 방향으로 x, y 2개 체크)
	int delta[8][2] = {
		{ 0, 1 },
		{ 1, 1 },
		{ 1, 0 },
		{ 1, -1 },
		{ 0, -1 },
		{ -1, -1 },
		{ -1, 0 },
		{ -1, 1 }
	};

	/*
	for (int i = 0; i < 8; i++) {
		checkPossible(x, y, delta[i][0], delta[i][1]);  //첫번째 delta가 x 증가, 두번째 delta가 y 증가
	}
	*/

	bool possible = false;
	for (auto d : delta) {
		if (checkPossible(x, y, d[0], d[1])) possible = true;  // ???
	}

	return possible;
}

void reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						setState(x, y, self);  // 역방향으로 오다가 상대 돌이 있으면 내 돌로 바꾸기
					}
					else {  // 상대 돌 없으면 내 돌 그대로 두기 (리턴값 없이 리턴, 끝)
						return;
					}
				}
			}
		}

		void reverse(int x, int y)
		{
			int delta[8][2] = {
			{ 0, 1 },
			{ 1, 1 },
			{ 1, 0 },
			{ 1, -1 },
			{ 0, -1 },
			{ -1, -1 },
			{ -1, 0 },
			{ -1, 1 }
			};

			bool possible = false;
			for (auto d : delta) {
				reverse(x, y, d[0], d[1]);
			}
		}

		bool setPossible()
		{
			// 8*8의 모든 위치에 대해서
			// 놓일 수 있으면 상태를 POSSIBLE로 바꾼다
			bool possible = false;
			for (int y = 0; y < 8; y++) {
				for (int x = 0; x < 8; x++) {
					if (checkPossible(x, y)) {
						setState(x, y, State::POSSIBLE);
						possible = true;
					}
				}
			}
			return possible;
		}

		int main() {
			setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);
			setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
			setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);

			auto scene = Scene::create("Othello", "Images/background.png");

			for (int y = 0; y < 8; y++) {  // y = 0부터, y < 8 일 동안, 1씩 커지면서
				for (int x = 0; x < 8; x++) {
					board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
					board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction) -> bool {
						if (board_state[y][x] == State::POSSIBLE) {
							if (turn == Turn::BLACK) {
								setState(x, y, State::BLACK);
								reverse(x, y);		// 잡힌 돌 뒤집기
								turn = Turn::WHITE;
							}
							else {
								setState(x, y, State::WHITE);
								reverse(x, y);
								turn = Turn::BLACK;
							}
							if (!setPossible()) {
								turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;  //	toggle ???

								if (setPossible()) {
									showMessage("End Game!!!");
								}

								return true;
							});
					board_state[y][x] = State::BLANK;
						}

					}

					setState(3, 3, State::BLACK);
					setState(4, 4, State::BLACK);
					setState(3, 4, State::WHITE);
					setState(4, 3, State::WHITE);

					setPossible();

					startGame(scene);
				}

				// 숫자 점수판 넣고 컴퓨터와 플레이하도록.