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
		    					how many points were needed?  3   (so add 9 to her excessPtsToday)

			Johhny's pos :
		   	Johnny's hand:  (these all go his excessPtsToday)
		   	Johnny's crib:




		If a 'w' and a number is entered on line 2 or 3 of input, we know who won: 
		just ask how many were needed and get loser's position:

		Johnny's crib
		          the cut:
			 Molly's hand: 6
		   	Johnny's hand: 7w
								how many points were needed?   4  (so add 3 to excessPtsToday for him)
			 Molly's pos :  (ask for this right away)
		   	Johnny's crib:	8



-----------


when to enter a jack cut (nibs)?  > 
	For these, we have to track them as their own thing: 
	need it for the end-of-game breakdown of points since it's not pegging, hand, or crib. 
	It's its own thing.

I could also just track all the cuts and we could get a breakdown of that.
	(should check for when a Jack is cut (nibs) and keep track of that for each player)

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
#include <iomanip>
#include <string>
#include <vector>

using namespace std;

struct Player {
	string name;
	string indtAdjstdName;

		vector<int> histoCutsForFirstCribAT {vector<int>(13,0)};  // in-struct vector initialization is a bit different...
		vector<int> histoCutsForMyCribAT {vector<int>(13,0)};  // aka starter cards
		vector<int> histoHandPtsAT {vector<int>(30,0)};
 		vector<int> histoCribPtsAT {vector<int>(30,0)};
		vector<int> histoWinMarginsAT {vector<int>(120,0)};
	int numFirstCribsAT = 0;
	int numFirstCribsWonAT = 0;
		int peggedPtsAt = 0;
		int excessPtsAT = 0;
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
		int excessPtsToday = 0;
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
	int numHandsThisGame = 0;
	int numCribsThisGame = 0;
};

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

