#include "stdio.h"
#include "stdlib.h"
#include "string.h"

//Hangman

/*
int board[25] = {
	:,  :, :, :, :,
	:, O,-,X, :,
	:, X, -, -, :,
	:, -, -,  -, :,
	:,  :,  :, :, :,
}
*/

enum { NOUGHTS, CROSSES, BORDER, EMPTY };
enum { HUMANWIN, COMPWIN, DRAW};

const int Directions[4] = { 1, 5, 4, 6 }; //Test for 3 in a row of a array up, down, across, across

const int ConvertTo25[9] = {
	6,7,8,
	11,12,13,
	16,17,18
};

//Get the squares in the array of directions
int GetNumForDir( int startSq, const int dir, const int *board, const int us ) {
	int found = 0;
	while( board[startSq] != BORDER ) { 
		if( board[startSq] != us) {
			break;
		}
		found++;
		startSq += dir;
	}
	return found;
}

//Count for three in a row both directions for each possible line of win from Directions[]
int FindThreeInARow(const int *board, const int ourIndex, const int us ) {
	
	int DirIndex = 0;  //Loop through the squares in directions of array
	int Dir = 0; //The direction, up down side
	int threeCount = 1; //Count of 3 in a row from an initial tile
	
	for( DirIndex = 0; DirIndex < 4; ++DirIndex) {
		Dir = Directions[DirIndex];
		threeCount += GetNumForDir( ourIndex + Dir, Dir, board, us );
		threeCount += GetNumForDir( ourIndex + Dir *-1, Dir * -1, board, us);
		if(threeCount == 3)	{	
			break;
		}
		threeCount = 1;
	}
	return threeCount;
} 

void InitialiseBoard(int *board) { //uses a pointer to our array
	
	//Border Squares
	int index = 0;
	for(index = 0; index < 25; ++index) {
		board[index] = BORDER;
	}
	//Blank Squares
	for(index = 0; index < 9; ++index) {
		board[ConvertTo25[index]] = EMPTY;
	}
}

void PrintBoard(const int *board) {
	int index = 0;
	char pceChars[] = "OX|-";
	
	printf("\n\nBoard:\n\n");
	for(index = 0; index < 9; ++index) {
		if(index != 0 && index%3==0){
			printf("\n\n");
		}
		printf("%4c", pceChars[board[ConvertTo25[index]]]); //get char piece, convert to 25 squares
	}
		printf("\n");
}

int HasEmpty(const int *board) {
	int index = 0;
	for(index = 0; index < 9; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) 
		return;
	}
	return 0;
}

void MakeMove(int *board, const int sq, const side) { //board pointer, square select, side making selection
	board[sq] = side;
}

int GetWinningMove(int *board, const int side) {
	int ourMove = -11; 
	int winFound = 0; //flag if winning move is found
	int index;
	
	for( index = 0; index < 9; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) { //Converts if square empty = available move
			ourMove = ConvertTo25[index];
			board[ourMove] = side;
			
			if( FindThreeInARow(board, ourMove, side) == 3) {
				winFound = 1;
			}
			board[ourMove] = EMPTY; //reset board point for next time
			if( winFound == 1) {
				break;
			}
			ourMove = -1;
		};
	}
	return ourMove;
}

int GetComputerMove(int *board, const int side) {
	//Need array with available moves and moves already completed
	int index = 0;
	int numFree = 0; //count of availableMoves[ ]
	int availableMoves[9]; //an array of available moves
	int randMove = 0; //rand() % numFree 
	
	//First check for winning move, if no winning move select random move
	randMove = GetWinningMove(board, side);
	if( randMove != -1) {
		return randMove;
	}
	randMove = 0;
	
	//Convert selection to the 25 space board including border
	for( index = 0; index < 9; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) { //Converts if square empty = available move
			availableMoves[numFree++] = ConvertTo25[index]; //saving to array is post ++
		};
	}
	
	randMove = (rand() % numFree);
	return availableMoves[randMove];
	
}

//int returned is arrayboard index
int GetHumanMove(const int *board) {
	char userInput[4];
	int moveOk = 0;
	int move = -1;
	
	//keeps running until the user makes an acceptable move
	while(moveOk == 0) {
		
		printf("Please enter a move from 1 to 9: ");
		fgets(userInput, 3, stdin);
		fflush(stdin);  //gets rid of input after the fgets 
		
		if(strlen(userInput) != 2) {
			printf("Invalid strlen()\n");
			continue;
		}
		
		if( sscanf(userInput, "%d", &move) != 1) { //sscanf parses results of a scan
			move = -1;
			printf("Invalid sscanf()\n");
			continue;
		}
		
		if( move < 1 || move > 9) {
			move = -1;
			printf("Invalid range \n");
			continue;
		}
		
		move--; //Zero index
		
		if( board[ConvertTo25[move]] != EMPTY) {
			move =-1;
			printf("Square not available\n");
			continue;
		}
		
		moveOk = 1;
	}
	printf("Making Move... %d\n", (move+1));
	return ConvertTo25[move];
}

void RunGame() {
	int GameOver = 0;
	int Side = NOUGHTS;
	int LastMoveMade = 0;
	int board[25];
	
	InitialiseBoard(&board[0]);
	PrintBoard(&board[0]);
	
	//In each loop test whether GameOver == 1, whichever side not to move is winner
	while(!GameOver){
		if(Side == NOUGHTS) {
			LastMoveMade = GetHumanMove(&board[0]);
			MakeMove(&board[0], LastMoveMade, Side);
			Side = CROSSES;
		} else{
		//get move from computer, make move on board, change side
			LastMoveMade = GetComputerMove(&board[0], Side);
			MakeMove(&board[0], LastMoveMade, Side);
			Side = NOUGHTS;
			PrintBoard(&board[0]);
		}
		// if three in a row exists GameOver == true
		if( FindThreeInARow(board, LastMoveMade, Side ^ 1) == 3 ) { //last move made references int ourIndex, side XOR 1, nought is 0, cross is 1, XOR is shortcut side switch
			printf("Game Over!\n");
			GameOver = 1;
			PrintBoard(&board[0]);
			if( Side == NOUGHTS ) {
				printf("\nComputer Wins!\n");
			} else {
				printf("\nHuman Wins\n");
			}
		}
		
		//if no more moves, game == draw
		
		if(!HasEmpty(board)) {
			printf("Game Over!\nIt's a draw\n");
			GameOver = 1;
		}
	}
	GameOver=1;
	
}

int main(){

	srand(time(NULL));
	RunGame();
	
	return 0;
}
