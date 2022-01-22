/*
	TO ADD:

	- it records the crib average for the day and for all time (use a file).
	- tracks histogram data for the day AND for all time (use a file).
	- at the end of a game, you input the winner and it records that for the day and all time (use a file).
	- at the end of a game, it lists the game's crib average, today's crib average, the all time crib average, and it saves all data to the file.
	- at the end of the session, it saves the data (same as with end of game) and congratulates the winner.
	- At the end of the session, reveal who would have won if just one big long game were played
	

	Start:
	
		 Molly's cut:  5
		Johnny's cut:  J
					
		Molly gets first crib!
		         the cut:  
		   Johnny's hand: 8
		    Molly's hand: 10
		    Molly's crib: 4

				   Crib avg:	  4.63 this game    5.46 today (33)    4.41 all-time (2034)

		Johnny's crib
		         the cut:
		    Molly's hand: 13
		   Johnny's hand: 12
		   Johnny's crib: 4

		... (last hand: have to specify how the game ends: jack cut, pegging, hand, or by crib; then specify how much was needed)





		If a 'w' is entered on the cut line (1st line) of input, we know the cribber won, so we just ask:

		Johnny's crib
		          the cut: jw
								JOHNNY WINS!
		    					how many points were needed?  1   (calculate and save the excess points: ONE in this case)

			 Molly's pos :  (get loser's position)
		     Molly's hand:  (these three (in this case) count toward the unused / excess)
		   	Johnny's hand:  
		   	Johnny's crib:





		If just a 'w' is entered on line 1 of input, you're asked who won and (see below):

		Johnny's crib
		         the cut:
		    Molly's hand: w
		    					who won? j  (must have been via pegging)
		    					how many points were needed?  4
		    					how many excess points pegged?  (only need to ask in the case of a pegging win. Can calculate in all other cases)

			 Molly's pos :  (get loser's position)
		     Molly's hand:  (these three (in this case) count toward the unused / excess)
		   	Johnny's hand:  
		   	Johnny's crib:


cribber won ^  vs nonCribber won --v


		Johnny's crib
		         the cut:
		    Molly's hand: w
		    					who won? m  (must have been via pegging)
		    					how many points were needed?  4
		    					how many excess points pegged?  (only need to ask in the case of a pegging win. Can calculate in all other cases)

			johnny's pos :  (get loser's position)
		     Molly's hand:  (these three (in this case) count toward the unused / excess)
		   	Johnny's hand:  
		   	Johnny's crib:




		If just a 'w' and a number is entered on line 1, then we know who won and we just need to ask 
		how much was needed and then collect users' excess and loser's pos"
		
		Johnny's crib
				 the cut:
		    Molly's hand: 12w   (so molly won and via her hand, obviously,)
		    					how many points were needed?  3   (so add 9 to her unusedPtsToday)

			Johhny's pos :
		   	Johnny's hand:  (these all go his unusedPtsToday)
		   	Johnny's crib:




		If a 'w' and a number is entered on line 2 or 3 of input, we know who won: 
		just ask how many were needed and get loser's position:

		Johnny's crib
		          the cut:
			 Molly's hand: 6
		   	Johnny's hand: 7w
								how many points were needed?   4  (so add 3 to unusedPtsToday for him)
			 Molly's pos :  (ask for this right away)
		   	Johnny's crib:	8



-----------

should inlcude a toggle flag for AT stats to be displayed--one for end of hand crib average stats,
and one for end of game where it displays the macro-game stats. (I'd leave it off by default).

I could also just track all the cuts and we could get a breakdown of that.

Track these for AT and Today, but not this game (not interesting):
	nobs
	4-of-a-kinds
	flushes
	super-flushes

- Could track who cuts what for the 1st crib... might be interesting to see the averages and a histo
of that.
- who gets the first crib (ie: who won the 1st crib cut), and 
how many times have got first crib AND won (and lost, and won going 2nd, lost going second)
- how many cribs each player has had
- how many points each player has got from crib, hand, and pegging (calculated)

maybe report how much "overwin" the winner has at the end of the day (and all-time)

Keep track of the winning margins? We could calculate skunks and 
super close games and track average winning margin.

Should we track how many hands are played each day? Should we track that? I mean... we already are.
Just also keep track of how many days we've played and we can then caculate an average.

Should be able to print out all the stats at any time (just not the histograms included)

At end of game should print out stats for overall game and for each player

At end of day, should print out even more stats of interest.




- - - future additions:

refactor to pass in 'players' instead of two seperate Player variables to
all functions it makes sense to

Update cutToSeeWhoGetsFirstCrib() / assignCribTo() to just return a pointer to the player who
is the cribber. Then can probably update determineLoser() to just be "giveMeOtherPlayer()" and 
use that to get nonCribber. 
This should make it more intuitive to read.

Should I rename "cribber" to "dealer" and "nonCribber" to "pone"?
Those are the correct terms for them...

press 'm' at any point to display a list of your input options 
(for the different flags and shit you are to use to input shit)

I could also accept a short-hand version of input at the end when someone wins, like so:
		+(j|m) p|h|c (needed) (excess)

error-check the inputs

Corrections should be possible in the event of a mis-entry / butterfingers.

OUTLIER: if you end the session in the middle, what should happen with the data? 
Should that incomplete round's data count toward the day's? The alltime's?
Should the numGamesPlayed variables be incremented? (I don't think so, but...?)

To generalize this program, keep the Players in an array and pass that into
functions instead of the individual Players. That would let us cycle through 
all that exist instead of needing to name them individually.



*/



#include <iostream>
//#include <iomanip>  <-- wtf was this for?
#include <vector>
#include <fstream>

using namespace std;

struct Player {
	string name;
	string indtAdjstdName;

	int legacyWins = 0;
	int legacyCribSum = 0;  // combined value (Molly + Johnny)
	int legacyNumHands = 0;

	vector<int> histoCutsForFirstCribAT {vector<int>(13,0)};  // in-struct vector initialization is a bit different...
	vector<int> histoCutsForMyCribAT {vector<int>(13,0)};  // aka starter cards
	vector<int> histoHandPtsAT {vector<int>(30,0)};
	vector<int> histoCribPtsAT {vector<int>(30,0)};
	vector<int> histoWinMarginsAT {vector<int>(120,0)};
	int numFirstCribsAT = 0;
	int numFirstCribsWonAT = 0;
	int peggedPtsAt = 0;
	vector<int> unusedPtsAT = {0,0,0,0}; // hand, crib, nibs, pegged
	int macrogamePtsAT = 0;
	int numSessionsPlayedAT = 0;
	int numHandsAT = 0;
	int numCribsAT = 0;
	int gamesPlayedAT = 0;
	int winsAT = 0;
	int nobsAT = 0;
	int FOKsAT = 0;  // four-of-a-kinds
	int flushesAT = 0;
	int superFlushesAT = 0;

	vector<string> cutsForFirstCribToday;
	vector<int> histoCutsForMyCribToday {vector<int>(13,0)};
	vector<int> histoHandPtsToday {vector<int>(30,0)};
	vector<int> histoCribPtsToday {vector<int>(30,0)};
	vector<int> winMarginsToday;
	int numFirstCribsToday = 0;
	int numFirstCribsWonToday = 0;
	int peggedPtsToday = 0;
	vector<int> unusedPtsToday = {0,0,0,0};
	int losingPositionToday = 0;
	int macrogamePtsToday = 0;
	int numHandsToday = 0;
	int numCribsToday = 0;
	int gamesPlayedToday = 0;
	int winsToday = 0;
	int nobsToday = 0;
	int FOKsToday = 0;
	int flushesToday = 0;
	int superFlushesToday = 0;

