#include "board.h"
#include <iostream>
#pragma warning(disable:26812) //prefer "Enum Class" over "Enum"

inline int Board::positionToBoardIndex(int X, int Y) {
	return Y * boardSize + X;
}

BoardSpace Board::getSpaceType(int X, int Y) {
	if (X < 0 || X >= boardSize || Y < 0 || Y >= boardSize) {
		return BS_Wall;
	}
	return gameBoard[positionToBoardIndex(X, Y)];
}

struct rowFragment {
	int length; //total amount of player pieces in fragment
	int airBufferBegin;
	int airBufferEnd;
	bool continuous;

	rowFragment(bool airBegin = false) {
		length = 0;
		airBufferBegin = airBegin ? 1 : 0;
		airBufferEnd = 0;
		continuous = true;
	}
};

BoardAnalysisScoreType getTypeSingleRow(int length, bool airBegin, bool airEnd, bool continuous) {
	if (length > 5) {
		length = 5;
	}

	if (!airBegin && !airEnd) {
		if (length == 5) {
			return BAST_FiveInARow;
		}
		return BAST_None;
	}

	bool isRow = airBegin && airEnd && continuous;

	switch (length) {
	case 5: return BAST_FiveInARow;
	case 4: return isRow ? BAST_FourInRow : BAST_Four;
	case 3: return isRow ? BAST_ThreeInRow : BAST_Three;
	case 2: return isRow ? BAST_TwoInRow : BAST_Two;
	case 1: return isRow ? BAST_OneInRow : BAST_One;
	}
	return BAST_None;
}

inline int calcRowScore(const rowFragment& r) {
	if (r.airBufferBegin == 0 && r.airBufferEnd == 0) {
		if (r.length == 0) {
			return 0;
		}
		if (r.length >= 5) {
			return 8;
		}
		return -1;
	}
	if (r.airBufferBegin > 0 && r.airBufferEnd > 0) {
		return r.length + 1;
	}
	else {
		return r.length;
	}
}

void Board::analyzeLine(BoardAnalysisRow& r) {
	std::vector<rowFragment> p1fragments;
	std::vector<rowFragment> p2fragments;

	p1fragments.emplace_back();
	p2fragments.emplace_back();

	rowFragment& p1c = p1fragments[p1fragments.size() - 1];
	rowFragment& p2c = p2fragments[p2fragments.size() - 1];

	for (int x = 0; x < 9; x++) {
		if (x == 4) { //middle of row, assume player placed
			p1c.length++;
			p2c.length++;
			if (p1c.airBufferEnd > 0) {
				p1c.continuous = false;
			}
			if (p2c.airBufferEnd > 0) {
				p2c.continuous = false;
			}
		}
		else {
			switch (r.line[x]) {
			case BS_Empty:
				if (p1c.length == 0) {
					p1c.airBufferBegin++;
				}
				else {
					p1c.airBufferEnd++;
					if (p1c.airBufferEnd >= 2) {
						p1fragments.emplace_back();
						p1c = p1fragments[p1fragments.size() - 1];
					}
				}
				if (p2c.length == 0) {
					p2c.airBufferBegin++;
				}
				else {
					p2c.airBufferEnd++;
					if (p2c.airBufferEnd >= 2) {
						p2fragments.emplace_back();
						p2c = p2fragments[p2fragments.size() - 1];
					}
				}
				break;
			case BS_P1:
				p1c.length++;
				if (p1c.airBufferEnd > 0) {
					p1c.continuous = false;
				}
				p1c.airBufferEnd = 0;
				if (p2c.length > 0) {
					p2fragments.emplace_back();
					p2c = p2fragments[p2fragments.size() - 1];
				}
				break;
			case BS_P2:
				p2c.length++; 
				if (p2c.airBufferEnd > 0) {
					p2c.continuous = false;
				}
				p2c.airBufferEnd = 0;
				if (p1c.length > 0) {
					p1fragments.emplace_back();
					p1c = p1fragments[p1fragments.size() - 1];
				}
				break;
			case BS_Wall:
				if (p1c.length > 0) {
					p1fragments.emplace_back();
					p1c = p1fragments[p1fragments.size() - 1];
				}
				if (p2c.length > 0) {
					p2fragments.emplace_back();
					p2c = p2fragments[p2fragments.size() - 1];
				}
				break;
			}
		}
	}

	std::sort(p1fragments.begin(), p1fragments.end(), [](const rowFragment lhs, const rowFragment rhs) {
		return calcRowScore(lhs) > calcRowScore(rhs);
	});
	std::sort(p2fragments.begin(), p2fragments.end(), [](const rowFragment lhs, const rowFragment rhs) {
		return calcRowScore(lhs) > calcRowScore(rhs);
	});

	r.p1 = getTypeSingleRow(p1fragments[0].length, p1fragments[0].airBufferBegin > 0, p1fragments[0].airBufferEnd > 0, p1fragments[0].continuous);
	r.p2 = getTypeSingleRow(p2fragments[0].length, p2fragments[0].airBufferBegin > 0, p2fragments[0].airBufferEnd > 0, p2fragments[0].continuous);
}

