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
		   Johnny's hand: 8
		    Molly's hand: 10
		    Molly's crib: 4

				   Crib avg:	  4.63 this game    5.46 today (33)    4.41 all-time (2034)

		Johnny's crib
		    Molly's hand: 13
		   Johnny's hand: 12
		   Johnny's crib: 4

		... (last hand: have to specify how the game ends: jack cut, pegging, hand, or by crib; then specify how much was needed)


		If just a 'w' is entered on line 1 of input, you're asked who won and (see below):

		Johnny's crib
		    Molly's hand: w
		    					who won? j
		    					via P)egging or Jack was cut?  p  (ask this if only 'w' entered, and don't need to if non-cribber won [had to be pegging])
		    					how many points were needed?  4
		    					how many excess points pegged?  (only need to ask in the case of a pegging win. Can calculate in all other cases)

			 Molly's pos :  (get loser's position)
		     Molly's hand:  (these three (in this case) count toward the unused / excess)
		   	Johnny's hand:  
		   	Johnny's crib:




		If just a 'w' and a number is entered on line 1, then we know who won and we just need to ask 
		how much was needed and then collect users' excess and loser's pos"
		
		Johnny's crib
		    Molly's hand: 12w   (so molly won and via her hand, obviously,)
		    					how many points were needed?  3   (so add 9 to her excessPtsToday)

			Johhny's pos :
		   	Johnny's hand:  (these all go his excessPtsToday)
		   	Johnny's crib:




		If a 'w' and a number is entered on line 2 or 3 of input, we know who won: 
		just ask how many were needed and get loser's position:

		Johnny's crib
			 Molly's hand: 6
		   	Johnny's hand: 7w
								how many points were needed?   4  (so add 3 to excessPtsToday for him)
			 Molly's pos :  (ask for this right away)
		   	Johnny's crib:	8




		FUTURE WORK:  I could also accept a short-hand version of input at the end like so:
		+(j|m) p|h|c (needed) (excess)


-----------

when to enter a jack cut?  > 
	For these, we have to track them as their own thing: 
	need it for the end-of-game breakdown of points since it's not pegging, hand, or crib. 
	It's its own thing.

Probably just organize each player's data into a struct. IS there a better way of organizing the data?

I could also just track all the cuts and we could get a breakdown of that.
	(should check for when a Jack is cut (+2 points) and keep track of that for each player)

think: when/how to enter a nibs?  Should we even? Is this interesting?
	yes, track this.
	just start the points entry with an 'n'  ex:  Johnnny's hand: n12

- Could track who cuts what for the 1st crib... might be interesting to see the averages and a histo
of that.
- who gets the first crib (ie: who won the 1st crib cut)
- how many cribs each player has had
- how many points each player has got from crib, hand, and pegging (calculated)

maybe report how much "overwin" the winner has at the end of the day (and all-time)

Should we track how many times the loser would have won if the had been able to count first? 

Keep track of the winning margins? We could calculate skunks and super close games and track average winning margin.

Should we track how many hands are played each day? Should we track that? I mean... we already are.
Just also keep track of how many days we've played and we can then caculate an average.

At end of game should print out stats for overall game and for each player

At end of day, should print out even more stats of interest.

Should be able to print out all the stats at any time (just not the histograms included)

Corrections should be possible in the event of a mis-entry / butterfingers





----- think this has been done / I've got it handled / done thinking about -----

Hmmm, to do the one long super game at the end thing, we have to track not only the excess points, but
the final position of the loser.  We can just add those together, add the excess points to those, then
multiply wins by 120 to get the user's final position for the day's mega uni-game... I think.


*/



#include <iostream>
#include <iomanip>
#include <string>

using namespace std;


// ----- FUNCTION PROTOTYPES -----
void initializeStuff(int[]);
void loadDataFromFile();
void saveDataToFile();
void printHistogram(int[]);


struct Player {
  // move everything into this...
}



int main() {

	const int END_TODAYS_SESSION = -1;
	const int HISTO_TD_SELECT = -2;
	const int HISTO_AT_SELECT = -3;

	int histoCribAT_molly[30];  // All time vars. LOAD these from file
	int histoCribAT_johnny[30];
	int totalCribAT_molly = 0;
	int totalCribAT_johnny = 0;
	int numCribsAT_molly = 0;     // BLAH instead of having 2 of everything, why not just just a 2 cell array for each?
	int numCribsAT_johnny = 0;
	int winsAT[2];

	int histoCribToday[30];  // today's vars
	int totalCribToday = 0;
	int numHandsToday = 0;
	int winsToday[] = (0, 0);
	int excessPtsTodayp[] = (0,0);
	int losingPositionToday[] = (0,0)

	int totalCribThisGame = 0;  // this particular game vars
	int numHandsThisGame = 0;
	string winnerThisGame;

	int thisCrib = 0;
	
	initializeStuff(histoCribToday);  // --------------------------


	cout << "\n\n\n" << 
			"* * * * MUDRUCKER & MURPHY'S CRIBULATOR V2 * * * *" << 
			"\n\nEnter crib: ";
	cin >> thisCrib;

	do {
		switch(thisCrib) {

			case HISTO_TD_SELECT:
				printHistogram(histoCribToday);
				break;

			case default:
				totalCribThisGame += thisCrib;  totalCribToday += thisCrib;  totalAT += thisCrib;
				numHandsThisGame++;  numHandsToday++;  numHandsAT++;
				histoCribToday[thisCrib]++;  histoAT[thisCrib]++;

				cout << "\t\t     " << (totalCribThisGame * 1.0) / (numHandsThisGame * 1.0) << " pts avg this game (";
				cout << numHandsThisGame << " " << ((numHandsThisGame > 1) ? "hands" : "hand") << ")\n";

				cout << "\t\t     " << (totalCribToday * 1.0) / (numHandsToday * 1.0) << " pts avg today (";
				cout << numHandsToday << " " << ((numHandsToday > 1) ? "hands" : "hand") << ")\n";

				cout << "\t\t     " << (totalAT * 1.0) / (numHandsAT * 1.0) << " pts all-time (";
				cout << numHandsAT << " " << ((numHandsAT > 1) ? "hands" : "hand") << ")\n";

				cout << "Enter crib: ";
		}		
		cin >> thisCrib;
	} while(thisCrib != END_TODAYS_SESSION);

	cout << "\n\n\n    -----------------------------\n\n";
	cout <<"    Crib average for today: " << (totalCribToday * 1.0) / (numHandsToday * 1.0);
	cout << " points (over " << numHandsToday << " hands)\n\n\n\n";

	return 0;
}


 // ----- FUNCTION DEFINITIONS -----
void initializeStuff(int[] histoCribToday) {
	cout << setprecision(2) << fixed; // 2 decimal places for output

	for (int i = 0; i < 30; i++)  // initialize today's histogram
		histoCribToday[i] = 0;
}

void loadDataFromFile() {

}

void saveDataToFile() {

}

void printHistogram(int[] histoCribToday) {
	cout << "\n";
	for (int i = 0; i < 30; i++) {
		cout << i << (i < 10 ? "  " : " ") << string(histoCribToday[i], '*') << endl;
	}
	cout << "\n";
	cout << "Enter crib: ";
}