	int handPtsThisGame = 0;
	int cribPtsThisGame = 0;
	int numNibsThisGame = 0;  // x2 to get pts
	int peggedPtsThisGame = 0;
	vector<int> unusedPtsThisGame = {0,0,0,0};
	int numHandsThisGame = 0;
	int numCribsThisGame = 0;
};


const string SAVE_FILE_NAME = "cribulator_player_save_data.txt";
const string VAR_PREFIX = "### ";
const int HAND = 0; // these 4 used for excess point variable indeces
const int CRIB = 1;
const int NIBS = 2;
const int PEGGED = 3; 
const bool USER_WANTS_TO_END_SESSION = false;
const bool DEALER_WINS_FROM_NIBS = true;
const bool CRIBBER_WINS = true;
const bool NON_CRIBBER_WINS = true;
const bool SOMEONE_HAS_WON = true;
const string INDENTATION = "\t\t\t";  // BLAH why aren't the following ones capitalized?
const vector<string> histoVariations = {"h", "histo", "hist", "histogram"};
const vector<string> winVariations = {"w", "win"};
const vector<string> nobsVariations = {"n", "nobs", "knobs"};
const vector<string> fokVariations = {"fok", "four", "foak"};
const vector<string> flushVariations = {"f", "fl", "flush"};
const vector<string> superFlushVariations = {"sf", "super", "superflush", "super flush"};
const vector<string> endSessionVariations = {"end", "done", "exit", "fin", "quit"};
const vector<string> numbers = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};


bool useLegacyData = true;  // flag that enables/disables the use of legacy crib data
							// aka data from Jan 1 -> 17 before this program was complete
							// and all data was able to be collected.

// ----- FUNCTION PROTOTYPES -----
void initializeStuff(vector<Player*>);
void loadATDataFromFile(vector<Player*>);
void saveATDataToFile(vector<Player*>);
bool userWantsToEndSession(string);
bool cutToSeeWhoGetsFirstCrib(Player*, Player*, Player*&, Player*&);
void assignCribTo(char, Player*, Player*, Player*&, Player*&);
void swapCribbers(Player*&, Player*&);
void incrementHandCounters(Player*, Player*);
bool cutTheStarterCard(Player*, Player*);
bool handleInputFlags(string, Player*);
Player* winHandler(string, Player*, Player*, int);
int sumOfHistogram(vector<int>); 
void printEndOfHandStuff(vector<Player*>);
void updatePlayerDataAfterGame(vector<Player*>, Player*, Player*);
void printEndOfGameStuff(vector<Player*>, Player*);
void printPointsBreakdownForPlayerThisGame(Player*);
void resetThisGameDataForPlayers(vector<Player*>);
void printEndOfSessionStuff(vector<Player*>);
float combinedCribAverage(vector<Player*>, string);
void performEndOfSessionTasks(vector<Player*>);
int cardIndexOf(string);
int pointValueOf(string);
void printHistogram(string, Player&, Player&);
bool search(string, string);
bool search(string, vector<string>);
Player* determineLoser(vector<Player*>, Player*);











int main() {
	Player molly; Player johnny;  
	Player* cribber;  Player* nonCribber;
	vector<Player*> players = {&molly, &johnny};
	string userInput;
	
	initializeStuff(players);  
	cout << "\n\n\n* * * *  MOLLY & JOHNNY'S CRIBULATOR!!!  * * * *\n\n";

	do {  // game loop ------------------------------
		cout << "New game...\n\n";
		if (cutToSeeWhoGetsFirstCrib(&molly, &johnny, cribber, nonCribber) == USER_WANTS_TO_END_SESSION)
			break;
		cribber->numFirstCribsToday++;
		cribber->numFirstCribsAT++;
		Player* playerWhoHadFirstCrib = cribber;
		Player* winner;

		do {  // hand loop -----------------------
			incrementHandCounters(cribber, nonCribber);
			if (cutTheStarterCard(cribber, nonCribber) == DEALER_WINS_FROM_NIBS) {
				winner = winHandler("nibs win", cribber, nonCribber, 0);
				break;
			}
			
			cout << nonCribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			if (handleInputFlags(userInput, nonCribber) == SOMEONE_HAS_WON) {
				if (!search(userInput, numbers))
					winner = winHandler("someone won via pegging", cribber, nonCribber, 0);
				else 
					winner = winHandler("nonCribber wins via hand", cribber, nonCribber, pointValueOf(userInput) );
				break;
			}
			nonCribber->histoHandPtsAT[pointValueOf(userInput)]++;
			nonCribber->histoHandPtsToday[pointValueOf(userInput)]++;
			nonCribber->handPtsThisGame += pointValueOf(userInput);

			cout << cribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			if (handleInputFlags(userInput, cribber) == CRIBBER_WINS) {
				winner = winHandler("cribber wins via hand", cribber, nonCribber, pointValueOf(userInput) );
				break;
			}
			cribber->histoHandPtsAT[pointValueOf(userInput)]++;
			cribber->histoHandPtsToday[pointValueOf(userInput)]++;
			cribber->handPtsThisGame += pointValueOf(userInput);
	
			cout << cribber->indtAdjstdName << "'s crib:  ";
			getline(cin, userInput);
			if (handleInputFlags(userInput, cribber) == CRIBBER_WINS) {
				winner = winHandler("cribber wins via crib", cribber, nonCribber, pointValueOf(userInput) );
				break;
			}
			cribber->histoCribPtsAT[pointValueOf(userInput)]++;
			cribber->histoCribPtsToday[pointValueOf(userInput)]++;
			cribber->cribPtsThisGame += pointValueOf(userInput);
			
			printEndOfHandStuff(players); 
			swapCribbers(cribber, nonCribber);
			cout << "\n\n" << cribber->name << "'s crib\n";
			
		} while(!search(userInput, endSessionVariations));

		updatePlayerDataAfterGame(players, winner, playerWhoHadFirstCrib);
		printEndOfGameStuff(players, winner);
		resetThisGameDataForPlayers(players);

	} while(!search(userInput, endSessionVariations) );

	performEndOfSessionTasks(players);
	return 0;
}














 // ----- FUNCTION DEFINITIONS -----
void initializeStuff(vector<Player*> players) {
	cout << setprecision(2) << fixed; // 2 decimal places for output  BLAH: should this be global or in main?

	loadATDataFromFile(players); // load the players' all-time data
}