BoardAnalysisScoreType Board::analyzeLineLabel(BoardAnalysisRow& r, BoardSpace player) {
	int maxCount = 0;
	bool airBegin = false;
	bool airEnd = false;
	bool continuous = true;

	std::vector<rowFragment> rf;

	for (int x = 0; x < 9; x++) {
		if (r.line[x] == BS_Empty) {
			if (maxCount == 0) {
				airBegin = true;
			}
			else if (airEnd) {
				rowFragment temp;
				temp.length = maxCount;
				temp.airBufferBegin = airBegin;
				temp.airBufferEnd = airEnd;
				temp.continuous = continuous;
				rf.push_back(temp);
				maxCount = 0;
				airBegin = false;
				airEnd = false;
				continuous = true;
			}
			else {
				airEnd = true;
			}
		}
		else if (r.line[x] == player) {
			maxCount++;
			if (airEnd) {
				continuous = false;
			}
			airEnd = false;
		}
		else {
			rowFragment temp;
			temp.length = maxCount;
			temp.airBufferBegin = airBegin;
			temp.airBufferEnd = airEnd;
			temp.continuous = continuous;
			rf.push_back(temp);
			maxCount = 0;
			airBegin = false;
			airEnd = false;
			continuous = true;
		}
	}

	rowFragment temp;
	temp.length = maxCount;
	temp.airBufferBegin = airBegin;
	temp.airBufferEnd = airEnd;
	rf.push_back(temp);

	std::sort(rf.begin(), rf.end(), [](const rowFragment& lhs, const rowFragment& rhs) {
		return lhs.length > rhs.length;
		});

	int trueLen = rf[0].length;
	bool row = rf[0].airBufferBegin > 0 && rf[0].airBufferEnd > 0;
	bool halfRow = rf[0].airBufferBegin > 0 || rf[0].airBufferEnd > 0;
	if (trueLen >= 5 && rf[0].continuous) {
		return BAST_FiveInARow;
	}
	if (trueLen == 4 && row && rf[0].continuous) {
		return BAST_FourInRow;
	}
	if (trueLen >= 4 && halfRow) {
		return BAST_Four;
	}
	if (trueLen == 3 && row) {
		return BAST_ThreeInRow;
	}
	return BAST_None;
}

BoardRowSet Board::generateRows(int x, int y) {
	BoardAnalysisRow hRow, vRow, ddRow, duRow;
	/*
	hRow  -
	vRow  |
	ddRow \
	duRow /
	*/

	for (int i = 0; i < 9; i++) {
		hRow.line[i] = getSpaceType(x + (i - 4), y);
		vRow.line[i] = getSpaceType(x, y + (i - 4));
		ddRow.line[i] = getSpaceType(x + (i - 4), y + (i - 4));
		duRow.line[i] = getSpaceType(x + (i - 4), y + (4 - i));
	}

	return { hRow, vRow, ddRow, duRow };
}

