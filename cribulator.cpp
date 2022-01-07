// Basic version, orignally written around Jan 1 2022 by CodyNS

#include <iostream>
#include <iomanip>


using namespace std;


int main() {
	int total = 0;
	int numHandsTotal = 0;
	int thisCrib = 0;
	int SENTINEL = -1;

	cout << setprecision(2) << fixed;

	cout << "\n\n\n" << 
			"* * * * MUDRUCKER & MURPHY'S CRIB CALCULATOR * * * *\n\nEnter crib: ";
	cin >> thisCrib;

	do {
		total += thisCrib;
		numHandsTotal++;

		cout << "\t\t     " << (1.0 * total) / (1.0 * numHandsTotal) << " pts avg  (";
		cout << numHandsTotal << " " << ((numHandsTotal > 1) ? "hands" : "hand") << ")\n";
		cout << "Enter crib: ";

		cin >> thisCrib;
	} while(thisCrib != SENTINEL);

	cout << "\n\n\n    -----------------------------\n\n";
	cout <<"    Crib average for today: " << (1.0 * total) / (1.0 * numHandsTotal);
	cout << " points (over " << numHandsTotal << " hands).\n\n\n\n";

	return 0;
}