void loadATDataFromFile(vector<Player*> players) {
	cout << "\nLoading player save data . . . ";
	fstream saveFile(SAVE_FILE_NAME, ios::in);
	if(!saveFile){
		cout << "\n\n*** Error in opening '" << SAVE_FILE_NAME << "' ***\n\n"
		     << "Exiting program...\n\n\n";
		exit(1);
	}

	string line;
	for (Player* p : players) {
		 while(getline(saveFile, line)) {
			if (line == "")
				continue;
			
			if (line == VAR_PREFIX + "name") {
				getline(saveFile, line);
				p->name = line;
			}
			else if (line == VAR_PREFIX + "indtAdjstdName"){
				getline(saveFile, line);
				p->indtAdjstdName = line;
			}
			else if (line == VAR_PREFIX + "legacyWins"){
				getline(saveFile, line);
				p->legacyWins = stoi(line);
			}
			else if (line == VAR_PREFIX + "legacyCribSum"){
				getline(saveFile, line);
				p->legacyCribSum = stoi(line);
			}
			else if (line == VAR_PREFIX + "legacyNumHands"){
				getline(saveFile, line);
				p->legacyNumHands = stoi(line);
			}
				
				
			

			else if (line == VAR_PREFIX + "histoCutsForFirstCribAT") {
				for (int i = 0; i < 13; i++) {
					getline(saveFile, line);
					p->histoCutsForFirstCribAT[i] = stoi(line);
				}
			}
			else if (line == VAR_PREFIX + "histoCutsForMyCribAT") {
				for (int i = 0; i < 13; i++) {
					getline(saveFile, line);
					p->histoCutsForMyCribAT[i] = stoi(line);
				}
			}
			else if (line == VAR_PREFIX + "histoHandPtsAT") {
				for (int i = 0; i < 30; i++) {
					getline(saveFile, line);
					p->histoHandPtsAT[i] = stoi(line);
				}
			}
			else if (line == VAR_PREFIX + "histoCribPtsAT") {
				for (int i = 0; i < 30; i++) {
					getline(saveFile, line);
					p->histoCribPtsAT[i] = stoi(line);
				}
			}
			else if (line == VAR_PREFIX + "histoWinMarginsAT") {
				for (int i = 0; i < 120; i++) {
					getline(saveFile, line);
					p->histoWinMarginsAT[i] = stoi(line);
				}
			}

			else if (line == VAR_PREFIX + "numFirstCribsAT"){
				getline(saveFile, line);
				p->numFirstCribsAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "numFirstCribsWonAT"){
				getline(saveFile, line);
				p->numFirstCribsWonAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "peggedPtsAt"){
				getline(saveFile, line);
				p->peggedPtsAt = stoi(line);
			}

			else if (line == VAR_PREFIX + "unusedPtsAT") {
				for (int i = 0; i < 4; i++) {
					getline(saveFile, line);
					p->unusedPtsAT[i] = stoi(line);
				}
			}

			else if (line == VAR_PREFIX + "macrogamePtsAT"){
				getline(saveFile, line);
				p->macrogamePtsAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "numSessionsPlayedAT"){
				getline(saveFile, line);
				p->numSessionsPlayedAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "numHandsAT"){
				getline(saveFile, line);
				p->numHandsAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "numCribsAT"){
				getline(saveFile, line);
				p->numCribsAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "gamesPlayedAT"){
				getline(saveFile, line);
				p->gamesPlayedAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "winsAT"){
				getline(saveFile, line);
				p->winsAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "nobsAT"){
				getline(saveFile, line);
				p->nobsAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "FOKsAT"){
				getline(saveFile, line);
				p->FOKsAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "flushesAT"){
				getline(saveFile, line);
				p->flushesAT = stoi(line);
			}
			else if (line == VAR_PREFIX + "superFlushesAT"){
				getline(saveFile, line);
				p->superFlushesAT = stoi(line);
			}

			else if (line == "- - - - - - - - - -")
				break; // next player...

		} // /while there are lines in the file
	} // /for each player
	cout << "success :)";
}


void saveATDataToFile(vector<Player*> players) {
// save the players' all-time data to file

	string saveData = "";

	for (Player* p : players) {	
		saveData += VAR_PREFIX + "name" + "\n";
		saveData += p->name + "\n\n";

		saveData += VAR_PREFIX + "indtAdjstdName" + "\n";
		saveData += p->indtAdjstdName + "\n\n";

		saveData += VAR_PREFIX + "legacyWins" + "\n";
		saveData += to_string(p->legacyWins) + "\n\n";

		saveData += VAR_PREFIX + "legacyCribSum" + "\n";
		saveData += to_string(p->legacyCribSum) + "\n\n";

		saveData += VAR_PREFIX + "legacyNumHands" + "\n";
		saveData += to_string(p->legacyNumHands) + "\n\n";

		saveData += VAR_PREFIX + "histoCutsForFirstCribAT" + "\n";
		for (int i = 0; i < 13; i++)
			saveData += to_string(p->histoCutsForFirstCribAT[i]) + "\n";
		saveData += "\n";
		
		saveData += VAR_PREFIX + "histoCutsForMyCribAT" + "\n";
		for (int i = 0; i < 13; i++)
			saveData += to_string(p->histoCutsForMyCribAT[i]) + "\n";
		saveData += "\n";
		
		saveData += VAR_PREFIX + "histoHandPtsAT" + "\n";
		for (int i = 0; i < 30; i++)
			saveData += to_string(p->histoHandPtsAT[i]) + "\n";
		saveData += "\n";
		
		saveData += VAR_PREFIX + "histoCribPtsAT" + "\n";
		for (int i = 0; i < 30; i++)
			saveData += to_string(p->histoCribPtsAT[i]) + "\n";
		saveData += "\n";
		
		saveData += VAR_PREFIX + "histoWinMarginsAT" + "\n";
		for (int i = 0; i < 120; i++)
			saveData += to_string(p->histoWinMarginsAT[i]) + "\n";
		saveData += "\n";
		
		saveData += VAR_PREFIX + "numFirstCribsAT" + "\n";
		saveData += to_string(p->numFirstCribsAT) + "\n\n";

		saveData += VAR_PREFIX + "numFirstCribsWonAT" + "\n";
		saveData += to_string(p->numFirstCribsWonAT) + "\n\n";

		saveData += VAR_PREFIX + "peggedPtsAt" + "\n";
		saveData += to_string(p->peggedPtsAt) + "\n\n";

		saveData += VAR_PREFIX + "unusedPtsAT" + "\n";
		for (int i = 0; i < 4; i++)
			saveData += to_string(p->unusedPtsAT[i]) + "\n";
		saveData += "\n";
		
		saveData += VAR_PREFIX + "macrogamePtsAT" + "\n";
		saveData += to_string(p->macrogamePtsAT) + "\n\n";

		saveData += VAR_PREFIX + "numSessionsPlayedAT" + "\n";
		saveData += to_string(p->numSessionsPlayedAT) + "\n\n";

		saveData += VAR_PREFIX + "numHandsAT" + "\n";
		saveData += to_string(p->numHandsAT) + "\n\n";

		saveData += VAR_PREFIX + "numCribsAT" + "\n";
		saveData += to_string(p->numCribsAT) + "\n\n";

		saveData += VAR_PREFIX + "gamesPlayedAT" + "\n";
		saveData += to_string(p->gamesPlayedAT) + "\n\n";

		saveData += VAR_PREFIX + "winsAT" + "\n";
		saveData += to_string(p->winsAT) + "\n\n";

		saveData += VAR_PREFIX + "nobsAT" + "\n";
		saveData += to_string(p->nobsAT) + "\n\n";

		saveData += VAR_PREFIX + "FOKsAT" + "\n";
		saveData += to_string(p->FOKsAT)+ "\n\n";

		saveData += VAR_PREFIX + "flushesAT" + "\n";
		saveData += to_string(p->flushesAT) + "\n\n";

		saveData += VAR_PREFIX + "superFlushesAT" + "\n";
		saveData += to_string(p->superFlushesAT) + "\n\n";

		saveData += "\n\n\n\n- - - - - - - - - -\n\n\n\n\n\n";
	} // /for each player

	cout << "Saving player data to file . . . ";
	ofstream saveFile(SAVE_FILE_NAME, ios::out);
	saveFile << saveData;
	cout << "success :)\n\n\n\n";
}


bool userWantsToEndSession(string input) {
	if (search(input, endSessionVariations))
		return true;
	return false;
}


