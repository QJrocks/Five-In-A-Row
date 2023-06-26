#pragma once
#include <stdint.h>
#include <vector>
#include "visualResource.h"

//enum for the various states any space of the board can be taken: if it's empty, walled, or taken by a piece.
enum BoardSpace {
	BS_Empty = 0,
	BS_P1 = 1,
	BS_P2 = 2,
	BS_Wall = 3
};

//used for the AI analysis of the board. A list of all relevant formations that can be created in the Five in a Row game.
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

//Struct that holds the result of a single analyzed row.
//Holds the data for 9 spaces in a line (the center and 4 on each side),
//and the most relevant formation for each player.
struct BoardAnalysisRow {
	BoardSpace line[9];
	BoardAnalysisScoreType p1;
	BoardAnalysisScoreType p2;
};

//Struct that holds the complete analysis of a single space on the board. Holds 4 different rows: horizontal, vertical, and both diagonals.
struct BoardRowSet {
	BoardAnalysisRow hRow;
	BoardAnalysisRow vRow;
	BoardAnalysisRow ddRow;
	BoardAnalysisRow duRow;
};

//Holds the resulting score of a given space, where higher values indicate that the space holds greater value to capture.
struct BoardAnalysisScore {
	int p1score;
	int p2score;

	BoardAnalysisScore operator + (const BoardAnalysisScore& rhs) {
		return { p1score + rhs.p1score, p2score + rhs.p2score };
	}
};

//Set of parameters for the AI. The array of weight indicates how much value should be assigned to each formation.
//(Matches up to the list of BAST enums)
//The AI will multiply its projected score for taking a piece by either the offense or defense multiplier.
//The multiplier is decided by the highest projected score of the opposing player, defaulting to the offense multiplier.
//If the score exceeds the danger threshold, the AI will use the defense multiplier.
struct BoardAI {
	int weights[13];
	int dangerThreshold;
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

	sf::View windowView;
	sf::View boardView;

	sf::Vector2f viewScaleDifference;

public:
	Board();

	void prepareViews(sf::View wView, sf::View bView);

	void clearBoard(int layout);

	void moveCursor(int X, int Y);

	void placePiece(BoardSpace type, BoardAI* AI = nullptr);

	void update();

	void draw(sf::RenderTarget& canvas);
};