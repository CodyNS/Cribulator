/*
	TO ADD:

	- it records the crib average for the day and for all time (use a file).
	- tracks histogram data for the day AND for all time (use a file).
	- at the end of a game, you input the winner and it records that for the day and all time (use a file).
	- at the end of a game, it lists the game's crib average, today's crib average, the all time crib average, and it saves all data to the file.
	- at the end of the session, it saves the data (same as with end of game) and congratulates the winner.

	

Basically, the more data you keep track of, the more stuff you can calculate. 
Keep track of it all? You can calculate it all. BUT... it's more of a pain in the ass.
You must choose.

	K, envision how these games go. Imagine doing this. See what is reasonable to track. Act it out here, now...

	...



I could also just track all the cuts and we could get a breakdown of that.
	(should check for when a Jack is cut (+2 points) and keep track of that for each player)

Could also track who cuts what for the 1st crib... might be interesting to see the averages and a histo
of that.

could track the number of flushes (just for fun),
...think of any other quirky hands or occurences that would be fun to track.
	


	Probably just organize each player's data into a struct.

	it should keep track of all stats for each player, then we can just go from there:
		- who gets the first crib (ie: who won the 1st crib cut)
		- how many cribs each player has had
		- how many points each player has got from crib, hand, and pegging (calculated)
	
	- should it keep track of the winning margin? We could calculate skunks and average winning margin then.

	- end of game should print out stats for overall game and for each player
	- should be able to print out all the stats at any time (just not the histograms included)
	- corrections should be possible

	...then turn this all into a web page.
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



int main() {

	const int END_TODAYS_SESSION = -1;
	const int HISTO_TD_SELECT = -2;
	const int HISTO_AT_SELECT = -3;

	int histoCribAT[30];  // LOAD these from file
	int totalAT= 0;
	int numHandsAT = 0;
	int winsAT[2];

	//vector <int> histoToday(30);
	int histoCribToday[30];
	int totalToday = 0;
	int numHandsToday = 0;
	int winsToday[] = (0, 0);

	int totalThisGame = 0;
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
				totalThisGame += thisCrib;  totalToday += thisCrib;  totalAT += thisCrib;
				numHandsThisGame++;  numHandsToday++;  numHandsAT++;
				histoCribToday[thisCrib]++;  histoAT[thisCrib]++;

				cout << "\t\t     " << (totalThisGame * 1.0) / (numHandsThisGame * 1.0) << " pts avg this game (";
				cout << numHandsThisGame << " " << ((numHandsThisGame > 1) ? "hands" : "hand") << ")\n";

				cout << "\t\t     " << (totalToday * 1.0) / (numHandsToday * 1.0) << " pts avg today (";
				cout << numHandsToday << " " << ((numHandsToday > 1) ? "hands" : "hand") << ")\n";

				cout << "\t\t     " << (totalAT * 1.0) / (numHandsAT * 1.0) << " pts all-time (";
				cout << numHandsAT << " " << ((numHandsAT > 1) ? "hands" : "hand") << ")\n";

				cout << "Enter crib: ";
		}		
		cin >> thisCrib;
	} while(thisCrib != END_TODAYS_SESSION);

	cout << "\n\n\n    -----------------------------\n\n";
	cout <<"    Crib average for today: " << (totalToday * 1.0) / (numHandsToday * 1.0);
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