bool cutToSeeWhoGetsFirstCrib(Player* molly, Player* johnny, Player*& cribber, Player*& nonCribber) {
// returns false if user indicates they want to quit playing; else true
	string mollyCut;
	string johnnyCut;

	cout << " Molly draws a  ";
	getline (cin, mollyCut);
	if (userWantsToEndSession(mollyCut))
		return false;  // quit for today

	cout << "Johnny draws a  ";
	getline (cin, johnnyCut);
	molly->histoCutsForFirstCribAT[cardIndexOf(mollyCut)]++;
	molly->cutsForFirstCribToday.push_back(mollyCut);
	johnny->histoCutsForFirstCribAT[cardIndexOf(johnnyCut)]++;
	johnny->cutsForFirstCribToday.push_back(johnnyCut);
	if (cardIndexOf(mollyCut) < cardIndexOf(johnnyCut))
		assignCribTo('M', molly, johnny, cribber, nonCribber);
	else  
		assignCribTo('J', molly, johnny, cribber, nonCribber);
	cout << endl << cribber->name << " gets first crib!\n";
	return true;  // yes, we cut and are going to play a game
}


void assignCribTo(char cribberInitial, Player* molly, Player* johnny, Player*& cribber, Player*& nonCribber) {
// BLAH these are using hard-coded values... at least use constants at the top of the program
	if (cribberInitial == 'm' || cribberInitial == 'M') {
		cribber = molly;
		nonCribber = johnny;
	}
	else if (cribberInitial == 'j' || cribberInitial == 'J') {
		cribber = johnny;
		nonCribber = molly;
	}
	else {
		cout << "\n\nIncorrect cribberInitial passed to \"assignCribTo\" function: \'" 
			 << cribberInitial << "\'\nprogram closing..." << endl << endl;
		exit(0);
	}
}


void swapCribbers(Player*& cribber, Player*& nonCribber) {
	Player* temp = cribber;
	cribber = nonCribber;
	nonCribber = temp;
}


void incrementHandCounters(Player* cribber, Player* nonCribber) {
// note: this will track unused hands and cribs, because of where it gets called
	cribber->numHandsThisGame++;
	cribber->numHandsToday++;
	cribber->numHandsAT++;
	cribber->numCribsThisGame++;
	cribber->numCribsToday++;
	cribber->numCribsAT++;
	nonCribber->numHandsThisGame++;
	nonCribber->numHandsToday++;
	nonCribber->numHandsAT++;
}


bool cutTheStarterCard(Player* cribber, Player* nonCribber) {
// returns true if the cut results in the cribber winning; false otherwise
	string theCut;  // aka the "starter card" that is cut

	cout << "         the cut:  ";
	getline(cin, theCut);
	cribber->histoCutsForMyCribToday[cardIndexOf(theCut)]++;
	cribber->histoCutsForMyCribAT[cardIndexOf(theCut)]++;

	if (search(theCut, "j") || search(theCut, "w")) {  // nibs
		cribber->numNibsThisGame++;
		if (search(theCut, "w") )  // nibs for the win!
			return true;
	}
	return false;
}


bool handleInputFlags(string userInput, Player* player) {
// returns true if the flags indicate a win; false otherwise
	if (search(userInput, nobsVariations) ){
		player->nobsToday++;
		player->nobsAT++;
	}
	if (search(userInput, fokVariations) ){
		player->FOKsToday++;
		player->FOKsAT++;
	}

	if (search(userInput, superFlushVariations) ){
		player->superFlushesToday++;
		player->superFlushesAT++;
	} else if (search(userInput, flushVariations) ){
		player->flushesToday++;
		player->flushesAT++;
	}

	if (search(userInput, winVariations) )
		return true;  // someone has won

	// histo or other stats
	// ... BLAH TODO




	return false;
}





