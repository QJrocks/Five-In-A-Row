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
	//Basic structures
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

	//Combo structures
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
//Mercy chance is the percent chance as to whether the AI will pass up a winning move.
struct BoardAI {
	int weights[13];
	int dangerThreshold;
	float offenseMultiplier;
	float defenseMultiplier;
	float mercyChance;

	int decisionX;
	int decisionY;
};

class Board {
private:
	//Board array things
	const int boardSize = 15;
	std::vector<BoardSpace> gameBoard;
	std::vector<BoardAnalysisScore> boardScoreValues;

	//Cursor location
	int cursorX;
	int cursorY;

	//AIs and their weights.
	//The easy AI mainly acts more recklessly when under attack, weighs combo structures less, and has a high mercy chance.
	BoardAI defaultAI = {{ 0, 1, 2, 2, 7, 4, 30, 20, 70, 1000, 10, 15, 20 }, 45, 1.2f, 0.2f, 0.05f };
	BoardAI easyAI = {{ 0, 1, 2, 2, 7, 4, 30, 20, 70, 1000, 5, 5, 5 }, 55, 1.6f, 0.8f, 0.6f };

	//Pointers to keep track of which AI each player is using. A null pointer means the player is user-controlled.
	BoardAI* p1AI;
	BoardAI* p2AI;

	int playerTurn;
	int inputBlockTimer; //used for animations between turns

	//Resources for rendering all of the board parts
	visualResource boardVis;
	visualResource wallVis;
	visualResource P1Vis;
	visualResource P2Vis;
	visualResource cursorVis;

	//Resources for displaying the text
	sf::Font weightDisplayFont;
	sf::Text dispValText;
	sf::Text flashyText;
	int flashDispTimer;
	
	//Internal helper functions

	//Basic 2D->1D index map
	int positionToBoardIndex(int X, int Y);
	//Retrieves the current contents of a given space coordinates
	BoardSpace getSpaceType(int X, int Y);
	//Generates a BoardRowSet for the given space
	BoardRowSet generateRows(int x, int y);
	//Takes a set of board scores and determines if any combo structures exist
	BoardAnalysisScoreType calculateCombos(BoardAnalysisScoreType b1, BoardAnalysisScoreType b2, BoardAnalysisScoreType b3, BoardAnalysisScoreType b4);
	//Analyzes a single line for score. The resulting values are written to the given BoardAnalysisRow's score variables.
	void analyzeLine(BoardAnalysisRow& r);
	//Analyzes a single line, but acting as if air between friendly pieces is removed. Used for detecting when to show certain texts on screen.
	BoardAnalysisScoreType analyzeLineLabel(BoardAnalysisRow& r, BoardSpace player);
	//Analyzes a space in full using the above helped functions, giving it a score based on the given weights
	BoardAnalysisScore analyzeSpace(int x, int y, const int weights[], bool printWeights = false);
	//Analyzes the entire board, space-by-space, and stores the results into boardScoreValues
	void analyzeBoard(BoardAI& analysisAI);

	//Gets the highest score for a given player
	int getHighestScoreP1();
	int getHighestScoreP2();

	//Views for rendering UI and board objects at different zoom levels
	sf::View windowView;
	sf::View boardView;

	//Used to align UI objects to board spaces
	sf::Vector2f viewScaleDifference;

public:
	Board();

	//Copies the given values to windowView and boardView above.
	void prepareViews(sf::View wView, sf::View bView);

	//Clears the board, resets the turn, and sets up a specific wall layout based off the number given.
	void clearBoard(int layout);

	//Moves the cursor a certain amount of spaces relative to its current position, by the X and Y given.
	//Any movement that would move off the edge of the board will wrap around.
	void moveCursor(int X, int Y);

	//Places a piece of the given type, using a given AI's values.
	//If the AI is valid, the piece is placed at the AI's decision variable coordinates.
	//If the AI is null, place the piece at the cursor location.
	void placePiece(BoardSpace type, BoardAI* AI = nullptr);

	//Called every frame, for things that need updating every frame.
	void update();

	//Renders everything.
	void draw(sf::RenderTarget& canvas);
};