BoardAnalysisScoreType Board::calculateCombos(BoardAnalysisScoreType b1, BoardAnalysisScoreType b2, BoardAnalysisScoreType b3, BoardAnalysisScoreType b4) {
	int fiveCount = 0;
	int fourCount = 0;
	int threeCount = 0;
	bool fourInRow = false;

	switch (b1) {
	case BAST_FiveInARow: fiveCount++; break;
	case BAST_FourInRow: fourCount++; fourInRow = true; break;
	case BAST_Four: fourCount++; break;
	case BAST_ThreeInRow:  threeCount++; break; //don't use base 3
	}
	switch (b2) {
	case BAST_FiveInARow: fiveCount++; break;
	case BAST_FourInRow: fourCount++; fourInRow = true; break;
	case BAST_Four: fourCount++; break;
	case BAST_ThreeInRow:  threeCount++; break; //don't use base 3
	}
	switch (b3) {
	case BAST_FiveInARow: fiveCount++; break;
	case BAST_FourInRow: fourCount++; fourInRow = true; break;
	case BAST_Four: fourCount++; break;
	case BAST_ThreeInRow:  threeCount++; break; //don't use base 3
	}
	switch (b4) {
	case BAST_FiveInARow: fiveCount++; break;
	case BAST_FourInRow: fourCount++; fourInRow = true; break;
	case BAST_Four: fourCount++; break;
	case BAST_ThreeInRow:  threeCount++; break; //don't use base 3
	}

	if (fiveCount > 0) {
		return BAST_FiveInARow;
	}
	else if (fourCount >= 2) {
		return BAST_FourFour;
	}
	else if (fourCount >= 1 && threeCount >= 1) {
		return BAST_FourThree;
	}
	else if (threeCount >= 2) {
		return BAST_ThreeThree;
	}
	else if (fourCount == 1 && fourInRow) {
		return BAST_FourInRow;
	}
	else if (fourCount == 1) {
		return BAST_Four;
	}
	else if (threeCount == 1) {
		return BAST_ThreeInRow;
	}

	return BAST_None;
}

BoardAnalysisScore Board::analyzeSpace(int x, int y, const int weights[], bool printWeights) {
	BoardRowSet brs = generateRows(x, y);

	analyzeLine(brs.hRow);
	analyzeLine(brs.vRow);
	analyzeLine(brs.ddRow);
	analyzeLine(brs.duRow);

	BoardAnalysisScoreType p1combo = calculateCombos(brs.hRow.p1, brs.vRow.p1, brs.ddRow.p1, brs.duRow.p1);
	BoardAnalysisScoreType p2combo = calculateCombos(brs.hRow.p2, brs.vRow.p2, brs.ddRow.p2, brs.duRow.p2);

	int p1bonus = 0, p2bonus = 0;

	switch (p1combo) { case BAST_FourFour: case BAST_FourThree: case BAST_ThreeThree: p1bonus = weights[p1combo]; break; }
	switch (p2combo) { case BAST_FourFour: case BAST_FourThree: case BAST_ThreeThree: p2bonus = weights[p2combo]; break; }

	if (printWeights) {
		std::cout << "-------------------------------" << std::endl;
		std::cout << "P1 - " << weights[brs.hRow.p1] << std::endl;
		std::cout << "P1 | " << weights[brs.vRow.p1] << std::endl;
		std::cout << "P1 \\ " << weights[brs.ddRow.p1] << std::endl;
		std::cout << "P1 / " << weights[brs.duRow.p1] << std::endl;
		std::cout << "P1 Combo " << p1bonus;
		std::cout << "-------------------------------" << std::endl;
		std::cout << "P2 - " << weights[brs.hRow.p2] << std::endl;
		std::cout << "P2 | " << weights[brs.vRow.p2] << std::endl;
		std::cout << "P2 \\ " << weights[brs.ddRow.p2] << std::endl;
		std::cout << "P2 / " << weights[brs.duRow.p2] << std::endl;
		std::cout << "P2 Combo " << p2bonus;
		std::cout << "-------------------------------" << std::endl;
	}

	return { weights[brs.hRow.p1] + weights[brs.vRow.p1] + weights[brs.ddRow.p1] + weights[brs.duRow.p1] + p1bonus, weights[brs.hRow.p2] + weights[brs.vRow.p2] + weights[brs.ddRow.p2] + weights[brs.duRow.p2] + p2bonus};
}