Player* winHandler(string winCondition, Player* cribber, Player* nonCribber, int ptsInWinningInput) {
// Calculates all the necessary crap when user indicates a win
// returns a pointer to the winning player
	string userInput;  // just going to use this for all the input and overwrite it,
					   // so I'll have mind the order in which I calculate stuff...

	if (search(winCondition, "nibs win") ){
		cout << INDENTATION << "~ ~ ~  " << cribber->name << " wins!  ~ ~ ~\n\n";
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		cribber->unusedPtsThisGame[NIBS] += (2 - stoi(userInput) );
		cribber->unusedPtsToday[NIBS] += cribber->unusedPtsThisGame[NIBS];
		cribber->unusedPtsAT[NIBS] += cribber->unusedPtsThisGame[NIBS];

		// calc winner's pegged points for this game
		cribber->peggedPtsThisGame = 121 - stoi(userInput) 
										 - cribber->handPtsThisGame 
										 - cribber->cribPtsThisGame 
										 - ((cribber->numNibsThisGame - 1) * 2); // -1 because we've already incr. numNibsThisGame in cut function								 
		cribber->peggedPtsToday += cribber->peggedPtsThisGame;
		cribber->peggedPtsAt += cribber->peggedPtsThisGame;

		// record loser's position
		cout << " " << nonCribber->indtAdjstdName << "'s pos:  ";  // nonCribber lost
		getline(cin, userInput);
		nonCribber->losingPositionToday += stoi(userInput);

		// use that to calc the winner's winning margin
		cribber->winMarginsToday.push_back(121 - stoi(userInput));
		cribber->histoWinMarginsAT[121 - stoi(userInput)]++;

		// and now calc loser's pegged points for this game
		nonCribber->peggedPtsThisGame = stoi(userInput)
									    - nonCribber->handPtsThisGame 
									    - nonCribber->cribPtsThisGame 
									    - (nonCribber->numNibsThisGame * 2);								 
		nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
		nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

		// record the other uncounted and excess points...
		// Hmmm, have to make an important decision here:
		// Do we count the unused hands and cribs toward their respective stats?
		// > Yes, I think we do. That way we can calc. averages properly.
		// We'll also keep track of the unused pts so we can subtract and calc
		// the "strict points" for a hand when desired
		cout << nonCribber->indtAdjstdName << "'s hand:  ";
		getline(cin, userInput);
		handleInputFlags(userInput, nonCribber);
		nonCribber->handPtsThisGame += stoi(userInput);
		nonCribber->histoHandPtsToday[stoi(userInput)]++;
		nonCribber->histoHandPtsAT[stoi(userInput)]++;
		nonCribber->unusedPtsThisGame[HAND] += stoi(userInput);
		nonCribber->unusedPtsToday[HAND] += stoi(userInput);
		nonCribber->unusedPtsAT[HAND] += stoi(userInput);

		cout << cribber->indtAdjstdName << "'s hand:  ";
		getline(cin, userInput);
		handleInputFlags(userInput, cribber);
		cribber->handPtsThisGame += stoi(userInput);
		cribber->histoHandPtsToday[stoi(userInput)]++;
		cribber->histoHandPtsAT[stoi(userInput)]++;
		cribber->unusedPtsThisGame[HAND] += stoi(userInput);
		cribber->unusedPtsToday[HAND] += stoi(userInput);
		cribber->unusedPtsAT[HAND] += stoi(userInput);

		cout << cribber->indtAdjstdName << "'s crib:  ";
		getline(cin, userInput);
		handleInputFlags(userInput, cribber);
		cribber->cribPtsThisGame += stoi(userInput);
		cribber->histoCribPtsToday[stoi(userInput)]++;
		cribber->histoCribPtsAT[stoi(userInput)]++;
		cribber->unusedPtsThisGame[CRIB] += stoi(userInput);
		cribber->unusedPtsToday[CRIB] += stoi(userInput);
		cribber->unusedPtsAT[CRIB] += stoi(userInput);

		return cribber;
	}


	else if (search(winCondition, "someone won via pegging") ){  
	// REFACTOR: there is SO much duplication of the above, below; there has to be a better way of doing this
	// can probably use *winner and *loser Player pointers to cut down on the code
		cout << INDENTATION << "Who won?  ";
		getline(cin, userInput);
		cout << INDENTATION << "How many points were needed?  ";
		string ptsNeeded;
		getline(cin, ptsNeeded);
		cout << INDENTATION << "How many points would have\n" << INDENTATION
							<< "   been pegged if there were space?  ";
		string peggedHypothetically;
		getline(cin, peggedHypothetically);

		if (toupper(cribber->name[0]) == toupper(userInput[0]) ) {  // cribber won
			cribber->unusedPtsThisGame[PEGGED] += (stoi(peggedHypothetically) - stoi(ptsNeeded) );
			cribber->unusedPtsToday[PEGGED] += cribber->unusedPtsThisGame[PEGGED];
			cribber->unusedPtsAT[PEGGED] += cribber->unusedPtsThisGame[PEGGED];

			// calc winner's pegged points for this game
			cribber->peggedPtsThisGame = 121 - cribber->handPtsThisGame 
											 - cribber->cribPtsThisGame 
											 - (cribber->numNibsThisGame * 2) 
											 + cribber->unusedPtsThisGame[PEGGED];  // have to include these to be consistent with the other point variables (they all store the macrogame data, not strict game data)							 
			cribber->peggedPtsToday += cribber->peggedPtsThisGame;
			cribber->peggedPtsAt += cribber->peggedPtsThisGame;

			// record loser's position
			cout << " " << nonCribber->indtAdjstdName << "'s pos:  ";  // nonCribber lost
			getline(cin, userInput);
			nonCribber->losingPositionToday += stoi(userInput);

			// use that to calc the winner's winning margin
			cribber->winMarginsToday.push_back(121 - stoi(userInput));
			cribber->histoWinMarginsAT[121 - stoi(userInput)]++;

			// and now calc loser's pegged points for this game
			nonCribber->peggedPtsThisGame = stoi(userInput)
											- nonCribber->handPtsThisGame 
											- nonCribber->cribPtsThisGame 
											- (nonCribber->numNibsThisGame * 2);								 
			nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
			nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

			// record the other uncounted and excess points...
			cout << nonCribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			handleInputFlags(userInput, nonCribber);
			nonCribber->handPtsThisGame += stoi(userInput);
			nonCribber->histoHandPtsToday[stoi(userInput)]++;
			nonCribber->histoHandPtsAT[stoi(userInput)]++;
			nonCribber->unusedPtsThisGame[HAND] += stoi(userInput);
			nonCribber->unusedPtsToday[HAND] += stoi(userInput);
			nonCribber->unusedPtsAT[HAND] += stoi(userInput);
			
			cout << cribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			handleInputFlags(userInput, cribber);
			cribber->handPtsThisGame += stoi(userInput);			
			cribber->histoHandPtsToday[stoi(userInput)]++;
			cribber->histoHandPtsAT[stoi(userInput)]++;
			cribber->unusedPtsThisGame[HAND] += stoi(userInput);
			cribber->unusedPtsToday[HAND] += stoi(userInput);
			cribber->unusedPtsAT[HAND] += stoi(userInput);

			cout << cribber->indtAdjstdName << "'s crib:  ";
			getline(cin, userInput);
			handleInputFlags(userInput, cribber);
			cribber->cribPtsThisGame += stoi(userInput);
			cribber->histoCribPtsToday[stoi(userInput)]++;
			cribber->histoCribPtsAT[stoi(userInput)]++;
			cribber->unusedPtsThisGame[CRIB] += stoi(userInput);
			cribber->unusedPtsToday[CRIB] += stoi(userInput);
			cribber->unusedPtsAT[CRIB] += stoi(userInput);

			return cribber;
		}
		else {  // nonCribber won
			nonCribber->unusedPtsThisGame[PEGGED] += (stoi(peggedHypothetically) - stoi(ptsNeeded) );
			nonCribber->unusedPtsToday[PEGGED] += nonCribber->unusedPtsThisGame[PEGGED];
			nonCribber->unusedPtsAT[PEGGED] += nonCribber->unusedPtsThisGame[PEGGED];

			// calc winner's pegged points for this game
			nonCribber->peggedPtsThisGame = 121 - nonCribber->handPtsThisGame 
											    - nonCribber->cribPtsThisGame 
											    - (nonCribber->numNibsThisGame * 2);							 
			nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
			nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

			// record loser's position
			cout << " " << cribber->indtAdjstdName << "'s pos:  ";  // cribber lost
			getline(cin, userInput);
			cribber->losingPositionToday += stoi(userInput);

			// use that to calc the winner's winning margin
			nonCribber->winMarginsToday.push_back(121 - stoi(userInput));
			nonCribber->histoWinMarginsAT[121 - stoi(userInput)]++;

			// and now calc loser's pegged points for this game
			cribber->peggedPtsThisGame = stoi(userInput)
											- cribber->handPtsThisGame 
											- cribber->cribPtsThisGame 
											- (cribber->numNibsThisGame * 2);								 
			cribber->peggedPtsToday += cribber->peggedPtsThisGame;
			cribber->peggedPtsAt += cribber->peggedPtsThisGame;

			// record the other uncounted and excess points...
			cout << nonCribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			handleInputFlags(userInput, nonCribber);
			nonCribber->handPtsThisGame += stoi(userInput);
			nonCribber->histoHandPtsToday[stoi(userInput)]++;
			nonCribber->histoHandPtsAT[stoi(userInput)]++;
			nonCribber->unusedPtsThisGame[HAND] += stoi(userInput);
			nonCribber->unusedPtsToday[HAND] += stoi(userInput);
			nonCribber->unusedPtsAT[HAND] += stoi(userInput);

			cout << cribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			handleInputFlags(userInput, cribber);
			cribber->handPtsThisGame += stoi(userInput);
			cribber->histoHandPtsToday[stoi(userInput)]++;
			cribber->histoHandPtsAT[stoi(userInput)]++;
			cribber->unusedPtsThisGame[HAND] += stoi(userInput);
			cribber->unusedPtsToday[HAND] += stoi(userInput);
			cribber->unusedPtsAT[HAND] += stoi(userInput);
			
			cout << cribber->indtAdjstdName << "'s crib:  ";
			getline(cin, userInput);
			handleInputFlags(userInput, cribber);
			cribber->cribPtsThisGame += stoi(userInput);
			cribber->histoCribPtsToday[stoi(userInput)]++;
			cribber->histoCribPtsAT[stoi(userInput)]++;
			cribber->unusedPtsThisGame[CRIB] += stoi(userInput);
			cribber->unusedPtsToday[CRIB] += stoi(userInput);
			cribber->unusedPtsAT[CRIB] += stoi(userInput);

			return nonCribber;
		}
	} // someone won via pegging


	else if (search(winCondition, "nonCribber wins via hand") ){
		cout << INDENTATION << "~ ~ ~  " << nonCribber->name << " wins!  ~ ~ ~\n\n";
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		nonCribber->unusedPtsThisGame[HAND] += (ptsInWinningInput - stoi(userInput) );
		nonCribber->unusedPtsToday[HAND] += nonCribber->unusedPtsThisGame[HAND];
		nonCribber->unusedPtsAT[HAND] += nonCribber->unusedPtsThisGame[HAND];

		// calc winner's pegged points for this game
		nonCribber->peggedPtsThisGame = 121 - stoi(userInput)
											- nonCribber->handPtsThisGame 
											- nonCribber->cribPtsThisGame 
											- (nonCribber->numNibsThisGame * 2);							 
		nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
		nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

		// record loser's position
		cout << " " << cribber->indtAdjstdName << "'s pos:  ";  // cribber lost
		getline(cin, userInput);
		cribber->losingPositionToday += stoi(userInput);

		// use that to calc the winner's winning margin
		nonCribber->winMarginsToday.push_back(121 - stoi(userInput));
		nonCribber->histoWinMarginsAT[121 - stoi(userInput)]++;

		// and now calc loser's pegged points for this game
		cribber->peggedPtsThisGame = stoi(userInput)
										- cribber->handPtsThisGame 
										- cribber->cribPtsThisGame 
										- (cribber->numNibsThisGame * 2);								 
		cribber->peggedPtsToday += cribber->peggedPtsThisGame;
		cribber->peggedPtsAt += cribber->peggedPtsThisGame;

		// record the other uncounted and excess points...
		nonCribber->handPtsThisGame += ptsInWinningInput;
		nonCribber->histoHandPtsToday[ptsInWinningInput]++;
		nonCribber->histoHandPtsAT[ptsInWinningInput]++;

		cout << cribber->indtAdjstdName << "'s hand:  ";
		getline(cin, userInput);
		handleInputFlags(userInput, cribber);
		cribber->handPtsThisGame += stoi(userInput);
		cribber->histoHandPtsToday[stoi(userInput)]++;
		cribber->histoHandPtsAT[stoi(userInput)]++;
		cribber->unusedPtsThisGame[HAND] += stoi(userInput);
		cribber->unusedPtsToday[HAND] += stoi(userInput);
		cribber->unusedPtsAT[HAND] += stoi(userInput);
		
		cout << cribber->indtAdjstdName << "'s crib:  ";
		getline(cin, userInput);
		handleInputFlags(userInput, cribber);
		cribber->cribPtsThisGame += stoi(userInput);
		cribber->histoCribPtsToday[stoi(userInput)]++;
		cribber->histoCribPtsAT[stoi(userInput)]++;
		cribber->unusedPtsThisGame[CRIB] += stoi(userInput);
		cribber->unusedPtsToday[CRIB] += stoi(userInput);
		cribber->unusedPtsAT[CRIB] += stoi(userInput);

		return nonCribber;
	}


	else if (search(winCondition, "cribber wins via hand") ){
		cout << INDENTATION << "~ ~ ~  " << cribber->name << " wins!  ~ ~ ~\n\n";
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		cribber->unusedPtsThisGame[HAND] += (ptsInWinningInput - stoi(userInput) );
		cribber->unusedPtsToday[HAND] += cribber->unusedPtsThisGame[HAND];
		cribber->unusedPtsAT[HAND] += cribber->unusedPtsThisGame[HAND];

		// calc winner's pegged points for this game
		cribber->peggedPtsThisGame = 121 - stoi(userInput)
									     - cribber->handPtsThisGame 
										 - cribber->cribPtsThisGame 
										 - (cribber->numNibsThisGame * 2);							 
		cribber->peggedPtsToday += cribber->peggedPtsThisGame;
		cribber->peggedPtsAt += cribber->peggedPtsThisGame;

		// record loser's position
		cout << " " << nonCribber->indtAdjstdName << "'s pos:  ";  // nonCribber lost
		getline(cin, userInput);
		nonCribber->losingPositionToday += stoi(userInput);

		// use that to calc the winner's winning margin
		cribber->winMarginsToday.push_back(121 - stoi(userInput));
		cribber->histoWinMarginsAT[121 - stoi(userInput)]++;

		// and now calc loser's pegged points for this game
		nonCribber->peggedPtsThisGame = stoi(userInput)
										  - nonCribber->handPtsThisGame 
										  - nonCribber->cribPtsThisGame 
										  - (nonCribber->numNibsThisGame * 2);								 
		nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
		nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

		// record the other uncounted and excess points...
		cribber->handPtsThisGame += ptsInWinningInput;
		cribber->histoHandPtsToday[ptsInWinningInput]++;
		cribber->histoHandPtsAT[ptsInWinningInput]++;
		
		cout << cribber->indtAdjstdName << "'s crib:  ";
		getline(cin, userInput);
		handleInputFlags(userInput, cribber);
		cribber->cribPtsThisGame += stoi(userInput);
		cribber->histoCribPtsToday[stoi(userInput)]++;
		cribber->histoCribPtsAT[stoi(userInput)]++;
		cribber->unusedPtsThisGame[CRIB] += stoi(userInput);
		cribber->unusedPtsToday[CRIB] += stoi(userInput);
		cribber->unusedPtsAT[CRIB] += stoi(userInput);

		return cribber;
	}


	else if (search(winCondition, "cribber wins via crib") ){
		cout << INDENTATION << "~ ~ ~  " << cribber->name << " wins!  ~ ~ ~\n\n";
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		cribber->unusedPtsThisGame[CRIB] += (ptsInWinningInput - stoi(userInput) );
		cribber->unusedPtsToday[CRIB] += cribber->unusedPtsThisGame[CRIB];
		cribber->unusedPtsAT[CRIB] += cribber->unusedPtsThisGame[CRIB];

		// calc winner's pegged points for this game
		cribber->peggedPtsThisGame = 121 - stoi(userInput)
									     - cribber->handPtsThisGame 
										 - cribber->cribPtsThisGame 
										 - (cribber->numNibsThisGame * 2);							 
		cribber->peggedPtsToday += cribber->peggedPtsThisGame;
		cribber->peggedPtsAt += cribber->peggedPtsThisGame;

		// record loser's position
		cout << " " << nonCribber->indtAdjstdName << "'s pos:  ";  // nonCribber lost
		getline(cin, userInput);
		nonCribber->losingPositionToday += stoi(userInput);

		// use that to calc the winner's winning margin
		cribber->winMarginsToday.push_back(121 - stoi(userInput));
		cribber->histoWinMarginsAT[121 - stoi(userInput)]++;  // check the indexing on this... hmmm.

		// and now calc loser's pegged points for this game
		nonCribber->peggedPtsThisGame = stoi(userInput)
										  - nonCribber->handPtsThisGame 
										  - nonCribber->cribPtsThisGame 
										  - (nonCribber->numNibsThisGame * 2);								 
		nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
		nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

		// record the other uncounted and excess points...
		cribber->cribPtsThisGame += ptsInWinningInput;
		cribber->histoCribPtsToday[ptsInWinningInput]++;
		cribber->histoCribPtsAT[ptsInWinningInput]++;

		return cribber;
	}

	else {
		cout << "\n\n*** Error: 'else' statement reached in winHandler() ***\n";
		cout << "    program closing...\n\n\n";
		exit(1);
	}
}


