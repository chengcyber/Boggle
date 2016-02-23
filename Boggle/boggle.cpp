/* File: boggle.cpp
 * ----------------
 * Your name here!
 */
 
#include "genlib.h"
#include "simpio.h"
#include <iostream>
#include "extgraph.h"
#include "gboggle.h"
#include "vector.h"
#include "grid.h"
#include "random.h"
#include "lexicon.h"
#include <string>
#include "strutils.h"

using namespace std;


/* Global variables */
string StandardCubes[16]  = 
{"AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS", "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
 "DISTTY", "EEGHNW", "EEINSU", "EHRTVW", "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"};
 
string BigBoggleCubes[25]  = 
{"AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM", "AEEGMU", "AEGMNN", "AFIRSY", 
"BJKQXZ", "CCNSTW", "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT", "DHHLOR", 
"DHLNOR", "EIIITT", "EMOTTT", "ENSSSU", "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"};

const int MAX_LENGTH = 8;
const int MIN_LENGTH = 4;


/* store point */
struct Point {
	int x;
	int y;
};

/* Direction structure */
const enum Direction {
	TOPLEFT,
	TOP,
	TOPRIGHT,
	RIGHT,
	BOTRIGHT,
	BOT,
	BOTLEFT,
	LEFT
};

/* operator overloadiang */
Direction operator+(Direction & dir) {
	dir = Direction(dir + 1);
	return dir;
}

Direction operator++(Direction & dir) {
	dir = Direction(dir + 1);
	return dir;
}

Direction operator++(Direction & dir, int) {
	Direction old = dir;
	dir = Direction(dir + 1);
	return old;
}

ostream & operator<< (ostream & os, Direction & dir) {
	switch (dir) {
		case TOPLEFT	: return os <<  "TOPLEFT" ;
		case TOP		: return os <<  "TOP" ;	
		case TOPRIGHT	: return os <<  "TOPRIGHT" ;
		case RIGHT		: return os <<  "RIGHT" ;	
		case BOTRIGHT	: return os <<  "BOTRIGHT" ;
		case BOT		: return os <<  "BOT" ;	
		case BOTLEFT	: return os <<  "BOTLEFT" ;
		case LEFT		: return os <<  "LEFT" ;	
		default: return os << "?";
	}

}


/* Function prototypes */

int getBoardSize();
void printStrings(string strArr[], int arrSize);
void loadCubeChar(int gameSize, Grid<char> &grid);
void fillCubeChar(Grid<char> &grid);
Vector<string> sloveBoggle(Grid<char> &grid,Point start, int depth, Vector<Point> &path, string &word, Lexicon &lex);
bool isBound(Grid<char> &grid, int i, int j, Direction &dir);
bool isContainPoint(Vector<Point> &path, Point &pt);
bool isContainWord(Vector<string> &total, string &word);
Point adjustPoint(Point start, Direction dir);
Vector<string> concateVec(Vector<string> &vec1, Vector<string> &vec2);
void printStrVec(Vector<string> &vec);
bool verifyGuess(string &guess, Grid<char> &grid, Lexicon &lex);
Vector<Point> getStartPoints(char ch, Grid<char> &grid);

void GiveInstructions()
{
    cout << endl << "The boggle board is a grid onto which I will randomly distribute " 
	 << "cubes. These 6-sided cubes have letters rather than numbers on the faces, " 
	 << "creating a grid of letters on which you try to form words. You go first, " 
	 << "entering all the words you can find that are formed by tracing adjoining " 
	 << "letters. Two letters adjoin if they are next to each other horizontally, " 
	 << "vertically, or diagonally. A letter can only be used once in the word. Words "
	 << "must be at least 4 letters long and can only be counted once. You score points "
	 << "based on word length: a 4-letter word is worth 1 point, 5-letters earn 2 "
	 << "points, and so on. After your puny brain is exhausted, I, the super computer, "
	 << "will find all the remaining words and double or triple your paltry score." << endl;
	
    cout << "\nHit return when you're ready...";
    GetLine();
}