void Board::analyzeBoard(BoardAI& analysisAI) {
	memset(&boardValues[0], 0, sizeof(BoardAnalysisScore) * boardValues.size());
	for (int x = 0; x < boardSize; x++) {
		for (int y = 0; y < boardSize; y++) {
			if (getSpaceType(x, y) == BS_Empty) {
				boardValues[positionToBoardIndex(x, y)] = analyzeSpace(x, y, analysisAI.weights);
			}
		}
	}
	if (getSpaceType(7, 7) == BS_Empty) {
		boardValues[positionToBoardIndex(7, 7)] = { 1000000, 1000000 }; //make the AI default to center of board
	}

	//now, make the AI pick a space
	int max = -1;
	int highestScoreP1 = getHighestScoreP1();
	float AImult = (highestScoreP1 > analysisAI.dangerThreshold) ? analysisAI.defenseMultiplier : analysisAI.offenseMultiplier;
	for (int x = 0; x < boardSize; x++) {
		for (int y = 0; y < boardSize; y++) {
			if (boardValues[positionToBoardIndex(x, y)].p2score >= analysisAI.weights[BAST_FiveInARow]) { //blood in the water
				analysisAI.decisionX = x;
				analysisAI.decisionY = y;
				return;
			}

			int AIval = (int)(boardValues[positionToBoardIndex(x, y)].p2score * AImult) + boardValues[positionToBoardIndex(x, y)].p1score;
			if (AIval > max) {
				max = AIval;
				analysisAI.decisionX = x;
				analysisAI.decisionY = y;
			}
			else if (AIval == max) {
				if (rand() < RAND_MAX / 4) { //gives some variation
					max = AIval;
					analysisAI.decisionX = x;
					analysisAI.decisionY = y;
				}
			}
		}
	}
}

int Board::getHighestScoreP1() {
	int max = boardValues[0].p1score;
	for (int x = 1; x < boardValues.size(); x++) {
		if (boardValues[x].p1score > max) { max = boardValues[x].p1score; }
	}
	return max;
}

int Board::getHighestScoreP2() {
	int max = boardValues[0].p1score;
	for (int x = 1; x < boardValues.size(); x++) {
		if (boardValues[x].p1score > max) { max = boardValues[x].p1score; }
	}
	return max;
}

Board::Board() {
	gameBoard.resize(boardSize * boardSize);
	boardValues.resize(boardSize * boardSize);

	boardVis = visualResource("Img/board.png");
	wallVis = visualResource("Img/wall.png");
	P1Vis = visualResource("Img/p1piece.png");
	P2Vis = visualResource("Img/p2piece.png");
	cursorVis = visualResource("Img/cursor.png");

	weightDisplayFont.loadFromFile("Roboto-Regular.ttf");
	dispValText.setFont(weightDisplayFont);
	dispValText.setOutlineThickness(1);
	dispValText.setCharacterSize(10);

	flashyText.setFont(weightDisplayFont);
	flashyText.setOutlineThickness(5);

	flashDispTimer = 0;
}

void Board::clearBoard(int layout) {
	memset(&gameBoard[0], BS_Empty, sizeof(BoardSpace) * gameBoard.size());
	//fill things depending on layout here
	switch (layout) {
	case 0:
		break;
	case 1:
		gameBoard[positionToBoardIndex(6, 6)] = BS_Wall;
		gameBoard[positionToBoardIndex(6, 7)] = BS_Wall;
		gameBoard[positionToBoardIndex(6, 8)] = BS_Wall;
		gameBoard[positionToBoardIndex(7, 6)] = BS_Wall;
		gameBoard[positionToBoardIndex(7, 7)] = BS_Wall;
		gameBoard[positionToBoardIndex(7, 8)] = BS_Wall;
		gameBoard[positionToBoardIndex(8, 6)] = BS_Wall;
		gameBoard[positionToBoardIndex(8, 7)] = BS_Wall;
		gameBoard[positionToBoardIndex(8, 8)] = BS_Wall;
		break;
	}

	analyzeBoard(defaultAI);

	inputBlockTimer = 0;
	flashDispTimer = 0;
}

void Board::moveCursor(int X, int Y) {
	cursorX = (cursorX + X + boardSize) % boardSize;
	cursorY = (cursorY + Y + boardSize) % boardSize;
}

