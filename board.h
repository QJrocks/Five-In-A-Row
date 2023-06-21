#pragma once
#include <stdint.h>
#include <vector>
#include "visualResource.h"

enum BoardSpace {
	BS_Empty = 0,
	BS_P1 = 1,
	BS_P2 = 2,
	BS_Wall = 3
};

enum BoardAnalysisScoreType {
	BAST_None = 0,
	BAST_One,
	BAST_OneInRow,
	BAST_Two,
	BAST_TwoInRow,
	BAST_Three,
	BAST_ThreeInRow,
	BAST_Four,
	BAST_FourInRow,
	BAST_FiveInARow,

	BAST_ThreeThree,
	BAST_FourThree,
	BAST_FourFour
};

struct BoardAnalysisRow {
	BoardSpace line[9];
	BoardAnalysisScoreType p1;
	BoardAnalysisScoreType p2;
};

struct BoardRowSet {
	BoardAnalysisRow hRow;
	BoardAnalysisRow vRow;
	BoardAnalysisRow ddRow;
	BoardAnalysisRow duRow;
};

struct BoardAnalysisScore {
	int p1score;
	int p2score;

	BoardAnalysisScore operator + (const BoardAnalysisScore& rhs) {
		return { p1score + rhs.p1score, p2score + rhs.p2score };
	}
};

struct BoardAI {
	int weights[13];
	int dangerThreshold; //uses defense instead of offense multiplier when highest opponent score is above this threshold
	float offenseMultiplier;
	float defenseMultiplier;

	int decisionX;
	int decisionY;
};

class Board {
private:
	const int boardSize = 15;
	std::vector<BoardSpace> gameBoard;
	int cursorX;
	int cursorY;

	BoardAI defaultAI = {{ 0, 1, 2, 2, 7, 4, 30, 20, 70, 1000, 10, 15, 20 }, 45, 1.2f, 0.2f };

	int playerTurn;
	int inputBlockTimer; //used for animations between turns

	visualResource boardVis;
	visualResource wallVis;
	visualResource P1Vis;
	visualResource P2Vis;
	visualResource cursorVis;

	sf::Text dispValText;
	sf::Font weightDisplayFont;

	sf::Text flashyText;
	int flashDispTimer;

	int positionToBoardIndex(int X, int Y);
	BoardSpace getSpaceType(int X, int Y);

	std::vector<BoardAnalysisScore> boardValues;
	BoardRowSet generateRows(int x, int y);
	BoardAnalysisScoreType calculateCombos(BoardAnalysisScoreType b1, BoardAnalysisScoreType b2, BoardAnalysisScoreType b3, BoardAnalysisScoreType b4);
	void analyzeLine(BoardAnalysisRow& r);
	BoardAnalysisScoreType analyzeLineLabel(BoardAnalysisRow& r, BoardSpace player);
	BoardAnalysisScore analyzeSpace(int x, int y, const int weights[], bool printWeights = false);
	void analyzeBoard(BoardAI& analysisAI);

	int getHighestScoreP1();
	int getHighestScoreP2();

public:
	Board();

	void clearBoard(int layout);

	void moveCursor(int X, int Y);

	void placePiece(BoardSpace type, BoardAI* AI = nullptr);

	void update();

	void draw(sf::RenderTarget& canvas);
};