int sumOfHistogram(vector<int> histo) {
	int sum = 0;
	for (int i = 0; i < histo.size(); i++)
		sum += i * histo[i];
	return sum;
}


void printEndOfHandStuff(vector<Player*> players) {
// want it to be somethng like this for ex: 
// 4.63 this game    5.46 today (33)    4.41 all-time (2034)
	cout << "\n           crib avg:       ";
	cout << ((players[0]->cribPtsThisGame + players[1]->cribPtsThisGame) * 1.0) / 
			(players[0]->numHandsThisGame * 1.0)
		 << " this game     "

		 << combinedCribAverage(players, "today") << " today (" << players[0]->numHandsToday << ")     "
		 << combinedCribAverage(players, "all-time") << " ever (" 
		 << (useLegacyData ? players[0]->numHandsAT + players[0]->legacyNumHands : 
		 					 players[0]->numHandsAT) << ")";
}


void updatePlayerDataAfterGame(vector<Player*> players, Player* winner, Player* playerWhoHadFirstCrib) {
	int macrogamePtsThisGame;  // have to update this at game's end and not every hand because we need
							   // to have pegged points calculated first, and that only happens at game's end
	for (Player* p : players) {
		macrogamePtsThisGame = p->handPtsThisGame
							    + p->cribPtsThisGame
								+ (p->numNibsThisGame * 2)
								+ p->peggedPtsThisGame;
		p->macrogamePtsToday += macrogamePtsThisGame;
		p->macrogamePtsAT += macrogamePtsThisGame;

		p->gamesPlayedToday++;
		p->gamesPlayedAT++;
	}
	winner->winsToday++;
	winner->winsAT++;
	if (winner->name == playerWhoHadFirstCrib->name) {
		winner->numFirstCribsWonToday++;
		winner->numFirstCribsWonAT++;
	}
}


