// Basic version + histogram feature for displaying occurences of crib totals for your play session (enter -2 at prompt to see it).
// Written Jan 5, 2022

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>


using namespace std;


void printHistogram(vector <int>);

int main() {
	int total = 0;
	int numHandsTotal = 0;
	int thisCrib = 0;
	int SENTINEL = -1;
	int HISTOGRAM_SELECT = -2;
	vector <int> occurences(30);

	for (int i = 0; i < 30; i++)
		occurences[i] = 0;

	cout << setprecision(2) << fixed;

	cout << "\n\n\n" << 
			"* * * * MUDRUCKER & MURPHY'S CRIBULATOR V2 * * * *\n\nEnter crib: ";
	cin >> thisCrib;

	do {
		if(HISTOGRAM_SELECT == thisCrib)
			printHistogram(occurences);
		else {
			total += thisCrib;
			numHandsTotal++;
			occurences[thisCrib]++;

			cout << "\t\t     " << (1.0 * total) / (1.0 * numHandsTotal) << " pts avg  (";
			cout << numHandsTotal << " " << ((numHandsTotal > 1) ? "hands" : "hand") << ")\n";
			cout << "Enter crib: ";
		}
		cin >> thisCrib;
	} while(thisCrib != SENTINEL);

	cout << "\n\n\n    -----------------------------\n\n";
	cout <<"    Crib average for today: " << (1.0 * total) / (1.0 * numHandsTotal);
	cout << " points (over " << numHandsTotal << " hands)\n\n\n\n";

	return 0;
}

 // --------- definitions ---------
void printHistogram(vector <int> occurences) {
	cout << "\n";
	for (int i = 0; i < 30; i++) {
		cout << i << (i < 10 ? "  " : " ") << string(occurences[i], '*') << endl;
	}
	cout << "\n";
	cout << "Enter crib: ";
}