static void Welcome()
{
    cout << "Welcome!  You're about to play an intense game of mind-numbing Boggle. " 
	 << "The good news is that you might improve your vocabulary a bit.  The "
	 << "bad news is that you're probably going to lose miserably to this little "
	 << "dictionary-toting hunk of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}


int main()
{
	SetWindowSize(9, 5);
	InitGraphics();
	Welcome();
	GiveInstructions();

	//Game Size
	int size = getBoardSize();
	DrawBoard(size,size);

	//fill board
	Grid<char> grid;
	loadCubeChar(size, grid);
	fillCubeChar(grid);

	Lexicon lex("lexicon.dat");

	//Computer turn
	Vector<string> comVec,comVecTotal;
	Vector<Point> path;
	string word;

	//start point ever element
	Point start;
	for(int i = 0; i < grid.numRows(); i++) {
		for (int j = 0; j < grid.numCols(); j++) {
			start.x = i;
			start.y = j;
			word = "";
			word += grid.getAt(start.x, start.y);
			path.clear();
			path.add(start);
			comVec = sloveBoggle(grid, start, 1, path, word, lex);
			comVecTotal = concateVec(comVecTotal, comVec);
		}
	}
	

	cout << "--------total-------" << endl;
	printStrVec(comVecTotal);
	cout << "--------------------" << endl;
	
	// human turn
	string guess;
	while(true) {
		cout << "Give me a word:";
		guess = GetLine();
		if(guess.length() >= MIN_LENGTH && guess.length() <= MAX_LENGTH) break;
		cout << "length invalid! try again." << endl;
	}

	if(verifyGuess(guess, grid, lex)) {
		cout << "true" << endl;
	} else {
		cout << "false" << endl;
	}

	return 0;
}


int getBoardSize(){
	cout << "Input game board size( 4 or 5 ):";
	int n = GetInteger();
	if (n >= 5) n = 5;
	if (n <= 4) n = 4;
	return n;
}

void printStrings(string strArr[], int arrSize) {
	for(int i = 0; i < arrSize; i++) {
		cout << strArr[i] << endl;
	}
}


void loadCubeChar(int gameSize, Grid<char> &grid) {
	grid.resize(gameSize, gameSize);
	if (gameSize == 5) {

	}
	for(int i = 0; i < grid.numRows(); i++) {
		for(int j = 0; j< grid.numCols(); j++) {
			int ranNum = RandomInteger(0, 5);
			if (gameSize == 5) {
				grid.setAt(i, j, BigBoggleCubes[i*gameSize + j][ranNum]);
			} else {
				grid.setAt(i, j, StandardCubes[i*gameSize + j][ranNum]);
			}
		}
	}
}

void fillCubeChar(Grid<char> &grid) {
	for(int i = 0; i< grid.numRows(); i++) {
		for (int j = 0; j < grid.numCols(); j++) {
			LabelCube(i, j, grid.getAt(i, j));
		}
	}
}

Vector<string> sloveBoggle(Grid<char> &grid, Point start, int depth, Vector<Point> &path, string &word, Lexicon &lex) {
	Vector<string> result; 
	if (depth > MAX_LENGTH) {
		// greater than maxium length, return
		// return lex.containsWord(word);
	} else {
		
		//recursion to the element around the current element.
		int i = start.x;
		int j = start.y;
		if(lex.containsWord(word) && depth >= MIN_LENGTH) {
			result.add(word);
		}
		for(Direction dir = TOPLEFT; dir <= LEFT; dir++) {
			if(isBound(grid, i, j, dir)) {
				Point new_start = adjustPoint(start, dir);
				if(isContainPoint(path, new_start)) continue;
				string new_word = word + grid.getAt(new_start.x, new_start.y);
				if(!lex.containsPrefix(new_word)) continue;

				path.add(new_start);
				depth++;
				Vector<string> res = sloveBoggle(grid, new_start, depth, path, new_word, lex);
				result = concateVec(result, res);
				path.removeAt(path.size() - 1);
				depth--;
			}
		}
	} 

	return result;
}


bool isBound(Grid<char> &grid, int i, int j, Direction &dir) {
	bool b_row = true;
	bool b_col = true;
	if(i == 0) {
		switch (dir) {
			case TOP : b_row = false; break;
			case TOPLEFT : b_row = false; break;
			case TOPRIGHT : b_row = false; break;
			default: b_row = true; break;
		}
	}

	if (i == grid.numRows() - 1) {
		switch(dir) {
			case BOT : b_row = false; break;
			case BOTLEFT : b_row = false; break;
			case BOTRIGHT : b_row = false; break;
			default : b_row = true; break;
		}
	}

	if(j == 0) {
		switch (dir) {
			case TOPLEFT : b_col = false; break;
			case LEFT : b_col = false; break;
			case BOTLEFT : b_col = false; break;
			default : b_col = true; break;
		}
	}

	if (j == grid.numCols() - 1) {
		switch (dir) {
			case RIGHT : b_col = false; break;
			case TOPRIGHT : b_col = false; break;
			case BOTRIGHT : b_col = false; break;
			default : b_col = true; break;
		}
	}

	return b_row && b_col;
}

bool isContainPoint(Vector<Point> &path, Point &pt) {
	for(int i = 0; i< path.size(); i++) {
		Point vp = path.getAt(i);
		if ((vp.x == pt.x) && (vp.y == pt.y))
			return true;
	}
	return false;
}


Point adjustPoint(Point start, Direction dir) {
	switch(dir) {
		case TOPLEFT	: start.x -= 1; start.y -= 1; break;
		case TOP		: start.x -= 1; start.y; break;
		case TOPRIGHT	: start.x -= 1; start.y += 1; break;
		case RIGHT	 	: start.x; start.y += 1; break;
		case BOTRIGHT	: start.x += 1; start.y += 1; break;
		case BOT		: start.x += 1; start.y; break;
		case BOTLEFT	: start.x += 1; start.y -= 1; break;
		case LEFT		: start.x; start.y -= 1; break;
		default : break;
	}
	return start;
}

Vector<string> concateVec(Vector<string> &vec1, Vector<string> &vec2) {
	if (vec1.isEmpty()) return vec2;
	if (vec2.isEmpty()) return vec1;
	for(int i = 0; i < vec2.size(); i++) {
		vec1.add(vec2.getAt(i));
	}
	return vec1;
}


void printStrVec(Vector<string> &vec) {
	for(int i = 0; i < vec.size(); i++) {
		cout << vec.getAt(i) << endl;
	}
}

bool verifyGuess(string &guess, Grid<char> &grid, Lexicon &lex) {
	guess = ConvertToUpperCase(guess);
	if(!lex.containsWord(guess)) return false;
	Vector<Point> starts = getStartPoints(guess[0], grid);
	Vector<string> humVec,humVecTotal;
	string word = "";
	Vector<Point> path;
	for (int i = 0; i < starts.size(); i++) {
		Point start = starts.getAt(i);
		path.clear();
		path.add(start);
		word = "";
		word += grid.getAt(start.x, start.y);
		humVec = sloveBoggle(grid, start, 1, path, word, lex);
		humVecTotal = concateVec(humVecTotal, humVec);
	}
	return isContainWord(humVecTotal, guess);
}

bool isContainWord(Vector<string> &total, string &word) {
	for(int i = 0; i < total.size(); i++) {
		if(word == total.getAt(i)) return true;
	}
	return false;
}

Vector<Point> getStartPoints(char ch, Grid<char> &grid) {
	Vector<Point> result;
	for(int i = 0; i < grid.numRows(); i++) {
		for(int j = 0; j < grid.numCols(); j++) {
			if(ch == grid.getAt(i, j)) {
				Point p;
				p.x = i;
				p.y = j;
				result.add(p);
			}
		}
	}
	return result;
}