void printEndOfGameStuff(vector<Player*> players, Player* winner) {
// calculate and display the stats you want to at the end of a game:
	cout << endl << INDENTATION << "Congratulations " << winner->name << "! \n";
	string winWord;
	if (winner->winsToday == 1)
		winWord = "1st";
	else if (winner->winsToday == 2)
		winWord = "2nd";
	else if (winner->winsToday == 3)
		winWord = "3rd";
	else 
		winWord = "" + to_string(winner->winsToday) + "th";
	cout << INDENTATION << "That's your " << winWord << " win today!\n\n";

	cout << "Game " << players[0]->gamesPlayedToday << " stats:\n\n";
	// points breakdown for the winner
	printPointsBreakdownForPlayerThisGame(winner);
	cout << endl;
	// points breakdown for the loser
	// will do it, but problem:  loser's unused pts have been added to their 'ThisGame' variables already,
	// ... so the numbers won't be correct for them as it stands... how to fix this?
	printPointsBreakdownForPlayerThisGame(determineLoser(players, winner));
	cout << endl;

	// xxx pts (Leader) to yyy pts (Trailer) in today's macro-game thus far...  


	// any average/combined stats of interest?


	cout << endl;
}


void printPointsBreakdownForPlayerThisGame(Player* p) {
// BLAH: use printf() for this to simplify the formatting
	cout << setprecision(0) << fixed;
	const string BD_INDENT = "                        ";

	int strictHandPts = p->handPtsThisGame - p->unusedPtsThisGame[HAND];
	int strictCribPts = p->cribPtsThisGame - p->unusedPtsThisGame[CRIB];
	int strictNibsPts = (p->numNibsThisGame * 2) - p->unusedPtsThisGame[NIBS];
	int strictPeggedPts = p->peggedPtsThisGame - p->unusedPtsThisGame[PEGGED];
	int finPos = strictHandPts + strictCribPts + strictNibsPts + strictPeggedPts;

	float handPercent = (strictHandPts * 100.0) / (finPos * 1.0);
	float cribPercent = (strictCribPts * 100.0) / (finPos * 1.0);
	float nibsPercent = (strictNibsPts * 100.0) / (finPos * 1.0);
	float peggedPercent = (strictPeggedPts * 100.0) / (finPos * 1.0);

	cout << ((finPos > 99) ? "" : " ") << p->indtAdjstdName << "'s " << finPos << " pts:" << endl;

	cout << BD_INDENT << strictHandPts << "  hands"
		 << ((handPercent > 9.99) ? string(5, ' ') : string(6, ' '))
	     << handPercent << "%\n";

	cout << BD_INDENT << ((strictCribPts >= 10) ? "" : " ")
		 << strictCribPts << "  cribs"
		 << ((cribPercent > 9.99) ? string(5, ' ') : string(6, ' '))
		 << cribPercent << "%\n";
		 
	if (strictNibsPts > 0)
		cout << BD_INDENT << " " << strictNibsPts << "  nibs       "
			 << nibsPercent << "%\n";

	cout << BD_INDENT << ((strictPeggedPts >= 10) ? "" : " ")
		 << strictPeggedPts << "  pegging"
		 << ((peggedPercent > 9.99) ? string(3, ' ') : string(4, ' '))
	     << peggedPercent << "%\n";

	cout << setprecision(2) << fixed; 
}


void resetThisGameDataForPlayers(vector<Player*> players) {
// zeros all the 'ThisGame' data for each player; used before starting a new game, after finishing one.
// Also updates each player's gamesPlayedAT and gamesPlayedToday attributes
  for (Player* p : players) {
		p->handPtsThisGame = 0;
		p->cribPtsThisGame = 0;
		p->numNibsThisGame = 0;
		p->peggedPtsThisGame = 0;
		p->unusedPtsThisGame = {0,0,0,0};
		p->numHandsThisGame = 0;
		p->numCribsThisGame = 0;
	}
}


float combinedCribAverage(vector<Player*> players, string timeframe) {
	if (timeframe == "today")
		return ((sumOfHistogram(players[0]->histoCribPtsToday) + sumOfHistogram(players[1]->histoCribPtsToday)) * 1.0) / 
			(players[0]->numHandsToday * 1.0);

	else if (timeframe == "all-time" && useLegacyData)
		return ( (sumOfHistogram(players[0]->histoCribPtsAT) 
			      + sumOfHistogram(players[1]->histoCribPtsAT)
				  + players[0]->legacyCribSum ) * 1.0) 
				/ 
				  ((players[0]->numHandsAT + players[0]->legacyNumHands) * 1.0);

	else if (timeframe == "all-time" && !useLegacyData)
		return ((sumOfHistogram(players[0]->histoCribPtsAT) + sumOfHistogram(players[1]->histoCribPtsAT)) * 1.0) / 
			(players[0]->numHandsAT * 1.0);

	else {
		cout << "\n\n\n*** Error: invalid timeframe specified for 'combinedCribAverage()' call ***\n";
		cout << "program closing...\n\n\n";
		exit(1);
	}
}


void performEndOfSessionTasks(vector<Player*> players) {
// handle stuff before exiting program
	for (Player* p : players) {
		p->numSessionsPlayedAT++;
	}

	printEndOfSessionStuff(players);
	saveATDataToFile(players);
}


void printEndOfSessionStuff(vector<Player*> players) {
// display the stats you want to at the end of the day's session:
	cout << "\n\n\n       --  --  --  --  --  --  --  --  --  --  --  --  --  --  --\n\n\n";

	// print win vs win results today
	cout << "Today's champion: ";
	if (players[0]->winsToday >= players[1]->winsToday)
		cout << ((players[0]->winsToday == players[1]->winsToday) ? "TIE!" : players[0]->name) << "!\n\n"
			 << players[0]->winsToday << " wins for " << players[0]->name << "  vs  "
		     << players[1]->winsToday << " wins for " << players[1]->name << "\n\n\n";
	else	
		cout << players[1]->name << "!\n\n"
			 << players[1]->winsToday << " wins for " << players[1]->name << "  vs  "
		     << players[0]->winsToday << " wins for " << players[0]->name << "\n\n\n";

	// print macrogame results today
	cout << "Today's MACRO-game:   ";
	if (players[0]->macrogamePtsToday >= players[1]->macrogamePtsToday)
		cout << players[0]->name << " had " << players[0]->macrogamePtsToday << " pts  vs  "
		     << players[1]->macrogamePtsToday << " pts for " << players[1]->name << "\n\n\n";
	else
		cout << players[1]->name << " had " << players[1]->macrogamePtsToday << " pts  vs  "
		     << players[0]->macrogamePtsToday << " pts for " << players[0]->name << "\n\n\n";

	// print number of first crib wins
	cout << players[0]->name << " won " << players[0]->numFirstCribsWonToday
		 << " of their " << players[0]->numFirstCribsToday << " games today where they had first crib.\n";
	cout << players[1]->name << " won " << players[1]->numFirstCribsWonToday
		 << " of their " << players[1]->numFirstCribsToday << " games today where they had first crib.\n\n";

	cout << players[0]->name << " has won " << players[0]->numFirstCribsWonAT
		 << " of their " << players[0]->numFirstCribsAT << " games where they had first crib all-time.\n";
	cout << players[1]->name << " has won " << players[1]->numFirstCribsWonAT
		 << " of their " << players[1]->numFirstCribsAT << " games where they had first crib all-time.\n\n\n";

	// print AT win vs win
	int allTimeWinsP0 = players[0]->winsAT;
	int allTimeWinsP1 = players[1]->winsAT;
	if (useLegacyData) {
		allTimeWinsP0 += players[0]->legacyWins;
		allTimeWinsP1 += players[1]->legacyWins;
	}
	cout << "All-time wins" << (useLegacyData ? " (since Jan 1 2022): " : ": ");
	if (allTimeWinsP0 >= allTimeWinsP1)
		cout << players[0]->name << ": " << allTimeWinsP0 << " wins  -  "
		     << players[1]->name << ": " << allTimeWinsP1 << " wins\n\n\n";
	else	
		cout << players[1]->name << ": " << allTimeWinsP1 << " wins  -  "
		     << players[0]->name << ": " << allTimeWinsP0 << " wins\n\n\n";

	// print macrogame results AT
	cout << "All-time MACRO-game (since Jan 19):   ";
	if (players[0]->macrogamePtsAT >= players[1]->macrogamePtsAT)
		cout << players[0]->name << ": " << players[0]->macrogamePtsAT << " pts  -  "
		     << players[1]->name << ": " << players[1]->macrogamePtsAT << " pts\n\n\n";
	else
		cout << players[1]->name << ": " << players[1]->macrogamePtsAT << " pts  -  "
		     << players[0]->name << ": " << players[0]->macrogamePtsAT << " pts\n\n\n";

	// print crib averages
	cout << "Today's crib avg: " << combinedCribAverage(players, "today")
	     << " pts  (vs " << combinedCribAverage(players, "all-time")
		 << " pts all-time [" 
		 << (useLegacyData ? players[0]->numHandsAT + players[0]->legacyNumHands :
		 					 players[0]->numHandsAT) 
		 << " hands])\n\n\n";
}