void Board::placePiece(BoardSpace type, BoardAI* AI) {
	int pieceX = AI ? AI->decisionX : cursorX;
	int pieceY = AI ? AI->decisionY : cursorY;

	if (getSpaceType(pieceX, pieceY) == BS_Empty) {
		if (!AI) {
			if (inputBlockTimer > 0) { return; }
			inputBlockTimer = 60;
		}
		gameBoard[positionToBoardIndex(pieceX, pieceY)] = type;

		analyzeBoard(AI ? *AI : defaultAI);

		//Check board for certain formations
		BoardRowSet brs = generateRows(pieceX, pieceY);


		BoardAnalysisScoreType hBAST = analyzeLineLabel(brs.hRow, type);
		BoardAnalysisScoreType vBAST = analyzeLineLabel(brs.vRow, type);
		BoardAnalysisScoreType ddBAST = analyzeLineLabel(brs.ddRow, type);
		BoardAnalysisScoreType duBAST = analyzeLineLabel(brs.duRow, type);

		BoardAnalysisScoreType comboResult = calculateCombos(hBAST, vBAST, ddBAST, duBAST);
		sf::Color labelColor = (type == BS_P1) ? sf::Color::Red : sf::Color::Blue;

		switch (comboResult) {
		case BAST_FiveInARow:
			flashyText.setString("5 IN A ROW");
			flashyText.setFillColor(labelColor);
			flashDispTimer = 10000;
			inputBlockTimer = 10000;
			break;
		case BAST_FourFour:
			flashyText.setString("FOUR FOUR");
			flashyText.setFillColor(labelColor);
			flashDispTimer = 120;
			break;
		case BAST_FourThree:
			flashyText.setString("FOUR THREE");
			flashyText.setFillColor(labelColor);
			flashDispTimer = 120;
			break;
		case BAST_ThreeThree:
			flashyText.setString("THREE THREE");
			flashyText.setFillColor(labelColor);
			flashDispTimer = 120;
			break;
		case BAST_FourInRow:
			flashyText.setString("FOUR IN A ROW");
			flashyText.setFillColor(labelColor);
			flashDispTimer = 120;
			break;
		case BAST_Four:
			flashyText.setString("FOUR");
			flashyText.setFillColor(labelColor);
			flashDispTimer = 40;
			break;
		case BAST_ThreeInRow:
			flashyText.setString("THREE");
			flashyText.setFillColor(labelColor);
			flashDispTimer = 40;
			break;
		default:
			//check for danger here?
			break;
		}
	}
}

void Board::update() {
	if (inputBlockTimer > 0) {
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
			inputBlockTimer--;
		}

		if (inputBlockTimer == 30) {
			placePiece(BS_P2, &defaultAI);
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		analyzeSpace(cursorX, cursorY, defaultAI.weights, true);
	}

	if (flashDispTimer > 0) {
		flashDispTimer--;
	}
}

void Board::draw(sf::RenderTarget& canvas) {
	boardVis.draw(canvas, -8, -8);
	for (int x = 0; x < boardSize; x++) {
		for (int y = 0; y < boardSize; y++) {
			switch (getSpaceType(x, y)) {
			case BS_Empty:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
					dispValText.setString(std::to_string(boardValues[positionToBoardIndex(x, y)].p2score));
				}
				else {
					dispValText.setString(std::to_string(boardValues[positionToBoardIndex(x, y)].p1score));
				}
				dispValText.setPosition((float)(x * 16 + 5), (float)(y * 16 + 2));
				canvas.draw(dispValText);
				break;
			case BS_P1:
				P1Vis.draw(canvas, (float)(x * 16), (float)(y * 16));
				break;
			case BS_P2:
				P2Vis.draw(canvas, (float)(x * 16), (float)(y * 16));
				break;
			case BS_Wall:
				wallVis.draw(canvas, (float)(x * 16), (float)(y * 16));
				break;
			}
		}
	}

	cursorVis.draw(canvas, (float)(cursorX * 16), (float)(cursorY * 16));

	if (flashDispTimer > 0) {
		canvas.draw(flashyText);
	}
}