// ----- FUNCTION PROTOTYPES -----
void initializeStuff(vector<Player*>);
void loadATDataFromFile(vector<Player*>);
void saveATDataToFile(vector<Player*>);
bool userWantsToEndSession(string);
bool cutToSeeWhoGetsFirstCrib(Player*, Player*, Player*&, Player*&);
void incrementHandCounters(Player*, Player*);
bool cutTheStarterCard(Player*, Player*);
bool handleInputFlags(string, Player*);
Player* winHandler(string, Player*, Player*, int);
void assignCribTo(char, Player*, Player*, Player*&, Player*&);
void swapCribbers(Player*&, Player*&);
int sumOfHistogram(vector<int>); 
void printEndOfHandStuff(vector<Player*>);
void printPointsBreakdownForPlayer(Player*);
void printEndOfGameStuff(vector<Player*>, Player*);
void resetThisGameDataForPlayers(vector<Player*>);
void printEndOfSessionStuff(vector<Player*>);
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
		if (!cutToSeeWhoGetsFirstCrib(&molly, &johnny, cribber, nonCribber) )
			break;  // user doesn't want to continue
		Player* winner;

		do {  // hand loop -----------------------
			incrementHandCounters(cribber, nonCribber);
			if (cutTheStarterCard(cribber, nonCribber) == DEALER_WINS_FROM_NIBS) {
				winner = winHandler("nibs win", cribber, nonCribber, pointValueOf(" "));
				break;
			}
			
			cout << nonCribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			if (handleInputFlags(userInput, nonCribber) == SOMEONE_HAS_WON) {
				if (!search(userInput, numbers))
					winner = winHandler("someone won via pegging", cribber, nonCribber, pointValueOf(" ") );
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

	// for (Player* p : players) {
	// 	p->histoCutsForMyCribToday.reserve(13);
 	// 	p->histoHandPtsToday.reserve(30);
 	// 	p->histoCribPtsToday.reserve(30);

	// 	for (int i = 0; i < 13; i++)
	// 		p->histoCutsForMyCribToday[i] = 0;

	// 	for (int i = 0; i < 30; i++) {
	// 		p->histoHandPtsToday[i] = 0;
	// 		p->histoCribPtsToday[i] = 0;
	// 	}
	// }
}


void loadATDataFromFile(vector<Player*> players) {
	// TEMP, until I write the file input code
	// This will load the actual data instead of using this hardcoded stuff below
	players[0]->name = "Molly";
	players[0]->indtAdjstdName = " Molly";
	players[1]->name = "Johnny";
	players[1]->indtAdjstdName = "Johnny";

	for (Player* p : players) {
		// p->histoCutsForFirstCribAT.reserve(13);
		// p->histoCutsForMyCribAT.reserve(13);
		// p->histoHandPtsAT.reserve(30);
		// p->histoCribPtsAT.reserve(30);
		// p->histoWinMarginsAT.reserve(120); // sb 121 technically, but opponent WILL always get a point
		// 								   // also note: index 0 will never be used for this. That's fine.
		// for (int i = 0; i < 13; i++) {
		// 	p->histoCutsForFirstCribAT[i] = 0;
		// 	p->histoCutsForMyCribAT[i] = 0;
		// }
		// for (int i = 0; i < 30; i++)
		// 	p->histoCribPtsAT[i] = 0;

		// for (int i = 0; i < 120; i++)
		// 	p->histoWinMarginsAT[i] = 0;

		p->numFirstCribsAT = 0;
		p->numFirstCribsWonAT = 0;
		p->peggedPtsAt = 0;
		p->nobsAT = 0;
		p->excessPtsAT = 0;
		p->macrogamePtsAT = 0;
		p->numSessionsPlayedAT = 0;
		p->numHandsAT = 0;
		p->numCribsAT = 0;
		p->gamesPlayedAT = 0;
		p->winsAT = 0;
		p->nobsAT = 0;
		p->FOKsAT = 0;
		p->flushesAT = 0;
		p->superFlushesAT = 0;
	}
}


void saveATDataToFile(vector<Player*> players) {
	return;
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

	cout << " Molly's cuts a  ";
	getline (cin, mollyCut);
	if (userWantsToEndSession(mollyCut))
		return false;  // quit for today
	cout << "Johnny's cuts a  ";
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


void incrementHandCounters(Player* cribber, Player* nonCribber) {
	cribber->numHandsAT++;
	cribber->numHandsToday++;
	cribber->numHandsThisGame++;
	cribber->numCribsAT++;
	cribber->numCribsToday++;
	cribber->numCribsThisGame++;
	nonCribber->numHandsAT++;
	nonCribber->numHandsToday++;
	nonCribber->numHandsThisGame++;
}


bool cutTheStarterCard(Player* cribber, Player* nonCribber) {
// returns true if the cut results in the cribber winning; false otherwise
	string theCut;  // aka the "starter card" that is cut

	cout << "      the cut:  ";
	getline(cin, theCut);
	cribber->histoCutsForMyCribAT[cardIndexOf(theCut)]++;
	cribber->histoCutsForMyCribToday[cardIndexOf(theCut)]++;

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
	if (search(userInput, flushVariations) ){
		player->flushesToday++;
		player->flushesAT++;
	}
	if (search(userInput, superFlushVariations) ){
		player->superFlushesToday++;
		player->superFlushesAT++;
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
		cout << INDENTATION << "~ ~ ~ " << cribber->name << " wins! ~ ~ ~" << endl;
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		cribber->excessPtsToday += (2 - stoi(userInput) );

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
		// Hmmm, have to make an important decision here:
		// Do we count the unused hands and cribs toward their respective stats?
		// > Yes, I think we do. That way we can calc. averages properly.
		cout << nonCribber->indtAdjstdName << "'s hand:  ";
		getline(cin, userInput);
		nonCribber->histoHandPtsAT[stoi(userInput)]++;
		nonCribber->histoHandPtsToday[stoi(userInput)]++;
		nonCribber->handPtsThisGame += stoi(userInput);

		cout << cribber->indtAdjstdName << "'s hand:  ";
		getline(cin, userInput);
		cribber->histoHandPtsAT[stoi(userInput)]++;
		cribber->histoHandPtsToday[stoi(userInput)]++;
		cribber->handPtsThisGame += stoi(userInput);
		cribber->excessPtsToday += stoi(userInput);
		cribber->excessPtsAT += stoi(userInput);

		cout << cribber->indtAdjstdName << "'s crib:  ";
		getline(cin, userInput);
		cribber->histoCribPtsAT[stoi(userInput)]++;
		cribber->histoCribPtsToday[stoi(userInput)]++;
		cribber->cribPtsThisGame += stoi(userInput);
		cribber->excessPtsToday += stoi(userInput);
		cribber->excessPtsAT += stoi(userInput);

		cout << "\n\nCongratulations " << cribber->name << "! \n\n\n";
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
		cout << INDENTATION << "How many points would have\n"
							<< "been pegged if there were room?  ";
		string peggedHypothetically;
		getline(cin, peggedHypothetically);

		if (toupper(cribber->name[0]) == toupper(userInput[0]) ) {  // cribber won
			cribber->excessPtsToday += (stoi(peggedHypothetically) - stoi(ptsNeeded) );

			// calc winner's pegged points for this game
			cribber->peggedPtsThisGame = 121 - cribber->handPtsThisGame 
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
			cout << nonCribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			nonCribber->histoHandPtsAT[stoi(userInput)]++;
			nonCribber->histoHandPtsToday[stoi(userInput)]++;
			nonCribber->handPtsThisGame += stoi(userInput);

			cout << cribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			cribber->histoHandPtsAT[stoi(userInput)]++;
			cribber->histoHandPtsToday[stoi(userInput)]++;
			cribber->handPtsThisGame += stoi(userInput);
			cribber->excessPtsToday += stoi(userInput);
			cribber->excessPtsAT += stoi(userInput);

			cout << cribber->indtAdjstdName << "'s crib:  ";
			getline(cin, userInput);
			cribber->histoCribPtsAT[stoi(userInput)]++;
			cribber->histoCribPtsToday[stoi(userInput)]++;
			cribber->cribPtsThisGame += stoi(userInput);
			cribber->excessPtsToday += stoi(userInput);
			cribber->excessPtsAT += stoi(userInput);

			cout << "\n\nCongratulations " << cribber->name << "! \n\n\n";
			return cribber;
		}
		else {  // nonCribber won
			nonCribber->excessPtsToday += (stoi(peggedHypothetically) - stoi(ptsNeeded) );

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
			nonCribber->histoHandPtsAT[stoi(userInput)]++;
			nonCribber->histoHandPtsToday[stoi(userInput)]++;
			nonCribber->handPtsThisGame += stoi(userInput);
			nonCribber->excessPtsToday += stoi(userInput);
			nonCribber->excessPtsAT += stoi(userInput);

			cout << cribber->indtAdjstdName << "'s hand:  ";
			getline(cin, userInput);
			cribber->histoHandPtsAT[stoi(userInput)]++;
			cribber->histoHandPtsToday[stoi(userInput)]++;
			cribber->handPtsThisGame += stoi(userInput);
			
			cout << cribber->indtAdjstdName << "'s crib:  ";
			getline(cin, userInput);
			cribber->histoCribPtsAT[stoi(userInput)]++;
			cribber->histoCribPtsToday[stoi(userInput)]++;
			cribber->cribPtsThisGame += stoi(userInput);

			cout << "\n\nCongratulations " << nonCribber->name << "! \n\n\n";
			return nonCribber;
		}
	} // someone won via pegging


	else if (search(winCondition, "nonCribber wins via hand") ){
		cout << INDENTATION << "~ ~ ~ " << nonCribber->name << " wins! ~ ~ ~" << endl;
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		nonCribber->excessPtsToday += (ptsInWinningInput - stoi(userInput) );

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
		nonCribber->histoHandPtsAT[ptsInWinningInput]++;
		nonCribber->histoHandPtsToday[ptsInWinningInput]++;
		nonCribber->handPtsThisGame += ptsInWinningInput;

		cout << cribber->indtAdjstdName << "'s hand:  ";
		getline(cin, userInput);
		cribber->histoHandPtsAT[stoi(userInput)]++;
		cribber->histoHandPtsToday[stoi(userInput)]++;
		cribber->handPtsThisGame += stoi(userInput);
		
		cout << cribber->indtAdjstdName << "'s crib:  ";
		getline(cin, userInput);
		cribber->histoCribPtsAT[stoi(userInput)]++;
		cribber->histoCribPtsToday[stoi(userInput)]++;
		cribber->cribPtsThisGame += stoi(userInput);

		cout << "\n\nCongratulations " << nonCribber->name << "! \n\n\n";
		return nonCribber;
	}


	else if (search(winCondition, "cribber wins via hand") ){
		cout << INDENTATION << "~ ~ ~ " << cribber->name << " wins! ~ ~ ~" << endl;
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		cribber->excessPtsToday += (ptsInWinningInput - stoi(userInput) );

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
		cribber->histoHandPtsAT[ptsInWinningInput]++;
		cribber->histoHandPtsToday[ptsInWinningInput]++;
		cribber->handPtsThisGame += ptsInWinningInput;
		
		cout << cribber->indtAdjstdName << "'s crib:  ";
		getline(cin, userInput);
		cribber->histoCribPtsAT[stoi(userInput)]++;
		cribber->histoCribPtsToday[stoi(userInput)]++;
		cribber->cribPtsThisGame += stoi(userInput);
		cribber->excessPtsToday += stoi(userInput);
		cribber->excessPtsAT += stoi(userInput);

		cout << "\n\nCongratulations " << cribber->name << "! \n\n";
		return cribber;
	}


	else if (search(winCondition, "cribber wins via crib") ){
		cout << INDENTATION << "~ ~ ~ " << cribber->name << " wins! ~ ~ ~" << endl;
		cout << INDENTATION << "How many points were needed?  ";
		getline(cin, userInput);
		cribber->excessPtsToday += (ptsInWinningInput - stoi(userInput) );

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
		cribber->histoCribPtsAT[ptsInWinningInput]++;
		cribber->histoCribPtsToday[ptsInWinningInput]++;
		cribber->cribPtsThisGame += ptsInWinningInput;

		cout << "\n\nCongratulations " << cribber->name << "! \n\n\n";
		return cribber;
	}

	else {
		cout << "\n\n*** Error: 'else' statement reached in winHandler() ***\n";
		cout << "    program closing...\n\n\n";
		exit(1);
	}
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


int sumOfHistogram(vector<int> histo) {
cout << endl << "size of 'histo' vector = " << histo.size() << endl;

	int sum = 0;
	for (int i = 0; i < histo.size(); i++)
		sum += i * histo[i];
	return sum;
}


void printEndOfHandStuff(vector<Player*> players) {
cout << "\n\nSIZE of players[0]->numHandsThisGame vector = " << players[0]->histoCribPtsToday.size() << endl << endl;
// want it to be somethng like this for ex: 
// 4.63 this game    5.46 today (33)    4.41 all-time (2034)
	cout << "           crib avg:\t\t";
	cout << ((players[0]->cribPtsThisGame + players[1]->cribPtsThisGame) * 1.0) / 
			(players[0]->numHandsThisGame * 1.0)
		 << " this game\t"

		 << ((sumOfHistogram(players[0]->histoCribPtsToday) + sumOfHistogram(players[1]->histoCribPtsToday)) * 1.0) / 
			(players[0]->numHandsToday * 1.0)
		 << " today (" << players[0]->numHandsToday << ")\t"

		 << ((sumOfHistogram(players[0]->histoCribPtsAT) + sumOfHistogram(players[1]->histoCribPtsAT)) * 1.0) / 
			(players[0]->numHandsAT * 1.0)
		 << " all-time (" << players[0]->numHandsAT << ")\t";


	// 4.00 this game	0.00 today (1)	0.00 all-time (1)
}


void printPointsBreakdownForPlayer(Player* p) {
	cout << "\t" << p->indtAdjstdName << "'s points:" << endl;

	cout << "\t\t" << p->peggedPtsThisGame << " pts pegged  (" 
	     << ((p->peggedPtsThisGame * 1.0) / (121 * 1.0)) << ")\n";

	cout << "\t\t" << p->handPtsThisGame << " pts from hands  ("
	     << ((p->handPtsThisGame * 1.0) / (121 * 1.0)) << ")\n";

	cout << "\t\t" << p->cribPtsThisGame << " pts from cribs  ("
		 << ((p->cribPtsThisGame * 1.0) / (121 * 1.0)) << ")\n";
		 
	if (p->numNibsThisGame > 0)
		cout << "\t\t" << p->numNibsThisGame * 2 << " pts from nibs  ("
			 << ((p->numNibsThisGame * 2.0) / (121 * 1.0)) << ")\n";
}


void printEndOfGameStuff(vector<Player*> players, Player* winner) {
// calculate and display the stats you want to at the end of a game:
	cout << "Game stats:\n\n";
	
	// points breakdown for the winner
	printPointsBreakdownForPlayer(winner);

	// points breakdown for the loser
	// will do it, but problem:  loser's unused pts have been added to their 'ThisGame' variables already,
	// ... so the numbers won't be correct for them as it stands... how to fix this?
	printPointsBreakdownForPlayer(determineLoser(players, winner));

	// xxx pts (Leader) to yyy pts (Trailer) in today's macro-game thus far...  


	// any average/combined stats of interest?



}


void resetThisGameDataForPlayers(vector<Player*> players) {
// zeros all the 'ThisGame' data for each player; used before starting a new game, after finishing one.
// Also updates each player's gamesPlayedAT and gamesPlayedToday attributes
  for (Player* p : players) {
		p->handPtsThisGame = 0;
		p->cribPtsThisGame = 0;
		p->numNibsThisGame = 0;
		p->numHandsThisGame = 0;
		p->numCribsThisGame = 0;

		p->gamesPlayedAT++;
		p->gamesPlayedToday++;
	}

}


void printEndOfSessionStuff(vector<Player*> players) {
// display the stats you want to at the end of the day's session:
	cout << "\n\n\n(end of session stuff here, printed from \'endOfCribSessionWrapUp()\')\n\n\n";



	// USED TO HAVE THIS:
	// cout << "\n\n\n    -----------------------------\n\n";
	// cout <<"    Crib average for today: " << (1.0 * total) / (1.0 * numHandsTotal);
	// cout << " points (over " << numHandsTotal << " hands)\n\n\n\n";
}


void performEndOfSessionTasks(vector<Player*> players) {
	// molly->numSessionsPlayedAT++;
	// johnny->numSessionsPlayedAT++;

	saveATDataToFile(players);
	printEndOfSessionStuff(players);
}


int cardIndexOf(string card) {
	if (search(card, "a"))  return 0;
	if (search(card, "1"))  return 0;
	if (search(card, "2"))  return 1;
	if (search(card, "3"))  return 2;
	if (search(card, "4"))  return 3;
	if (search(card, "5"))  return 4;
	if (search(card, "6"))  return 5;
	if (search(card, "7"))  return 6;
	if (search(card, "8"))  return 7;
	if (search(card, "9"))  return 8;
	if (search(card, "10")) return 9;
	if (search(card, "j"))  return 10;
	if (search(card, "q"))  return 11;
	if (search(card, "k"))  return 12;

	return -1;
}


int pointValueOf(string userInput) {
// this is used to strip out any flags or excess present in userInput
	if (search(userInput, "1"))  return 1;
	if (search(userInput, "2"))  return 2;
	if (search(userInput, "3"))  return 3;
	if (search(userInput, "4"))  return 4;
	if (search(userInput, "5"))  return 5;
	if (search(userInput, "6"))  return 6;
	if (search(userInput, "7"))  return 7;
	if (search(userInput, "8"))  return 8;
	if (search(userInput, "9"))  return 9;
	if (search(userInput, "10")) return 10;
	if (search(userInput, "11")) return 11;
	if (search(userInput, "12")) return 12;
	if (search(userInput, "13")) return 13;
	if (search(userInput, "14")) return 14;
	if (search(userInput, "15")) return 15;
	if (search(userInput, "16")) return 16;
	if (search(userInput, "17")) return 17;
	if (search(userInput, "18")) return 18;
	if (search(userInput, "20")) return 20;
	if (search(userInput, "21")) return 21;
	if (search(userInput, "22")) return 22;
	if (search(userInput, "23")) return 23;
	if (search(userInput, "24")) return 24;
	if (search(userInput, "28")) return 28;
	if (search(userInput, "29")) return 29;

	return -1;
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