int cardIndexOf(string card) {
// order is important. You'll want to search for 10 before you search for 1...

// BLAH: also might want to check for erronious entries. What if 16 is entered, for example?
// ...in this case, it will catch as a 1 and be entered as an Ace
	if (search(card, "k"))  return 12;
	if (search(card, "q"))  return 11;
	if (search(card, "j"))  return 10;  if (search(card, "w"))  return 10;  // might happen
	if (search(card, "10")) return 9;
	if (search(card, "9"))  return 8;
	if (search(card, "8"))  return 7;
	if (search(card, "7"))  return 6;
	if (search(card, "6"))  return 5;
	if (search(card, "5"))  return 4;
	if (search(card, "4"))  return 3;
	if (search(card, "3"))  return 2;
	if (search(card, "2"))  return 1;
	if (search(card, "1"))  return 0;
	if (search(card, "a"))  return 0;

	cout << "\n\n*** Error: '" << card << "' is not a valid card ***\n";
	cout << "Closing program...\n\n\n";
	exit(1);
}


int pointValueOf(string userInput) {
// this is used to strip out any flags or excess present in userInput
// order is important. You'll want to search for 10 before you search for 1...

// BLAH: also might want to check for erronious entries. What if 16 is entered, for example?
// ...in this case, it will catch as a 1 and be entered as an Ace
	if (search(userInput, "29")) return 29;
	if (search(userInput, "28")) return 28;
	if (search(userInput, "24")) return 24;
	if (search(userInput, "23")) return 23;
	if (search(userInput, "22")) return 22;
	if (search(userInput, "21")) return 21;
	if (search(userInput, "20")) return 20;
	if (search(userInput, "18")) return 18;
	if (search(userInput, "17")) return 17;
	if (search(userInput, "16")) return 16;
	if (search(userInput, "15")) return 15;
	if (search(userInput, "14")) return 14;
	if (search(userInput, "13")) return 13;
	if (search(userInput, "12")) return 12;
	if (search(userInput, "11")) return 11;	
	if (search(userInput, "10")) return 10;
	if (search(userInput, "9"))  return 9;
	if (search(userInput, "8"))  return 8;
	if (search(userInput, "7"))  return 7;
	if (search(userInput, "6"))  return 6;
	if (search(userInput, "5"))  return 5;
	if (search(userInput, "4"))  return 4;
	if (search(userInput, "3"))  return 3;
	if (search(userInput, "2"))  return 2;
	if (search(userInput, "1"))  return 1;
	if (search(userInput, "0"))  return 0;

	cout << "\n\n*** Error: '" << userInput << "' is not a possible score ***\n";
	cout << "Re-enter value:  ";
	string input;
	getline(cin, input);
	return pointValueOf(input);
}


void printHistogram(string input, Player& molly, Player& johnny) {
	string histoType;
	vector<string> mollyVariations = {"molly", "Molly", "m", "M"};
	vector<string> johnnyVariations = {"johnny", "Johnny", "j", "J"};
	vector<string> todayVariations = {"today", "td", "tod", "Today", "TD", "Tod"};
	vector<string> alltimeVariations = {"at", "AT", "All-Time", "all-time", "alltime", 
								        "All-time", "Alltime", "All Time", "all time"};

	cout << "\n\n";	
	if (search(input, mollyVariations) ) {
		if (search(input, todayVariations) ) {
			histoType = "molly-today";
			cout << "Molly's cribs today:";
		}	
		else  {
			histoType = "molly-alltime";
			cout << "Molly's cribs (all-time):";
		}
	}
	else if (search(input, johnnyVariations) ) {
		if (search(input, todayVariations) ) {
			histoType = "johnny-today";
			cout << "Johnny's cribs today:";
		}
		else {
			histoType = "johnny-alltime";
			cout << "Johnny's cribs (all-time):";
		}
	}
	else {
		if (search(input, todayVariations) ) {
			histoType = "combined-today";
			cout << "Today's cribs:";
		}
		else {
			histoType = "combined-alltime";
			cout << "Crib breakdown (all-time):";
		}
	}
	cout << "\n\n";
	for (int i = 0; i < 30; i++) {
		cout << i << (i < 10 ? "  " : " ");
		if (search(histoType, "molly-today") ) {
			cout << string(molly.histoCribPtsToday[i], '*');
		}
		if (search(histoType, "molly-alltime") ) {
			cout << string(molly.histoCribPtsAT[i], '*');
		}
		if (search(histoType, "johnny-today") ) {
			cout << string(johnny.histoCribPtsToday[i], '*');
		}
		if (search(histoType, "johnny-alltime") ) {
			cout << string(johnny.histoCribPtsAT[i], '*');
		}
		if (search(histoType, "combined-today") ) {
			cout << string((molly.histoCribPtsToday[i] + johnny.histoCribPtsToday[i]), '*');
		}
		if (search(histoType, "combined-alltime") ) {
			cout << string((molly.histoCribPtsAT[i] + johnny.histoCribPtsAT[i]), '*');
		}
		cout << endl;
	}
	cout << "\n";
	cout << "(should I also print the crib average here (or anything else?)\n"; // delete this
}


bool search(string s1, string s2) {
	// BLAH: make a function that converts strings to all lowercase or uppercase and use that to compare the two
	return (s1.find(s2) != string::npos);	 // is s2 in s1?
}


bool search(string s1, vector<string> s2) {
	// BLAH: use the uppercase/lowercase function to do this comparison, same as the above function
	// is any of s2[] in s1?
	for (int i = 0; i < s2.size(); i++) {
		if (s1.find(s2[i]) != string::npos)
			return true;
	}
	return (false);
}


Player* determineLoser(vector<Player*> players, Player* winner) {
// given a list of both players and given the winner, return a pointer to the loser
	if (players[0]->name == winner->name)
		return players[1];
	return players[0];
}