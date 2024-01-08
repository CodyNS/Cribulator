// Uses global variables now. This simplifies the function calls.
// But... this still uses pointers. How can I simplify this further for JS, where you don't have pointers?
// ...probably do some JS experimentation to see what I can do...

// Future ideas?
// - blank save data file format needs updating to include the b4_cribulator pieces and the 3 new montly histogram pieces
// - win margin histogram
// - largest hand and largest crib printout added to end of session screen


#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;


// CONSTANTS (most important ones)
const string SAVE_FILE_NAME = "cribulator_player_save_data.txt";  // save file for current year
const string SAVE_FILE_LEGACY_NAME = "cribulator_legacy_player_data.txt";  // combined save data for all time before this year

const string BLANK_SAVE_FILE_INPUT_NAME = "test_(blank)_save_data.txt";
const string BLANK_SAVE_FILE_OUTPUT_NAME = "test_save_data_output.txt";
const string VAR_PREFIX = "### ";
const int SCREEN_WIDTH = 140;
const int SCREEN_WIDTH_NARROW = 45;
const int MAX_HEIGHT_VERTICAL_HISTO_BAR = 30;



struct Player {
    string name;
    string indtAdjstdName;

    // These 4 only have > 0 values in the legacy data file. They're from times when I wasn't tracking more than just these.
    // Combine them with the rest of this data in the legacy file to get the full legacy totals.
    int wins_b4_cribulator = 0;
    int cribSum_b4_cribulator = 0;  // combined value (Molly + Johnny)
    int numHands_b4_cribulator = 0; // combined value (Molly + Johnny)
    int macroPts_b4_cribulator = 0;

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

    vector<int> macrogamePtsByMonth = {vector<int>(12,0)};
    vector<int> gamesPlayedByMonth = {vector<int>(12,0)};
    vector<int> winsByMonth = {vector<int>(12,0)};

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


// CONSTANTS, continued...
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
const vector<string> CORRECTION_VARIATIONS = {"fix", "correct", "undo", "redo", "mistake", "oops", "woops"};
const vector<string> HISTO_VARIATIONS = {"histo", "hist", "histogram"};
const vector<string> WIN_VARIATIONS = {"w", "win"};
const vector<string> NOBS_VARIATIONS = {"n", "nobs", "knobs"};
const vector<string> FOK_VARIATIONS = {"fok", "four", "foak"};
const vector<string> FLUSH_VARIATIONS = {"f", "fl", "flush"};
const vector<string> SUPER_FLUSH_VARIATIONS = {"sf", "super", "superflush", "super flush"};
const vector<string> END_SESSION_VARIATIONS = {"end", "done", "exit", "fin", "quit"};
const vector<string> NUMBERS = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};


// GLOBAL VARIABLES (gasp)
bool usingBlankSaveData = false;  // flips to true if blank save data file is found
bool userWantsToCorrectRound = false;      // flag used to trigger the correction mechanism
bool userWantsToCorrect1stCribCut = false;
//bool backupWasJustRestored = false;      // another flag part of the correction mechanism
int lastCutCardIndex = -1;      // used as part of the correction mechanism. Needed because of where 
                                // the backup snapshot is taken. We need to correct the crib cut data
                                // for the cribber using this (we -1 for this cut in the cribber's data)
string previousInvalidInput = "";
int previousInvalidInputFix = -1;

// These used to be defined in main() and passed in as parameters, but global scope simplifies the function calls so much.
// For a private, innocent game tracker like this, I just don't care: it's a worthy trade-off.
Player molly; 
Player johnny; 
vector<Player*> players = {&molly, &johnny};
Player mollyBackup;
Player johnnyBackup;
vector<Player*> playerBackups = {&mollyBackup, &johnnyBackup};
Player mollyLegacy;
Player johnnyLegacy;
vector<Player*> playersLegacy = {&mollyLegacy, &johnnyLegacy};
Player* cribber = NULL;
Player* nonCribber = NULL;   
Player* cribberBackup = NULL;
Player* nonCribberBackup = NULL;
Player* playerWhoHadFirstCrib = NULL;
Player* winner = NULL;

time_t now = time(0);
tm *mytime = localtime(&now);
int year = mytime->tm_year + 1900;
int month = mytime->tm_mon;  // 0 to 11
int day = mytime->tm_mday;



// ----- FUNCTION PROTOTYPES --------------------------------------------------
void initializeStuff();
bool userWantsToEndSession(string);
bool cutToSeeWhoGetsFirstCrib();
void assignCribTo(char);
void swapCribbers();
void incrementHandCounters();
bool cutTheStarterCard();
bool handleInputFlags(string, Player*);
Player* winHandler(string, int);
int sumOfHistogram(vector<int>); 
void printEndOfHandStuff();
void updatePlayerDataAfterGame();
void printEndOfGameStuff();
void printPointsBreakdownForPlayerThisGame(Player*);
void resetThisGameDataForPlayers();
string thisMonthsName();
string abbrevForMonth(int);
void printEndOfSessionStuff();
string percentageString(int, int);
double combinedCribAverage(string);
void performEndOfSessionTasks();
int cardIndexOf(string);
string cardStringForIndex(int);
int pointValueOf(string);
vector<int> scaleHistoValuesToFitScreen(vector<int>, int, int, bool, bool);
void printHistogram(string);
double calcWeightedAvgWeird(double, int, int, int);
void printSummaryMessageAtBottomOfVsHisto(bool, bool);
string multString(int, string);
double computeMeanFromHisto(vector<int>);
int domainOfHisto(vector<int>);
int rangeOfHisto(vector<int>);
vector<int> combinedHistoFrom(vector<vector<int>>);
bool search(string, string);
bool search(string, vector<string>);
Player* determineLoser();
void printCentered(int, string, int);
void printCenteredManual(int, string, int, int, int);
vector<int> parseCommaSeparatedValuesFromString(string);
void loadDataForPlayerFromFile(Player*, fstream*);
void loadSaveDataFromFiles();
void saveATDataToFile();
string getlineMine();
void backupVariables();
void restoreBackupOfVariables();
void undoLastDrawForWhoGets1stCrib();
bool userWantsToCorrect1stCribDraw();
int mystoi(string);
bool handleStatsRequests(string);


// ----- MAIN -----------------------------------------------------------------
int main() {

    bool isFirstGame = true;
    string userInput;
    
    initializeStuff();
    //printCentered(3, "* * * *  MOLLY & JOHNNY'S CRIBULATOR!!!  * * * *", 1);  <-- for a more centered presentation
    printCenteredManual(3, "* * *  MOLLY & JOHNNY'S CRIBULATOR!!!  * * *", 3, 45, 44);

    do {  // game loop ------------------------------
        if (!userWantsToCorrect1stCribCut)
            if ( !isFirstGame)  cout << "\nNew game!\n\n";

        if (cutToSeeWhoGetsFirstCrib() == USER_WANTS_TO_END_SESSION)
            break;
        playerWhoHadFirstCrib = cribber;
        cribber->numFirstCribsToday++;
        cribber->numFirstCribsAT++;

        if (userWantsToCorrectRound)
            restoreBackupOfVariables();
        
        do {  // hand loop -----------------------          
            if (cutTheStarterCard() == DEALER_WINS_FROM_NIBS) {
                winner = winHandler("nibs win", 0);
                if (winner != NULL)  break;  // we have a winner
            }
            if (userWantsToCorrectRound) {
                if (userWantsToCorrect1stCribDraw()) {
                    cribber->numFirstCribsToday--;
                    cribber->numFirstCribsAT--;
                    break;
                }
                restoreBackupOfVariables();
                continue;
            }

            backupVariables();
            //backupWasJustRestored = false;  <-- was using this to be efficient, but it was getting tricky so fck it
            incrementHandCounters();

            do {
                cout << nonCribber->indtAdjstdName << "'s hand:  ";
                userInput = getlineMine();
            } while(handleStatsRequests(userInput) );
            if (handleInputFlags(userInput, nonCribber) == SOMEONE_HAS_WON) {
                if (!search(userInput, NUMBERS))
                    winner = winHandler("someone won via pegging", 0);
                else 
                    winner = winHandler("nonCribber wins via hand", pointValueOf(userInput));
                if (winner != NULL)  break;
            }
            if (userWantsToCorrectRound) {
                restoreBackupOfVariables();
                continue;
            }
            int ptValue = pointValueOf(userInput);
            nonCribber->histoHandPtsAT[ptValue]++;
            nonCribber->histoHandPtsToday[ptValue]++;
            nonCribber->handPtsThisGame += ptValue;

            do {
                cout << cribber->indtAdjstdName << "'s hand:  ";
                userInput = getlineMine();
            } while(handleStatsRequests(userInput) );
            if (handleInputFlags(userInput, cribber) == CRIBBER_WINS) {
                winner = winHandler("cribber wins via hand", pointValueOf(userInput));
                if (winner != NULL)  break;
            }
            if (userWantsToCorrectRound) {
                restoreBackupOfVariables();
                continue;
            }
            ptValue = pointValueOf(userInput);
            cribber->histoHandPtsAT[ptValue]++;
            cribber->histoHandPtsToday[ptValue]++;
            cribber->handPtsThisGame += ptValue;
    
            do {
                cout << cribber->indtAdjstdName << "'s crib:  ";
                userInput = getlineMine();
            } while(handleStatsRequests(userInput) );
            if (handleInputFlags(userInput, cribber) == CRIBBER_WINS) {
                winner = winHandler("cribber wins via crib", pointValueOf(userInput));
                if (winner != NULL)  break;
            }
            if (userWantsToCorrectRound) {
                restoreBackupOfVariables();
                continue;
            }
            ptValue = pointValueOf(userInput);
            cribber->histoCribPtsAT[ptValue]++;
            cribber->histoCribPtsToday[ptValue]++;
            cribber->cribPtsThisGame += ptValue;
            
            printEndOfHandStuff(); 
            swapCribbers();
            cout << "\n\n" << cribber->name << "'s crib\n";
            
        } while(!search(userInput, END_SESSION_VARIATIONS));  // hand loop

        if (!userWantsToCorrect1stCribCut) {  // skip if user is correcting the draw to see who gets game's 1st crib
            updatePlayerDataAfterGame();
            printEndOfGameStuff();
            resetThisGameDataForPlayers();
        }

        isFirstGame = false;
    } while(!search(userInput, END_SESSION_VARIATIONS) );  // game loop

    performEndOfSessionTasks();
    return 0;
}




 // ----- FUNCTION DEFINITIONS ------------------------------------------------
void initializeStuff() {
    cout << setprecision(2) << fixed; // 2 decimal places for output  BLAH: should this be global or in main?
    loadSaveDataFromFiles(); // load the players' all-time data
}


bool userWantsToEndSession(string input) {
    if (search(input, END_SESSION_VARIATIONS))
        return true;
    return false;
}


bool cutToSeeWhoGetsFirstCrib() {
// returns false if user indicates they want to quit playing; else true
    string mollyCut;
    string johnnyCut;
    int mollyCardIndex;
    int johnnyCardIndex;

    do {
        cout << " Molly draws a  ";
        mollyCut = getlineMine();
    } while(handleStatsRequests(mollyCut) );
    if (userWantsToEndSession(mollyCut))
        return false;  // quit for today
    if (userWantsToCorrectRound) 
        return true;  // will have been flipped in getlineMine() via the user's input if true
    mollyCardIndex = cardIndexOf(mollyCut);

    do {
        cout << "Johnny draws a  ";
        johnnyCut = getlineMine();
    } while(handleStatsRequests(johnnyCut) );
    if (userWantsToEndSession(johnnyCut))
        return false;  // quit for today
    if (userWantsToCorrectRound) {
        userWantsToCorrectRound = false;  // if correction input is entered here, assume user just wants to correct the cut input
        return cutToSeeWhoGetsFirstCrib(); // just start again if user wants to correct at this step
    }
    johnnyCardIndex = cardIndexOf(johnnyCut);

    molly.histoCutsForFirstCribAT[mollyCardIndex]++;
    molly.cutsForFirstCribToday.push_back(cardStringForIndex(mollyCardIndex));
    johnny.histoCutsForFirstCribAT[johnnyCardIndex]++;
    johnny.cutsForFirstCribToday.push_back(cardStringForIndex(johnnyCardIndex));
    if (mollyCardIndex < johnnyCardIndex)
        assignCribTo('M');
    else  
        assignCribTo('J');
    cout << endl << cribber->name << " gets first crib!\n";
    return true;  // yes, we cut and are going to play a game
}


void assignCribTo(char cribberInitial) {
// BLAH these are using hard-coded values... at least use constants at the top of the program
    if (cribberInitial == 'm' || cribberInitial == 'M') {
        cribber = &molly;
        nonCribber = &johnny;
    }
    else if (cribberInitial == 'j' || cribberInitial == 'J') {
        cribber = &johnny;
        nonCribber = &molly;
    }
    else {
        cout << "\n\nIncorrect cribberInitial passed to \"assignCribTo\" function: \'" 
             << cribberInitial << "\'\nprogram closing..." << endl << endl;
        exit(0);
    }
}


void swapCribbers() {
    Player* temp = cribber;
    cribber = nonCribber;
    nonCribber = temp;
}


void incrementHandCounters() {
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


bool cutTheStarterCard() {
// returns true if the cut results in the cribber winning; false otherwise
// BLAH: refactor the if statement by using a to_upper() function (make one)
    string theCut;  // aka the "starter card" that is cut
    int cutCardIndex;
    do {
        cout << "         the cut:  ";
        theCut = getlineMine();
    } while(handleStatsRequests(theCut) );
    if (userWantsToCorrectRound)
        return false;

    cutCardIndex = cardIndexOf(theCut);
    cribber->histoCutsForMyCribToday[cutCardIndex]++;
    cribber->histoCutsForMyCribAT[cutCardIndex]++;
    lastCutCardIndex = cutCardIndex;
    if ( search(theCut, "j") || search(theCut, "J") || 
         search(theCut, "w") || search(theCut, "W")    ) {  // nibs
        cribber->numNibsThisGame++;
        if (search(theCut, "w") || search(theCut, "W"))  // nibs for the win!
            return true;
    }
    return false;
}


bool handleInputFlags(string userInput, Player* player) {
// returns true if the flags indicate a win; false otherwise
// Be careful about the order you check these, especially the ones with single letter variations...

    if (search(userInput, FOK_VARIATIONS) ){
        player->FOKsToday++;
        player->FOKsAT++;
    } else if (search(userInput, SUPER_FLUSH_VARIATIONS) ){
        player->superFlushesToday++;
        player->superFlushesAT++;
    } else if (search(userInput, FLUSH_VARIATIONS) && !search(userInput, FOK_VARIATIONS) ){ // don't let 'fok' also count as a flush
        player->flushesToday++;
        player->flushesAT++;
    }

    if (search(userInput, NOBS_VARIATIONS) ){
        player->nobsToday++;
        player->nobsAT++;
    }

    if (search(userInput, WIN_VARIATIONS) )
        return true;  // someone has won

    return false; // no win indicated
}


Player* winHandler(string winCondition, int ptsInWinningInput) {
// Calculates all the necessary crap when user indicates a win
// returns a pointer to the winning player
// returns a NULL pointer in the event user wants to make a correction to the hand
    string userInput;  // just going to use this for most of the input and reuse it,
                       // so I'll have mind the order in which I calculate stuff...

    if (search(winCondition, "nibs win") ){
        // printCentered(1, "~ ~ ~  " + cribber->name + " wins!  ~ ~ ~", 3);  <--- for a more centered presentation
        printCenteredManual(2, "~ ~ ~  " + cribber->name + " wins!  ~ ~ ~", 3, 
                            SCREEN_WIDTH_NARROW, string("~ ~ ~  " + cribber->name + " wins!  ~ ~ ~").length());
        cout << "How many points were needed?  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;  // user wants to make a correction; no winner rn
        cribber->unusedPtsThisGame[NIBS] += (2 - mystoi(userInput) );
        cribber->unusedPtsToday[NIBS] += cribber->unusedPtsThisGame[NIBS];
        cribber->unusedPtsAT[NIBS] += cribber->unusedPtsThisGame[NIBS];

        // calc winner's pegged points for this game
        cribber->peggedPtsThisGame = 121 - mystoi(userInput) 
                                         - cribber->handPtsThisGame 
                                         - cribber->cribPtsThisGame 
                                         - ((cribber->numNibsThisGame - 1) * 2); // -1 because we've already incr. numNibsThisGame in cut function                               
        cribber->peggedPtsToday += cribber->peggedPtsThisGame;
        cribber->peggedPtsAt += cribber->peggedPtsThisGame;

        // record loser's position
        cout << " " << nonCribber->indtAdjstdName << "'s pos:  ";  // nonCribber lost
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        nonCribber->losingPositionToday += mystoi(userInput);

        // use that to calc the winner's winning margin
        cribber->winMarginsToday.push_back(121 - mystoi(userInput));
        cribber->histoWinMarginsAT[121 - mystoi(userInput)]++;

        // and now calc loser's pegged points for this game
        nonCribber->peggedPtsThisGame = mystoi(userInput)
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
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        handleInputFlags(userInput, nonCribber);
        nonCribber->handPtsThisGame += mystoi(userInput);
        nonCribber->histoHandPtsToday[mystoi(userInput)]++;
        nonCribber->histoHandPtsAT[mystoi(userInput)]++;
        nonCribber->unusedPtsThisGame[HAND] += mystoi(userInput);
        nonCribber->unusedPtsToday[HAND] += mystoi(userInput);
        nonCribber->unusedPtsAT[HAND] += mystoi(userInput);

        cout << cribber->indtAdjstdName << "'s hand:  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        handleInputFlags(userInput, cribber);
        cribber->handPtsThisGame += mystoi(userInput);
        cribber->histoHandPtsToday[mystoi(userInput)]++;
        cribber->histoHandPtsAT[mystoi(userInput)]++;
        cribber->unusedPtsThisGame[HAND] += mystoi(userInput);
        cribber->unusedPtsToday[HAND] += mystoi(userInput);
        cribber->unusedPtsAT[HAND] += mystoi(userInput);

        cout << cribber->indtAdjstdName << "'s crib:  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        handleInputFlags(userInput, cribber);
        cribber->cribPtsThisGame += mystoi(userInput);
        cribber->histoCribPtsToday[mystoi(userInput)]++;
        cribber->histoCribPtsAT[mystoi(userInput)]++;
        cribber->unusedPtsThisGame[CRIB] += mystoi(userInput);
        cribber->unusedPtsToday[CRIB] += mystoi(userInput);
        cribber->unusedPtsAT[CRIB] += mystoi(userInput);

        return cribber;
    }


    else if (search(winCondition, "someone won via pegging") ){  
    // REFACTOR: there is SO much duplication of the above, below; there has to be a better way of doing this
    // can probably use *winner and *loser Player pointers to cut down on the code
        cout << "Who won?  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        cout << "How many points were needed?  ";
        string ptsNeeded;
        ptsNeeded = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        cout << "How many points would have been pegged if there were space?  ";
        string peggedHypothetically;
        peggedHypothetically = getlineMine();  if (userWantsToCorrectRound)  return NULL;

        if (toupper(cribber->name[0]) == toupper(userInput[0]) ) {  // cribber won
            cribber->unusedPtsThisGame[PEGGED] += (mystoi(peggedHypothetically) - mystoi(ptsNeeded) );
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
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            nonCribber->losingPositionToday += mystoi(userInput);

            // use that to calc the winner's winning margin
            cribber->winMarginsToday.push_back(121 - mystoi(userInput));
            cribber->histoWinMarginsAT[121 - mystoi(userInput)]++;

            // and now calc loser's pegged points for this game
            nonCribber->peggedPtsThisGame = mystoi(userInput)
                                            - nonCribber->handPtsThisGame 
                                            - nonCribber->cribPtsThisGame 
                                            - (nonCribber->numNibsThisGame * 2);                                 
            nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
            nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

            // record the other uncounted and excess points...
            cout << nonCribber->indtAdjstdName << "'s hand:  ";
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            handleInputFlags(userInput, nonCribber);
            nonCribber->handPtsThisGame += mystoi(userInput);
            nonCribber->histoHandPtsToday[mystoi(userInput)]++;
            nonCribber->histoHandPtsAT[mystoi(userInput)]++;
            nonCribber->unusedPtsThisGame[HAND] += mystoi(userInput);
            nonCribber->unusedPtsToday[HAND] += mystoi(userInput);
            nonCribber->unusedPtsAT[HAND] += mystoi(userInput);
            
            cout << cribber->indtAdjstdName << "'s hand:  ";
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            handleInputFlags(userInput, cribber);
            cribber->handPtsThisGame += mystoi(userInput);          
            cribber->histoHandPtsToday[mystoi(userInput)]++;
            cribber->histoHandPtsAT[mystoi(userInput)]++;
            cribber->unusedPtsThisGame[HAND] += mystoi(userInput);
            cribber->unusedPtsToday[HAND] += mystoi(userInput);
            cribber->unusedPtsAT[HAND] += mystoi(userInput);

            cout << cribber->indtAdjstdName << "'s crib:  ";
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            handleInputFlags(userInput, cribber);
            cribber->cribPtsThisGame += mystoi(userInput);
            cribber->histoCribPtsToday[mystoi(userInput)]++;
            cribber->histoCribPtsAT[mystoi(userInput)]++;
            cribber->unusedPtsThisGame[CRIB] += mystoi(userInput);
            cribber->unusedPtsToday[CRIB] += mystoi(userInput);
            cribber->unusedPtsAT[CRIB] += mystoi(userInput);

            return cribber;
        }
        else {  // nonCribber won
            nonCribber->unusedPtsThisGame[PEGGED] += (mystoi(peggedHypothetically) - mystoi(ptsNeeded) );
            nonCribber->unusedPtsToday[PEGGED] += nonCribber->unusedPtsThisGame[PEGGED];
            nonCribber->unusedPtsAT[PEGGED] += nonCribber->unusedPtsThisGame[PEGGED];

            // calc winner's pegged points for this game
            nonCribber->peggedPtsThisGame = 121 - nonCribber->handPtsThisGame 
                                                - nonCribber->cribPtsThisGame 
                                                - (nonCribber->numNibsThisGame * 2)
                                                + nonCribber->unusedPtsThisGame[PEGGED]; // (for same reason given above in 'if' case)                   
            nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
            nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

            // record loser's position
            cout << " " << cribber->indtAdjstdName << "'s pos:  ";  // cribber lost
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            cribber->losingPositionToday += mystoi(userInput);

            // use that to calc the winner's winning margin
            nonCribber->winMarginsToday.push_back(121 - mystoi(userInput));
            nonCribber->histoWinMarginsAT[121 - mystoi(userInput)]++;

            // and now calc loser's pegged points for this game
            cribber->peggedPtsThisGame = mystoi(userInput)
                                            - cribber->handPtsThisGame 
                                            - cribber->cribPtsThisGame 
                                            - (cribber->numNibsThisGame * 2);                                
            cribber->peggedPtsToday += cribber->peggedPtsThisGame;
            cribber->peggedPtsAt += cribber->peggedPtsThisGame;

            // record the other uncounted and excess points...
            cout << nonCribber->indtAdjstdName << "'s hand:  ";
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            handleInputFlags(userInput, nonCribber);
            nonCribber->handPtsThisGame += mystoi(userInput);
            nonCribber->histoHandPtsToday[mystoi(userInput)]++;
            nonCribber->histoHandPtsAT[mystoi(userInput)]++;
            nonCribber->unusedPtsThisGame[HAND] += mystoi(userInput);
            nonCribber->unusedPtsToday[HAND] += mystoi(userInput);
            nonCribber->unusedPtsAT[HAND] += mystoi(userInput);

            cout << cribber->indtAdjstdName << "'s hand:  ";
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            handleInputFlags(userInput, cribber);
            cribber->handPtsThisGame += mystoi(userInput);
            cribber->histoHandPtsToday[mystoi(userInput)]++;
            cribber->histoHandPtsAT[mystoi(userInput)]++;
            cribber->unusedPtsThisGame[HAND] += mystoi(userInput);
            cribber->unusedPtsToday[HAND] += mystoi(userInput);
            cribber->unusedPtsAT[HAND] += mystoi(userInput);
            
            cout << cribber->indtAdjstdName << "'s crib:  ";
            userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
            handleInputFlags(userInput, cribber);
            cribber->cribPtsThisGame += mystoi(userInput);
            cribber->histoCribPtsToday[mystoi(userInput)]++;
            cribber->histoCribPtsAT[mystoi(userInput)]++;
            cribber->unusedPtsThisGame[CRIB] += mystoi(userInput);
            cribber->unusedPtsToday[CRIB] += mystoi(userInput);
            cribber->unusedPtsAT[CRIB] += mystoi(userInput);

            return nonCribber;
        }
    } // someone won via pegging


    else if (search(winCondition, "nonCribber wins via hand") ){
        // printCentered(1, "~ ~ ~  " + nonCribber->name + " wins!  ~ ~ ~", 3); <--- for a more centered presentation
        printCenteredManual(2, "~ ~ ~  " + nonCribber->name + " wins!  ~ ~ ~", 3, 
                            SCREEN_WIDTH_NARROW, string("~ ~ ~  " + nonCribber->name + " wins!  ~ ~ ~").length());
        cout << "How many points were needed?  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        nonCribber->unusedPtsThisGame[HAND] += (ptsInWinningInput - mystoi(userInput) );
        nonCribber->unusedPtsToday[HAND] += nonCribber->unusedPtsThisGame[HAND];
        nonCribber->unusedPtsAT[HAND] += nonCribber->unusedPtsThisGame[HAND];

        // calc winner's pegged points for this game
        nonCribber->peggedPtsThisGame = 121 - mystoi(userInput)
                                            - nonCribber->handPtsThisGame 
                                            - nonCribber->cribPtsThisGame 
                                            - (nonCribber->numNibsThisGame * 2);                             
        nonCribber->peggedPtsToday += nonCribber->peggedPtsThisGame;
        nonCribber->peggedPtsAt += nonCribber->peggedPtsThisGame;

        // record loser's position
        cout << " " << cribber->indtAdjstdName << "'s pos:  ";  // cribber lost
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        cribber->losingPositionToday += mystoi(userInput);

        // use that to calc the winner's winning margin
        nonCribber->winMarginsToday.push_back(121 - mystoi(userInput));
        nonCribber->histoWinMarginsAT[121 - mystoi(userInput)]++;

        // and now calc loser's pegged points for this game
        cribber->peggedPtsThisGame = mystoi(userInput)
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
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        handleInputFlags(userInput, cribber);
        cribber->handPtsThisGame += mystoi(userInput);
        cribber->histoHandPtsToday[mystoi(userInput)]++;
        cribber->histoHandPtsAT[mystoi(userInput)]++;
        cribber->unusedPtsThisGame[HAND] += mystoi(userInput);
        cribber->unusedPtsToday[HAND] += mystoi(userInput);
        cribber->unusedPtsAT[HAND] += mystoi(userInput);
        
        cout << cribber->indtAdjstdName << "'s crib:  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        handleInputFlags(userInput, cribber);
        cribber->cribPtsThisGame += mystoi(userInput);
        cribber->histoCribPtsToday[mystoi(userInput)]++;
        cribber->histoCribPtsAT[mystoi(userInput)]++;
        cribber->unusedPtsThisGame[CRIB] += mystoi(userInput);
        cribber->unusedPtsToday[CRIB] += mystoi(userInput);
        cribber->unusedPtsAT[CRIB] += mystoi(userInput);

        return nonCribber;
    }


    else if (search(winCondition, "cribber wins via hand") ){
        // printCentered(1, "~ ~ ~  " + cribber->name + " wins!  ~ ~ ~", 3);   <-- for a more centered presentation
        printCenteredManual(2, "~ ~ ~  " + cribber->name + " wins!  ~ ~ ~", 3, 
                            SCREEN_WIDTH_NARROW, string("~ ~ ~  " + cribber->name + " wins!  ~ ~ ~").length());

        cout << "How many points were needed?  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        cribber->unusedPtsThisGame[HAND] += (ptsInWinningInput - mystoi(userInput) );
        cribber->unusedPtsToday[HAND] += cribber->unusedPtsThisGame[HAND];
        cribber->unusedPtsAT[HAND] += cribber->unusedPtsThisGame[HAND];

        // calc winner's pegged points for this game
        cribber->peggedPtsThisGame = 121 - mystoi(userInput)
                                         - cribber->handPtsThisGame 
                                         - cribber->cribPtsThisGame 
                                         - (cribber->numNibsThisGame * 2);                           
        cribber->peggedPtsToday += cribber->peggedPtsThisGame;
        cribber->peggedPtsAt += cribber->peggedPtsThisGame;

        // record loser's position
        cout << " " << nonCribber->indtAdjstdName << "'s pos:  ";  // nonCribber lost
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        nonCribber->losingPositionToday += mystoi(userInput);

        // use that to calc the winner's winning margin
        cribber->winMarginsToday.push_back(121 - mystoi(userInput));
        cribber->histoWinMarginsAT[121 - mystoi(userInput)]++;

        // and now calc loser's pegged points for this game
        nonCribber->peggedPtsThisGame = mystoi(userInput)
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
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        handleInputFlags(userInput, cribber);
        cribber->cribPtsThisGame += mystoi(userInput);
        cribber->histoCribPtsToday[mystoi(userInput)]++;
        cribber->histoCribPtsAT[mystoi(userInput)]++;
        cribber->unusedPtsThisGame[CRIB] += mystoi(userInput);
        cribber->unusedPtsToday[CRIB] += mystoi(userInput);
        cribber->unusedPtsAT[CRIB] += mystoi(userInput);

        return cribber;
    }


    else if (search(winCondition, "cribber wins via crib") ){
        // printCentered(1, "~ ~ ~  " + cribber->name + " wins!  ~ ~ ~", 3);   <-- for a more centered presentation
        printCenteredManual(2, "~ ~ ~  " + cribber->name + " wins!  ~ ~ ~", 3, 
                            SCREEN_WIDTH_NARROW, string("~ ~ ~  " + cribber->name + " wins!  ~ ~ ~").length());
        cout << "How many points were needed?  ";
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        cribber->unusedPtsThisGame[CRIB] += (ptsInWinningInput - mystoi(userInput) );
        cribber->unusedPtsToday[CRIB] += cribber->unusedPtsThisGame[CRIB];
        cribber->unusedPtsAT[CRIB] += cribber->unusedPtsThisGame[CRIB];

        // calc winner's pegged points for this game
        cribber->peggedPtsThisGame = 121 - mystoi(userInput)
                                         - cribber->handPtsThisGame 
                                         - cribber->cribPtsThisGame 
                                         - (cribber->numNibsThisGame * 2);                           
        cribber->peggedPtsToday += cribber->peggedPtsThisGame;
        cribber->peggedPtsAt += cribber->peggedPtsThisGame;

        // record loser's position
        cout << " " << nonCribber->indtAdjstdName << "'s pos:  ";  // nonCribber lost
        userInput = getlineMine();  if (userWantsToCorrectRound)  return NULL;
        nonCribber->losingPositionToday += mystoi(userInput);

        // use that to calc the winner's winning margin
        cribber->winMarginsToday.push_back(121 - mystoi(userInput));
        cribber->histoWinMarginsAT[121 - mystoi(userInput)]++;  // check the indexing on this... hmmm.

        // and now calc loser's pegged points for this game
        nonCribber->peggedPtsThisGame = mystoi(userInput)
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


void printEndOfHandStuff() {
// want it to be somethng like this for ex: 
// 4.63 this game    5.46 today (33)    4.41 all-time (2034)
    cout << "\n           crib avg:       ";
    cout << ((players[0]->cribPtsThisGame + players[1]->cribPtsThisGame) * 1.0) / 
            (players[0]->numHandsThisGame * 1.0)
         << " this game     "

         << combinedCribAverage("today") << " today (" << players[0]->numHandsToday << ")     "
         << combinedCribAverage("year") << " year (" << players[0]->numHandsAT << ")";
}


void updatePlayerDataAfterGame() {
    int macrogamePtsThisGame;  // have to update this at game's end and not every hand because we need
                               // to have pegged points calculated first, and that only happens at game's end
    for (Player* p : players) {
        macrogamePtsThisGame = p->handPtsThisGame
                                + p->cribPtsThisGame
                                + (p->numNibsThisGame * 2)
                                + p->peggedPtsThisGame;
        p->macrogamePtsToday += macrogamePtsThisGame;
        p->macrogamePtsAT += macrogamePtsThisGame;
        p->macrogamePtsByMonth[month] += macrogamePtsThisGame;

        p->gamesPlayedToday++;
        p->gamesPlayedAT++;
        p->gamesPlayedByMonth[month]++;
    }
    winner->winsToday++;
    winner->winsAT++;
    winner->winsByMonth[month]++;
    if (winner->name == playerWhoHadFirstCrib->name) {
        winner->numFirstCribsWonToday++;
        winner->numFirstCribsWonAT++;
    }
}


// void printEndOfGameStuff() {  
// // calculate and display the stats you want to at the end of a game:
//     printCentered(1, "Congratulations " + winner->name + "!", 1);
//     string winWord;
//     if (winner->winsToday == 1)
//         winWord = "1st";
//     else if (winner->winsToday == 2)
//         winWord = "2nd";
//     else if (winner->winsToday == 3)
//         winWord = "3rd";
//     else 
//         winWord = "" + to_string(winner->winsToday) + "th";
//     printCentered(0, "That's your " + winWord + " win today!", 3);

//     printCentered(0, "Game " + to_string(players[0]->gamesPlayedToday) + " stats:", 2);
//     // points breakdown for the winner
//     printPointsBreakdownForPlayerThisGame(winner);
//     cout << endl;
//     // points breakdown for the loser
//     // will do it, but problem:  loser's unused pts have been added to their 'ThisGame' variables already,
//     // ... so the numbers won't be correct for them as it stands... how to fix this?
//     printPointsBreakdownForPlayerThisGame(determineLoser());
//     cout << endl;

//     // xxx pts (Leader) to yyy pts (Trailer) in today's macro-game thus far...  


//     // any average/combined stats of interest?


//     cout << endl;
// }                            // ^---- use this version for a more centered presentation
void printEndOfGameStuff() {
// calculate and display the stats you want to at the end of a game:
    printCenteredManual(2, "Congratulations " + winner->name + "!", 1, 
                        SCREEN_WIDTH_NARROW, string("Congratulations " + winner->name + "!").length());
    string winWord;
    if (winner->winsToday == 1)
        winWord = "1st";
    else if (winner->winsToday == 2)
        winWord = "2nd";
    else if (winner->winsToday == 3)
        winWord = "3rd";
    else 
        winWord = "" + to_string(winner->winsToday) + "th";
    printCenteredManual(0, "That's your " + winWord + " win today!", 4, 
                        SCREEN_WIDTH_NARROW, string("That's your " + winWord + " win today!").length());

    printCenteredManual(0, "Game " + to_string(players[0]->gamesPlayedToday) + " stats:", 2, 
                        SCREEN_WIDTH_NARROW, string("Game " + to_string(players[0]->gamesPlayedToday)).length());
    // points breakdown for the winner
    printPointsBreakdownForPlayerThisGame(winner);
    cout << endl;
    // points breakdown for the loser
    // will do it, but problem:  loser's unused pts have been added to their 'ThisGame' variables already,
    // ... so the numbers won't be correct for them as it stands... how to fix this?
    printPointsBreakdownForPlayerThisGame(determineLoser());
    cout << endl;

    // xxx pts (Leader) to yyy pts (Trailer) in today's macro-game thus far...  


    // any average/combined stats of interest?


    cout << endl;
}


void printPointsBreakdownForPlayerThisGame(Player* p) {
// BLAH: use printf() for this to simplify the formatting
    cout << setprecision(0) << fixed;
    // const string BD_INDENT = "                                    "; <-- for a more centered presentation
    const string BD_INDENT = "                        ";

    int strictHandPts = p->handPtsThisGame - p->unusedPtsThisGame[HAND];
    int strictCribPts = p->cribPtsThisGame - p->unusedPtsThisGame[CRIB];
    int strictNibsPts = (p->numNibsThisGame * 2) - p->unusedPtsThisGame[NIBS];
    int strictPeggedPts = p->peggedPtsThisGame - p->unusedPtsThisGame[PEGGED];
    int finPos = strictHandPts + strictCribPts + strictNibsPts + strictPeggedPts;

    double handPercent = (strictHandPts * 100.0) / (finPos * 1.0);
    double cribPercent = (strictCribPts * 100.0) / (finPos * 1.0);
    double nibsPercent = (strictNibsPts * 100.0) / (finPos * 1.0);
    double peggedPercent = (strictPeggedPts * 100.0) / (finPos * 1.0);

    // cout << "           "                 <-- for a more centered presentation
    cout << ""
         << ((finPos > 99) ? "" : " ") << p->indtAdjstdName << "'s " 
         << finPos << " pts:" << endl;

    cout << BD_INDENT << strictHandPts << "  hands"
         << ((handPercent > 9.5) ? string(5, ' ') : string(6, ' '))
         << handPercent << "%\n";

    cout << BD_INDENT << ((strictCribPts >= 10) ? "" : " ")
         << strictCribPts << "  cribs"
         << ((cribPercent > 9.5) ? string(5, ' ') : string(6, ' '))
         << cribPercent << "%\n";
         
    if (strictNibsPts > 0)
        cout << BD_INDENT << " " << strictNibsPts << "  nibs       "
             << nibsPercent << "%\n";

    cout << BD_INDENT << ((strictPeggedPts >= 10) ? "" : " ")
         << strictPeggedPts << "  pegging"
         << ((peggedPercent > 9.5) ? string(3, ' ') : string(4, ' '))
         << peggedPercent << "%\n";

    cout << setprecision(2) << fixed; 
}


void resetThisGameDataForPlayers() {
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


double combinedCribAverage(string timeframe) {
    if (timeframe == "today")
        return computeMeanFromHisto(combinedHistoFrom({players[0]->histoCribPtsToday, players[1]->histoCribPtsToday}));

    else if (timeframe == "year")
        return computeMeanFromHisto(combinedHistoFrom({players[0]->histoCribPtsAT, players[1]->histoCribPtsAT}));

    else if (timeframe == "all-time")
        return ( (sumOfHistogram(players[0]->histoCribPtsAT) + sumOfHistogram(playersLegacy[0]->histoCribPtsAT)
                  + sumOfHistogram(players[1]->histoCribPtsAT) + sumOfHistogram(playersLegacy[1]->histoCribPtsAT)
                  + playersLegacy[0]->cribSum_b4_cribulator ) * 1.0) 
                / 
                  ((players[0]->numHandsAT + playersLegacy[0]->numHandsAT + playersLegacy[0]->numHands_b4_cribulator) * 1.0);

    else {
        cout << "\n\n\n*** Error: invalid timeframe specified for 'combinedCribAverage()' call ***\n";
        cout << "program closing...\n\n\n";
        exit(1);
    }
}


void performEndOfSessionTasks() {
// handle stuff before exiting program
    for (Player* p : players) {
        p->numSessionsPlayedAT++;
    }

    printEndOfSessionStuff();
    saveATDataToFile();
}


string thisMonthsName() {
    switch(month) {
        case 0:  return "January";
        case 1:  return "February";
        case 2:  return "March";
        case 3:  return "April";
        case 4:  return "May";
        case 5:  return "June";
        case 6:  return "July";
        case 7:  return "August";
        case 8:  return "September";
        case 9:  return "October";
        case 10: return "November";
        case 11: return "December";
        default: return "oops... invalid month number";
    }
}


string abbrevForMonth(int monthIndex) {
    switch(monthIndex) {
        case 0:  return "Jan";
        case 1:  return "Feb";
        case 2:  return "Mar";
        case 3:  return "Apr";
        case 4:  return "May";
        case 5:  return "Jun";
        case 6:  return "Jul";
        case 7:  return "Aug";
        case 8:  return "Sep";
        case 9:  return "Oct";
        case 10: return "Nov";
        case 11: return "Dec";
        default: return "oops...";
    }
}


void printEndOfSessionStuff() {
// display the stats you want to at the end of the day's session:
    printCentered(2, "__________________________________________________________", 2);
    printCentered(0, "WINS", 1);
    printCentered(0, "__________________________________________________________", 2);

    // print today's wins
    if (players[0]->winsToday >= players[1]->winsToday) {
        printCentered(0, "Today's champion: " + 
            ((players[0]->winsToday == players[1]->winsToday) ? "It's a tie" : players[0]->name) + "!", 2);
        printCentered(0, to_string(players[0]->winsToday) + " wins for " + players[0]->name + "  vs  " +
             to_string(players[1]->winsToday) + " wins for " + players[1]->name, 3);
    } else {
        printCentered(0, "Today's champion: " + players[1]->name + "!", 2);
        printCentered(0, to_string(players[1]->winsToday) + " wins for " + players[1]->name + "  vs  " +
             to_string(players[0]->winsToday) + " wins for " + players[0]->name, 3);
    }


    // print this month's wins
    int thisMonthWinsP0 = players[0]->winsByMonth[month];
    int thisMonthWinsP1 = players[1]->winsByMonth[month];
    printCentered(0, "in " + thisMonthsName() + ":", 2);
    if (thisMonthWinsP0 >= thisMonthWinsP1)
        printCentered(0, players[0]->name + " leads with " + to_string(thisMonthWinsP0) + " wins vs " +
            to_string(thisMonthWinsP1) + " for " + players[1]->name, 3);
    else    
        printCentered(0, players[1]->name + " leads with " + to_string(thisMonthWinsP1) + " wins vs " +
            to_string(thisMonthWinsP0) + " for " + players[0]->name, 3);


    // print this year's wins
    int allTimeWinsP0 = players[0]->winsAT;
    int allTimeWinsP1 = players[1]->winsAT;
    int diff = abs(allTimeWinsP0 - allTimeWinsP1);
    printCentered(0, "This Year:", 2);
    if (allTimeWinsP0 >= allTimeWinsP1)
        printCentered(0, players[0]->name + " leads with " + to_string(allTimeWinsP0) + " wins vs " +
            to_string(allTimeWinsP1) + " for " + players[1]->name, 1);
    else    
        printCentered(0, players[1]->name + " leads with " + to_string(allTimeWinsP1) + " wins vs " +
            to_string(allTimeWinsP0) + " for " + players[0]->name, 1);

    if (diff > 0)  printCentered(0, "(" + to_string(diff) + " game lead)" , 2);
    else  printCentered(0, "(tied!)" , 2);





    printCentered(0, "__________________________________________________________", 2);
    printCentered(0, "MACRO-GAME", 1);
    printCentered(0, "__________________________________________________________", 2);


    // print macrogame results for today
    printCentered(0, "Today:", 2);
    if (players[0]->macrogamePtsToday >= players[1]->macrogamePtsToday) {
        printCentered(0, players[0]->name + " won with " + to_string(players[0]->macrogamePtsToday) + " pts vs " +
             to_string(players[1]->macrogamePtsToday) + " for " + players[1]->name, 1);
        printCentered(0, "(" + to_string(players[0]->macrogamePtsToday - players[1]->macrogamePtsToday) + " pt gain for " + players[0]->name + "!)", 3);
    }
    else {
        printCentered(0, players[1]->name + " won with " + to_string(players[1]->macrogamePtsToday) + " pts vs " +
             to_string(players[0]->macrogamePtsToday) + " for " + players[0]->name, 1);
        printCentered(0, "(" + to_string(players[1]->macrogamePtsToday - players[0]->macrogamePtsToday) + " pt gain for " + players[1]->name + "!)", 3);
    }


    // print macrogame results for this month
    int macroPtsMnthP0 = players[0]->macrogamePtsByMonth[month];
    int macroPtsMnthP1 = players[1]->macrogamePtsByMonth[month];
    printCentered(0, "in " + thisMonthsName() + ":", 2);
    if (macroPtsMnthP0 >= macroPtsMnthP1) {
        printCentered(0, players[0]->name + " leads with " + to_string(macroPtsMnthP0) + " pts vs " +
            to_string(macroPtsMnthP1) + " for " + players[1]->name, 1);
        printCentered(0, "(" + to_string(macroPtsMnthP0 - macroPtsMnthP1) + " pt lead)", 3);
        
    }
    else {
        printCentered(0, players[1]->name + " leads with " + to_string(macroPtsMnthP1) + " pts vs " +
            to_string(macroPtsMnthP0) + " for " + players[0]->name, 1);
        printCentered(0, "(" + to_string(macroPtsMnthP1 - macroPtsMnthP0) + " pt lead)", 3);
    }


    // print macrogame results for this year
    printCentered(0, "This Year:", 2);
    if (players[0]->macrogamePtsAT >= players[1]->macrogamePtsAT) {
        printCentered(0, players[0]->name + " leads with " + to_string(players[0]->macrogamePtsAT) + " pts vs " +
            to_string(players[1]->macrogamePtsAT) + " for " + players[1]->name, 1);
        printCentered(0, "(" + to_string(players[0]->macrogamePtsAT - players[1]->macrogamePtsAT) + " pt lead)", 2);
        
    }
    else {
        printCentered(0, players[1]->name + " leads with " + to_string(players[1]->macrogamePtsAT) + " pts vs " +
            to_string(players[0]->macrogamePtsAT) + " for " + players[0]->name, 1);
        printCentered(0, "(" + to_string(players[1]->macrogamePtsAT - players[0]->macrogamePtsAT) + " pt lead)", 2);
    }


    // if I decide to show the truly-all-time stats, modify the below code (right now, is just a copy of the above piece)
    // // print macrogame results AT
    // printCentered(0, "All-time MACRO-game (2024):", 2);
    // if (players[0]->macrogamePtsAT >= players[1]->macrogamePtsAT) {
    //     printCentered(0, players[0]->name + ": " + to_string(players[0]->macrogamePtsAT) + " pts  vs  " +
    //         to_string(players[1]->macrogamePtsAT) + " pts for " + players[1]->name, 2);
    //     printCentered(0, "(" + to_string(players[0]->macrogamePtsAT - players[1]->macrogamePtsAT) + " pt lead)", 4);
        
    // }
    // else {
    //     printCentered(0, players[1]->name + ": " + to_string(players[1]->macrogamePtsAT) + " pts  vs  " +
    //         to_string(players[0]->macrogamePtsAT) + " pts for " + players[0]->name, 2);
    //     printCentered(0, "(" + to_string(players[1]->macrogamePtsAT - players[0]->macrogamePtsAT) + " pt lead)", 4);
    // }
    


    printCentered(0, "__________________________________________________________", 2);
    printCentered(0, "OTHER STATS", 1);
    printCentered(0, "__________________________________________________________", 3);


    // print crib averages
    string todayCribAvg = to_string(combinedCribAverage("today"));
    todayCribAvg = todayCribAvg.substr(0, todayCribAvg.size() - 4);  // we get 6 decimal pts by default; cut off 4 of them

    string yearCribAvg = to_string(combinedCribAverage("year"));
    yearCribAvg = yearCribAvg.substr(0, yearCribAvg.size() - 4);

    string alltimeCribAvg = to_string(combinedCribAverage("all-time"));
    alltimeCribAvg = alltimeCribAvg.substr(0, alltimeCribAvg.size() - 4); 

    printCentered(0, "Today's crib avg:  " + todayCribAvg + " pts  (" + 
        to_string(players[0]->numHandsToday) + " hands)", 2); 

    printCentered(0, to_string(year) + " crib avg:  " + yearCribAvg + " pts  (" +
        to_string(players[0]->numHandsAT) + " hands)", 2);

    printCentered(0, "All-time crib avg:  " + alltimeCribAvg + " pts  (" +
        to_string(players[0]->numHandsAT + playersLegacy[0]->numHandsAT + playersLegacy[0]->numHands_b4_cribulator) + 
        " hands)", 4);



    // print number of first crib wins
    printCentered(0, "First crib advantage today?", 2);

    printCentered(0, players[0]->name + " won " + to_string(players[0]->numFirstCribsWonToday) +
        " of " + to_string(players[0]->numFirstCribsToday) + " games when she had first crib" + 
        percentageString(players[0]->numFirstCribsWonToday, players[0]->numFirstCribsToday), 1);

    printCentered(0, players[1]->name + " won " + to_string(players[1]->numFirstCribsWonToday) +
        " of " + to_string(players[1]->numFirstCribsToday) + " games when he had first crib" +
        percentageString(players[1]->numFirstCribsWonToday, players[1]->numFirstCribsToday), 2);

    printCentered(0, "This Year:", 2);

    printCentered(0, players[0]->name + " has won " + to_string(players[0]->numFirstCribsWonAT) +
        " of " + to_string(players[0]->numFirstCribsAT) + " games when she had first crib" +
        percentageString(players[0]->numFirstCribsWonAT, players[0]->numFirstCribsAT), 1);

    printCentered(0, players[1]->name + " has won " + to_string(players[1]->numFirstCribsWonAT) +
        " of " + to_string(players[1]->numFirstCribsAT) + " games when he had first crib" +
        percentageString(players[1]->numFirstCribsWonAT, players[1]->numFirstCribsAT), 7);
    // ^ could improve this by adding gender/pronoun to player struct so those words aren't hard-coded...
    // Only necessary if you going to generalize the game for different people. IMPROVE. BLAH.
}


string percentageString(int numerator, int denominator){
    if (denominator == 0)
        return ".";

    double num = (double)numerator, den = (double)denominator;
    double quotient = num / den;
    double percentage = quotient * 100.0;
    string percentageStr = to_string(percentage);
    percentageStr = percentageStr.substr(0, percentageStr.find(".") + 2);  // gives 1 decimal place
    return "  (" + percentageStr + "%)";
}


int cardIndexOf(string card) {
// order is important. You'll want to search for 10 before you search for 1...

// BLAH: also might want to check for erronious entries. What if 16 is entered, for example?
// ...in this case, it will catch as a 1 and be entered as an Ace
    if (search(card, "k"))  return 12;
    if (search(card, "K"))  return 12;
    if (search(card, "q"))  return 11;
    if (search(card, "Q"))  return 11;
    if (search(card, "j"))  return 10;  if (search(card, "w"))  return 10;  // might happen
    if (search(card, "J"))  return 10;  if (search(card, "W"))  return 10;
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
    if (search(card, "A"))  return 0;

    // printCentered(1, "Error: '" + card + "' is not a valid card", 2);  <-- for a more centered presentation
    printCenteredManual(1, "Error: '" + card + "' is not a valid card", 2, 
                        SCREEN_WIDTH_NARROW, string("Error: '" + card + "' is not a valid card").length());
    cout << "What card did you mean?  ";
    string secondTry = getlineMine();
    return cardIndexOf(secondTry);
}


string cardStringForIndex(int cardIndex) {
// converts a card index to its string representation (goes in the opposite direction as cardIndexOf() )
    if (12 == cardIndex)  return "K";
    if (11 == cardIndex)  return "Q";
    if (10 == cardIndex)  return "J";
    if (9  == cardIndex)  return "10";
    if (8  == cardIndex)  return "9";
    if (7  == cardIndex)  return "8";
    if (6  == cardIndex)  return "7";
    if (5  == cardIndex)  return "6";
    if (4  == cardIndex)  return "5";
    if (3  == cardIndex)  return "4";
    if (2  == cardIndex)  return "3";
    if (1  == cardIndex)  return "2";
    if (0  == cardIndex)  return "A";
    
    // printCentered(2, "Error: '" + to_string(cardIndex) + "' is not a valid card index", 2);  <-- for a more centered presentation
    cout << "\n Error: '" + to_string(cardIndex) + "' is not a valid card index\n\n";
    cout << "Exiting program...\n\n\n";
    exit(1);
}


int pointValueOf(string userInput) {
// this is used to strip out any flags or excess present in userInput
// order is important. You'll want to search for 10 before you search for 1...

// BLAH: also might want to check for erronious entries. What if 19 is entered, for example?
// ...in this case, it will catch as a 1 and be entered as an Ace, which is not what we want
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

    // printCentered(2, "Error: '" + userInput + "' is not a possible score", 2);  <-- for a more centered presentation
    cout << "\n\n Error: '" + userInput + "' is not a possible score\n\n";
    cout << "   Re-enter value:  ";
    string secondTry = getlineMine();
    return pointValueOf(secondTry);
}


vector<int> scaleHistoValuesToFitScreen(vector<int> histo, int domain, int range, bool isHorizontal, bool isForVsGraph) {
    int marginNeededForLabels = isForVsGraph ? 20 : 14;
    int sizeBiggestBar = isHorizontal ? (SCREEN_WIDTH - marginNeededForLabels) : MAX_HEIGHT_VERTICAL_HISTO_BAR;
    for (int i = 0; i <= domain; i++)
        histo[i] = (int)((double)histo[i] / (double)range * (double)sizeBiggestBar);
    return histo;
}


double calcWeightedAvgWeird(double avg1, int size1, int additionalSum, int size2) {
    return ((avg1 * (size1 * 1.0)) + (additionalSum * 1.0)) / ((size1 + size2) * 1.0);
}


void printHistogram(string input) {
// BLAH REFACTOR this: there's tons of repetition here. Could just declare a struct in 
// here and then pass a pointer to an instance of that into a helper function that
// calculates all that stuff in each case.
    vector<string> mollyVariations      = {"mo", "M"};  // default is combined
    vector<string> johnnyVariations     = {"j", "J"};
    vector<string> winsVsVariations     = {"win"};
    vector<string> macrosVsVariations   = {"mac"};
    vector<string> handVariations       = {"han"};  // default is crib
    vector<string> todayVariations      = {"tod", "td", "TD", "Tod"};  // default is this year
    vector<string> alltimeVariations    = {"all"};
    vector<string> thickVariations      = {"th", "wi", "bi", "fat", "chub"};  // default is single bar width
    vector<string> totalsVariations     = {"am", "nu", "#", "tot"};  // default doesn't show totals
    vector<string> horizontalVariations = {"hor"};  // default is vertical presentation
    vector<int> histo;
    vector<int> histoScaled;
    vector<int> histo2 = {};
    vector<int> histo2Scaled = {};
    int domain = 0;  // highest point total achieved in the histogram(s) of interest
    int range = 0;  // highest value found throughout the domain of the histogram(s) of interest
    int extraIndent = 0;
    bool winsVsHisto = search(input, winsVsVariations);
    bool macrosVsHisto = search(input, macrosVsVariations);
    bool handHisto = search(input, handVariations);
    bool showMolly = search(input, mollyVariations);
    bool showJohnny = search(input, johnnyVariations);
    bool forToday = search(input, todayVariations);
    bool forAllTime = search(input, alltimeVariations);
    bool presentHorizontally = search(input, horizontalVariations);
    bool useThickBars = search(input, thickVariations);
    bool haveToCalcAllTimeCombinedCribAvg = false;
    //bool showAmounts = search(input, totalsVariations);
    string barChar = presentHorizontally ? "■" : "█"; // useThickBars ? "█": "■";
    string barCharMolly = "█";
    string barCharJohnny = "▒";
    string barCharFractional = presentHorizontally ? "▪" : "▄";
    //string headDec = presentHorizontally ? "-----------" : "";  // header decoration
    string headDec = "----------------";
    string header;

    if (winsVsHisto || macrosVsHisto) {  // these two are a bit different 
        if (winsVsHisto) {
            if (forAllTime) {
                histo  = combinedHistoFrom({molly.winsByMonth, mollyLegacy.winsByMonth});
                histo2 = combinedHistoFrom({johnny.winsByMonth, johnnyLegacy.winsByMonth});
                header = "Wins by Month (since Jan 1, 2024)";
            }
            else {  // this year
                histo  = molly.winsByMonth;
                histo2 = johnny.winsByMonth;
                header = "Wins by Month (" + to_string(year) + ")";
            }
            
        }
        else {  // macrosVsHisto
            if (forAllTime) {
                histo  = combinedHistoFrom({molly.macrogamePtsByMonth, mollyLegacy.macrogamePtsByMonth});
                histo2 = combinedHistoFrom({johnny.macrogamePtsByMonth, johnnyLegacy.macrogamePtsByMonth});
                header = "MACRO-GAME points by month (since Jan 1, 2024)";
            }
            else {  // this year
                histo  = molly.macrogamePtsByMonth;
                histo2 = johnny.macrogamePtsByMonth;
                header = "MACRO-GAME points by month (" + to_string(year) + ")";
            }
        }
    }
    else if (showMolly) {
        if (handHisto){
            if (forToday){
                histo = molly.histoHandPtsToday;
                header = "Molly's hands, today (" + to_string(molly.numHandsToday) + ")";
            } else if (forAllTime) {
                histo = combinedHistoFrom({molly.histoHandPtsAT, mollyLegacy.histoHandPtsAT});
                header = "Molly's hands, all-time (" + to_string(molly.numHandsAT + mollyLegacy.numHandsAT) + ")";
            } else {  // default: this year
                histo = molly.histoHandPtsAT;
                header = "Molly's hands in " + to_string(year) + " (" + to_string(molly.numHandsAT) + ")";
            }
        } else { // default: data for cribs
            if (forToday){
                histo = molly.histoCribPtsToday;
                header = "Molly's cribs today (" + to_string(molly.numCribsToday) + ")";
            } else if (forAllTime) {
                histo = combinedHistoFrom({molly.histoCribPtsAT, mollyLegacy.histoCribPtsAT});
                header = "Molly's cribs, all-time (" + to_string(molly.numCribsAT + mollyLegacy.numCribsAT) + ")";
            } else {  // this year
                histo = molly.histoCribPtsAT;
                header = "Molly's cribs in " + to_string(year) + " (" + to_string(molly.numCribsAT) + ")";
            }
        }
    }
    else if (showJohnny) {
        if (handHisto){
            if (forToday){
                histo = johnny.histoHandPtsToday;
                header = "Johnny's hands, today (" + to_string(johnny.numHandsToday) + ")";
            } else if (forAllTime) {
                histo = combinedHistoFrom({johnny.histoHandPtsAT, johnnyLegacy.histoHandPtsAT});
                header = "Johnny's hands, all-time (" + to_string(johnny.numHandsAT + johnnyLegacy.numHandsAT) + ")";
            } else {  // default: this year
                histo = johnny.histoHandPtsAT;
                header = "Johnny's hands in " + to_string(year) + " (" + to_string(johnny.numHandsAT) + ")";
            }
        } else { // default: data for cribs
            if (forToday){
                histo = johnny.histoCribPtsToday;
                header = "Johnny's cribs today (" + to_string(johnny.numCribsToday) + ")";
            } else if (forAllTime) {
                histo = combinedHistoFrom({johnny.histoCribPtsAT, johnnyLegacy.histoCribPtsAT});
                header = "Johnny's cribs, all-time (" + to_string(johnny.numCribsAT + johnnyLegacy.numCribsAT) + ")";
            } else {  // this year
                histo = johnny.histoCribPtsAT;
                header = "Johnny's cribs in " + to_string(year) + " (" + to_string(johnny.numCribsAT) + ")";
            }
        }
    }
    else {  // default is combined (both players)
        if (handHisto){
            if (forToday){
                histo = combinedHistoFrom({molly.histoHandPtsToday, johnny.histoHandPtsToday});
                header = "Today's hands (" + to_string(molly.numHandsToday) + ")";
            } else if (forAllTime) {
                histo = combinedHistoFrom({
                            combinedHistoFrom({molly.histoHandPtsAT, mollyLegacy.histoHandPtsAT}),
                            combinedHistoFrom({johnny.histoHandPtsAT, johnnyLegacy.histoHandPtsAT})
                        });
                header = "Hands, all-time (" + 
                         to_string(molly.numHandsAT + mollyLegacy.numHandsAT + mollyLegacy.numHands_b4_cribulator) + ")";
            } else {  // this year
                histo = combinedHistoFrom({molly.histoHandPtsAT, johnny.histoHandPtsAT});
                header = "Hands in " + to_string(year) + " (" + to_string(molly.numHandsAT) + ")";
            }
        } else {  // default: data for cribs
            if (forToday){
                histo = combinedHistoFrom({molly.histoCribPtsToday, johnny.histoCribPtsToday});
                header = "Today's cribs (" + to_string(molly.numHandsToday) + ")";
            } else if (forAllTime) {
                histo = combinedHistoFrom({
                            combinedHistoFrom({molly.histoCribPtsAT, mollyLegacy.histoCribPtsAT}),
                            combinedHistoFrom({johnny.histoCribPtsAT, johnnyLegacy.histoCribPtsAT})
                        });
                header = "Cribs, all-time (" + 
                         to_string(molly.numHandsAT + mollyLegacy.numHandsAT + mollyLegacy.numHands_b4_cribulator) + ")";
                haveToCalcAllTimeCombinedCribAvg = true;
            } else {  // this year
                histo = combinedHistoFrom({molly.histoCribPtsAT, johnny.histoCribPtsAT});
                header = "Cribs in " + to_string(year) + " (" + to_string(molly.numHandsAT) + ")";
            }
        }
    }

    domain = domainOfHisto(histo);
    range = rangeOfHisto(histo);
    if ( ! histo2.empty()) {  // when printing a WinsVs or MacrosVs graph...
        range = max(range, rangeOfHisto(histo2));
        histo2Scaled = scaleHistoValuesToFitScreen(histo2, domain, range, presentHorizontally, true);
    }
    histoScaled = scaleHistoValuesToFitScreen(histo, domain, range, presentHorizontally, !histo2.empty() );

    if (domain < 10)        extraIndent += 1;
    else if (useThickBars)  extraIndent += 2;

    

    // Similar code to what's below it, but different enough that I'm just going to duplicate the code and make the needed tweaks to it instead of making some uber-complicated combined code. Simpler for me now, initially. Harder to maintain in the future? Maybe. Maybe not. Printing highly formatted output like this is always going to be messy. I'm not going to nitpick.
    if (winsVsHisto || macrosVsHisto) {
        printCentered(5, headDec + "  " + header + "  " + headDec, 2);
        if (presentHorizontally) {
            printCenteredManual(0, multString(3, barCharMolly)  + " = Molly  (top bar)", 1, 140, 24);
            printCenteredManual(0, multString(3, barCharJohnny) + " = Johnny (bottom bar)", 4, 140, 24);
            for (int month = 0; month <= domain; month++) { // BLAH should either make it 1 to 12 or 'Jan' to 'Dec'
                string bar = "", bar2 = "";
                if (histo[month] > 0)
                    bar = histoScaled[month] > 0 ? multString(histoScaled[month], barCharMolly) : barCharMolly;
                if (histo2[month] > 0)
                    bar2 = histo2Scaled[month] > 0 ? multString(histo2Scaled[month], barCharJohnny) : barCharJohnny;
                cout << " " << abbrevForMonth(month) << "  ";
                cout << bar  << " " << to_string(histo[month])  << "  \n";
                cout << "      " << bar2 << " " << to_string(histo2[month]) << "  \n\n\n";
            }
        } else {  // present it vertically
            // printCenteredManual(0, multString(3, barCharMolly)  + " = Molly  (left bar)", 1, 140, 24);
            // printCenteredManual(0, multString(3, barCharJohnny) + " = Johnny (right bar)", 5, 140, 24);
            printCenteredManual(0, "Molly " + multString(3, barCharMolly) + "  " + multString(3, barCharJohnny) + " Johnny", 5, 140, 22);
            const string INDENT = string((SCREEN_WIDTH - (11*(domain+1)))/2 + 2, ' ');

            // top scale ------------------------------------------------------
            // cout << INDENT;  
            // for (int month = 0; month <= domain; month++)
            //     cout << "  " << abbrevForMonth(month) + "      ";
            // printCentered(1, string(11*(domain+1), '-'), 2);

            // the bars -------------------------------------------------------
            for (int freq = MAX_HEIGHT_VERTICAL_HISTO_BAR+1; freq > 0; freq--) {  // +1 to fit the number ontop of the bar
                cout << INDENT;
                for (int month = 0; month <= domain; month++) {
                    // Molly
                    if (freq == 1 && histo[month] > 0 && histoScaled[month] == 0)  // tiny, fractional bar
                        printf("%-4s", multString(3, barCharMolly).c_str());  // used to use 'barCharFractional', but there's not equivalent for Johnny's color
                    else if (histoScaled[month] == freq-1) // time to print the number
                        printf("%-4d", histo[month]);
                    else 
                        cout << (histoScaled[month] >= freq ? multString(3, barCharMolly) + " " : "    ");

                    // Johnny
                    if (freq == 1 && histo2[month] > 0 && histo2Scaled[month] == 0) // tiny, fractional bar
                        printf("%-7s", multString(3, barCharJohnny).c_str());
                    else if (histo2Scaled[month] == freq-1) // time to print the number
                        printf("%-7d", histo2[month]);
                    else 
                        cout << (histo2Scaled[month] >= freq ? multString(3, barCharJohnny) + "    " : "       ");
                }
                cout << endl;
            }

            // bottom scale ---------------------------------------------------
            printCentered(0, string(11*(domain+1), '-'), 1);
            
            // cout << INDENT;
            // for (int month = 0; month <= domain; month++)
            //     cout << " M   J     ";

            cout << INDENT;
            for (int month = 0; month <= domain; month++)
                cout << "  " << abbrevForMonth(month) + "      ";
            cout << "\n\n\n"; // for consistency with horizontal presentations
        }

        // vs total printout at very bottom -----------------------------------
        printSummaryMessageAtBottomOfVsHisto(winsVsHisto, forAllTime); // don't need to pass in macrosVsHisto; can tell without it

        cout << multString(7, "\n");
        return;
    }



    // ----- not a vs histogram but rather a Crib or Hand histogram...
    if (presentHorizontally) {
        printCentered(5, headDec + "  " + header + "  " + headDec, 4);
        for (int ptVal = 0; ptVal <= domain; ptVal++) {
            string bar = "";
            if (histo[ptVal] > 0)
                bar = histoScaled[ptVal] > 0 ? multString(histoScaled[ptVal], barChar) : barChar; // : barCharFractional; <-- shows smaller one for 0 < x < 1 scaled value (changed this to normal one because it looks cleaner)

            if (useThickBars)  cout << "\n      " << bar << endl;
            cout << (ptVal < 10 ? "   " : "  ") << ptVal << "  ";
            // cout << bar << (showAmounts && histo[ptVal] > 0 ? " " + to_string(histo[ptVal]) : "") << endl; // make showing the totals optional
            cout << bar << (histo[ptVal] > 0 ? " " + to_string(histo[ptVal]) : "") << endl;
        }

    } else {  // present it vertically
        printCentered(5, headDec + "  " + header + "  " + headDec, 3);
        const string INDENT = useThickBars ? string((SCREEN_WIDTH-(4 * domain+1))/2, ' ') : string((SCREEN_WIDTH-(3 * domain+1))/2, ' ');

        // top scale ----------------------------------------------------------
        cout << INDENT;
        for (int ptVal = 0; ptVal <= domain; ptVal++) {
            if (useThickBars)
                cout << (ptVal < 10 ? to_string(ptVal) + "   " : to_string(ptVal) + "  ");
            else
                cout << (ptVal < 10 ? to_string(ptVal) + "  " : to_string(ptVal) + " ");
        }
        printCentered(1, (useThickBars ?  string(4*domain + 4 + extraIndent, '-') : string(3*domain + 4 + extraIndent, '-')), 2);

        // the bars -----------------------------------------------------------
        for (int freq = MAX_HEIGHT_VERTICAL_HISTO_BAR; freq > 0; freq--) {
            cout << INDENT;
            for (int ptVal = 0; ptVal <= domain; ptVal++) {
                if (freq == 1 && histo[ptVal] > 0 && histoScaled[ptVal] == 0)
                    cout << barCharFractional + barCharFractional + (useThickBars ? "  " : " ");
                else {
                    if (useThickBars)
                        cout << (histoScaled[ptVal] >= freq ? barChar + barChar + "  " : "    ");
                    else 
                        cout << (histoScaled[ptVal] >= freq ? barChar + barChar + " " : "   ");
                }  
            }
            cout << endl;
        }

        // bottom scale -------------------------------------------------------
        printCentered(0, (useThickBars ?  string(4*domain + 4 + extraIndent, '-') : string(3*domain + 4 + extraIndent, '-')), 2);
        cout << INDENT;
        for (int ptVal = 0; ptVal <= domain; ptVal++) {
            if (useThickBars)
                cout << (ptVal < 10 ? to_string(ptVal) + "   " : to_string(ptVal) + "  ");
            else
                cout << (ptVal < 10 ? to_string(ptVal) + "  " : to_string(ptVal) + " ");
        }
    }
    double avg = haveToCalcAllTimeCombinedCribAvg ? combinedCribAverage("all-time") : computeMeanFromHisto(histo);
    string avgStr = to_string(avg);
    avgStr = avgStr.substr(0, avgStr.size() - 4);  // to only show 2 decimal places
    printCentered((presentHorizontally ? 2 : 3), "Average:  " + avgStr + " pts", 7);
}


void printSummaryMessageAtBottomOfVsHisto(bool winsVsHisto, bool forAllTime) {
// Prints summary msg for bottom of winsVs histogram if winsVsHisto is true,
// otherwise prints that msg for the macrosVs histogram.
// Prints it for all-time if forAllTime is true, otherwise for just the current year.
    int diff = -1;
    string msg;

    if (winsVsHisto) {
        if (forAllTime) {
            int allWinsMolly = molly.winsAT + mollyLegacy.winsAT + mollyLegacy.wins_b4_cribulator;
            int allWinsJohnny = johnny.winsAT + johnnyLegacy.winsAT + johnnyLegacy.wins_b4_cribulator;
            diff = abs(allWinsMolly - allWinsJohnny);

            if (allWinsMolly > allWinsJohnny)
                msg = "Molly leads in all-time wins with " + to_string(allWinsMolly) + " vs " + to_string(allWinsJohnny) + " for Johnny";
            else if (allWinsJohnny > allWinsMolly)
                msg = "Johnny leads in all-time wins with " + to_string(allWinsJohnny) + " vs " + to_string(allWinsMolly) + " for Molly";
            else  // tied!
                msg = "Remarkably, it is currently tied: Molly and Johnny both have " + to_string(allWinsMolly) + " wins all-time!";
        }
        else {  // for this year (default)
            int yearWinsMolly = molly.winsAT;
            int yearWinsJohnny = johnny.winsAT;
            diff = abs(yearWinsMolly - yearWinsJohnny);

            if (yearWinsMolly > yearWinsJohnny)
                msg = "Molly currently leads in " + to_string(year) + " with " + to_string(yearWinsMolly) + " wins vs " + to_string(yearWinsJohnny) + " for Johnny";
            else if (yearWinsJohnny > yearWinsMolly)
                msg = "Johnny currently leads in " + to_string(year) + " with " + to_string(yearWinsJohnny) + " wins vs " + to_string(yearWinsMolly) + " for Molly";
            else  // tie
                msg = "It's currently a tie: Molly and Johnny both have " + to_string(yearWinsMolly) + " wins this year!";
        }
    }

    else {  // msg is for the macrosVs histogram
        if (forAllTime) {
            int allMacroPtsMolly  = molly.macrogamePtsAT  + mollyLegacy.macrogamePtsAT  + mollyLegacy.macroPts_b4_cribulator;
            int allMacroPtsJohnny = johnny.macrogamePtsAT + johnnyLegacy.macrogamePtsAT + johnnyLegacy.macroPts_b4_cribulator;
            diff = abs(allMacroPtsMolly - allMacroPtsJohnny);

            if (allMacroPtsMolly > allMacroPtsJohnny)
                msg = "Molly leads the all-time MACRO-GAME with " + to_string(allMacroPtsMolly) + " pts vs " + to_string(allMacroPtsJohnny) + " for Johnny";
            else if (allMacroPtsJohnny > allMacroPtsMolly)
                msg = "Johnny leads the all-time MACRO-GAME with " + to_string(allMacroPtsJohnny) + " pts vs " + to_string(allMacroPtsMolly) + " for Molly";
            else  // tied!
                msg = "Unbelievable! The all-time MACRO-GAME is currently tied! Molly and Johnny both have " + to_string(allMacroPtsMolly) + " points!";
        }
        else {  // for this year (default)
            int yearMacroPtsMolly = molly.macrogamePtsAT;
            int yearMacroPtsJohnny = johnny.macrogamePtsAT;
            diff = abs(yearMacroPtsMolly - yearMacroPtsJohnny);

            if (yearMacroPtsMolly > yearMacroPtsJohnny)
                msg = "Molly currently leads this year's MACRO-GAME with " + to_string(yearMacroPtsMolly) + " pts vs " + to_string(yearMacroPtsJohnny) + " for Johnny";
            else if (yearMacroPtsJohnny > yearMacroPtsMolly)
                msg = "Johnny currently leads this year's MACRO-GAME with " + to_string(yearMacroPtsJohnny) + " pts vs " + to_string(yearMacroPtsMolly) + " for Molly";
            else  // tie
                msg = "Wow! This year's MACRO-GAME is currently tied! Molly and Johnny both have " + to_string(yearMacroPtsMolly) + " points in " + to_string(year) + ".";
        }
    }

    printCentered(2, msg, 1);
    if (diff > 0)  printCentered(1, "(" + to_string(diff) + " " + (winsVsHisto ? "game" : "pt") + " lead)", 1);
}


string multString(int multiple, string st) {
    string multipliedSt = "";
    for (int i = 0; i < multiple; i++)
        multipliedSt += st;
    return multipliedSt;
}


double computeMeanFromHisto(vector<int> histo) {
    int numHands = 0;
    int sumPts = 0;
    for (int i = 0; i < histo.size(); i++) {
        numHands += histo[i];
        sumPts   += i * histo[i];
    }
    return (sumPts * 1.0) / (numHands * 1.0);
}


int domainOfHisto(vector<int> histo) {
// returns the upper bound of the histograms' UTILIZED domain (aka: the highest score achieved)
    for (int i = histo.size()-1; i >= 0; i--)
        if (histo[i] != 0)
            return i;

    return 0;
}


int rangeOfHisto(vector<int> histo) {
// returns the biggest value in the histogram
    int biggest = 0;
    for (int i = 0; i < histo.size(); i++) {
        if (histo[i] > biggest)
            biggest = histo[i];
    }
    return biggest;
}


vector<int> combinedHistoFrom(vector<vector<int>> histos) {
// Combines histograms of the same size. Assumes those passed in are the same size.
    vector<int> combined ((int)histos[0].size(), 0);
    for (vector<int> h : histos) {
        for (int i = 0; i < h.size(); i++) {
            combined[i] += h[i];
        }

    }
    return combined;
}


bool search(string s1, string s2) {
    // BLAH: make a function that converts strings to all lowercase or uppercase and use that to compare the two
    return (s1.find(s2) != string::npos);    // is s2 in s1?
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


Player* determineLoser() {
// given a list of both players and given the winner, return a pointer to the loser
    if (players[0]->name == winner->name)
        return players[1];
    return players[0];
}


void printCentered(int numNewLinesBefore, string message, int numNewLinesAfter) {
    cout << string(numNewLinesBefore, '\n') 
         << string((SCREEN_WIDTH - message.length())/2, ' ') << message 
         << string(numNewLinesAfter, '\n');
}
void printCenteredManual(int numNewLinesBefore, string message, int numNewLinesAfter, int screenWidth, int messageLength) {
// Had to make this because it seems C++'s string .length() function doesn't count certain unicode characters. Ok then.
    cout << string(numNewLinesBefore, '\n') 
         << string((screenWidth - messageLength)/2, ' ') << message 
         << string(numNewLinesAfter, '\n');
}


void loadSaveDataFromFiles() {
// Wrote this a few years ago. It's more convoluted than it needs to be....
// It works though, so I'm not changing it.
    cout << "\nLoading player save data . . .  ";
    fstream saveFile;
    fstream saveFileLegacy;
    saveFile.open(BLANK_SAVE_FILE_INPUT_NAME, ios::in);  // use blank test data if it's present (not common)

    if (!saveFile.is_open()){  // otherwise use the actual save data (expected)
        saveFile.open(SAVE_FILE_NAME, ios::in);
        saveFileLegacy.open(SAVE_FILE_LEGACY_NAME, ios::in);  // + the legacy data
        if (!saveFile.is_open() || !saveFileLegacy.is_open() ){
            cout << "\n\n*** Error in opening '" << SAVE_FILE_NAME << "' or '" << SAVE_FILE_LEGACY_NAME
                 << "' ***\n\n"
                 << "Exiting program...\n\n\n";
            exit(1);
        }
        for (Player* p : playersLegacy)
            loadDataForPlayerFromFile(p, &saveFileLegacy);
        saveFileLegacy.close();
    } else  usingBlankSaveData = true;

    for (Player* p : players)
        loadDataForPlayerFromFile(p, &saveFile);
    saveFile.close();
 
    cout << (usingBlankSaveData ? "BLANK TEST DATA LOADED" : "success :)") << endl;
}


void loadDataForPlayerFromFile(Player* p, fstream* saveFile){
    string line;
    while(getline(*saveFile, line)) {
        if (line == "")
            continue;
        
        if (line == VAR_PREFIX + "name") {
            getline(*saveFile, line);
            p->name = line;
        }
        else if (line == VAR_PREFIX + "indtAdjstdName"){
            getline(*saveFile, line);
            p->indtAdjstdName = line;
        }
        else if (line == VAR_PREFIX + "wins_b4_cribulator"){
            getline(*saveFile, line);
            p->wins_b4_cribulator = stoi(line);
        }
        else if (line == VAR_PREFIX + "cribSum_b4_cribulator"){
            getline(*saveFile, line);
            p->cribSum_b4_cribulator = stoi(line);
        }
        else if (line == VAR_PREFIX + "numHands_b4_cribulator"){
            getline(*saveFile, line);
            p->numHands_b4_cribulator = stoi(line);
        }
        else if (line == VAR_PREFIX + "macroPts_b4_cribulator"){
            getline(*saveFile, line);
            p->macroPts_b4_cribulator = stoi(line);
        }
            
        else if (line == VAR_PREFIX + "histoCutsForFirstCribAT") {
            for (int i = 0; i < 13; i++) {
                getline(*saveFile, line);
                p->histoCutsForFirstCribAT[i] = stoi(line);
            }
        }
        else if (line == VAR_PREFIX + "histoCutsForMyCribAT") {
            for (int i = 0; i < 13; i++) {
                getline(*saveFile, line);
                p->histoCutsForMyCribAT[i] = stoi(line);
            }
        }
        else if (line == VAR_PREFIX + "histoHandPtsAT") {
            for (int i = 0; i < 30; i++) {
                getline(*saveFile, line);
                p->histoHandPtsAT[i] = stoi(line);
            }
        }
        else if (line == VAR_PREFIX + "histoCribPtsAT") {
            for (int i = 0; i < 30; i++) {
                getline(*saveFile, line);
                p->histoCribPtsAT[i] = stoi(line);
            }
        }
        else if (line == VAR_PREFIX + "histoWinMarginsAT") {
            for (int i = 0; i < 120; i++) {
                getline(*saveFile, line);
                p->histoWinMarginsAT[i] = stoi(line);
            }
        }

        else if (line == VAR_PREFIX + "numFirstCribsAT"){
            getline(*saveFile, line);
            p->numFirstCribsAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "numFirstCribsWonAT"){
            getline(*saveFile, line);
            p->numFirstCribsWonAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "peggedPtsAt"){
            getline(*saveFile, line);
            p->peggedPtsAt = stoi(line);
        }

        else if (line == VAR_PREFIX + "unusedPtsAT") {
            for (int i = 0; i < 4; i++) {
                getline(*saveFile, line);
                p->unusedPtsAT[i] = stoi(line);
            }
        }

        else if (line == VAR_PREFIX + "macrogamePtsAT"){
            getline(*saveFile, line);
            p->macrogamePtsAT = stoi(line);
        }

        else if (line == VAR_PREFIX + "macrogamePtsByMonth"){  // new addition for 2024
            for (int i = 0; i < 12; i++) {
                getline(*saveFile, line);
                p->macrogamePtsByMonth[i] = stoi(line);
            }
        }

        else if (line == VAR_PREFIX + "numSessionsPlayedAT"){
            getline(*saveFile, line);
            p->numSessionsPlayedAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "numHandsAT"){
            getline(*saveFile, line);
            p->numHandsAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "numCribsAT"){
            getline(*saveFile, line);
            p->numCribsAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "gamesPlayedAT"){
            getline(*saveFile, line);
            p->gamesPlayedAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "gamesPlayedByMonth"){  // new addition for 2024
            for (int i = 0; i < 12; i++) {
                getline(*saveFile, line);
                p->gamesPlayedByMonth[i] = stoi(line);
            }
        }
        else if (line == VAR_PREFIX + "winsAT"){
            getline(*saveFile, line);
            p->winsAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "winsByMonth"){  // new addition for 2024
            for (int i = 0; i < 12; i++) {
                getline(*saveFile, line);
                p->winsByMonth[i] = stoi(line);
            }
        }
        else if (line == VAR_PREFIX + "nobsAT"){
            getline(*saveFile, line);
            p->nobsAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "FOKsAT"){
            getline(*saveFile, line);
            p->FOKsAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "flushesAT"){
            getline(*saveFile, line);
            p->flushesAT = stoi(line);
        }
        else if (line == VAR_PREFIX + "superFlushesAT"){
            getline(*saveFile, line);
            p->superFlushesAT = stoi(line);
        }

        else if (line == "- - - - - - - - - -")
            break; // done for this player

    } // /while there are lines in the file
}


void saveATDataToFile() {
// save the players' all-time data to file

    string saveData = "";

    for (Player* p : players) { 
        saveData += VAR_PREFIX + "name" + "\n";
        saveData += p->name + "\n\n";

        saveData += VAR_PREFIX + "indtAdjstdName" + "\n";
        saveData += p->indtAdjstdName + "\n\n";

        saveData += VAR_PREFIX + "wins_b4_cribulator" + "\n";
        saveData += to_string(p->wins_b4_cribulator) + "\n\n";

        saveData += VAR_PREFIX + "cribSum_b4_cribulator" + "\n";
        saveData += to_string(p->cribSum_b4_cribulator) + "\n\n";

        saveData += VAR_PREFIX + "numHands_b4_cribulator" + "\n";
        saveData += to_string(p->numHands_b4_cribulator) + "\n\n";

        saveData += VAR_PREFIX + "macroPts_b4_cribulator" + "\n";
        saveData += to_string(p->macroPts_b4_cribulator) + "\n\n";

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

        saveData += VAR_PREFIX + "macrogamePtsByMonth" + "\n";  // new addition for 2024
        for (int i = 0; i < 12; i++)
            saveData += to_string(p->macrogamePtsByMonth[i]) + "\n";
        saveData += "\n";

        saveData += VAR_PREFIX + "numSessionsPlayedAT" + "\n";
        saveData += to_string(p->numSessionsPlayedAT) + "\n\n";

        saveData += VAR_PREFIX + "numHandsAT" + "\n";
        saveData += to_string(p->numHandsAT) + "\n\n";

        saveData += VAR_PREFIX + "numCribsAT" + "\n";
        saveData += to_string(p->numCribsAT) + "\n\n";

        saveData += VAR_PREFIX + "gamesPlayedAT" + "\n";
        saveData += to_string(p->gamesPlayedAT) + "\n\n";

        saveData += VAR_PREFIX + "gamesPlayedByMonth" + "\n";  // new addition for 2024
        for (int i = 0; i < 12; i++)
            saveData += to_string(p->gamesPlayedByMonth[i]) + "\n";
        saveData += "\n";

        saveData += VAR_PREFIX + "winsAT" + "\n";
        saveData += to_string(p->winsAT) + "\n\n";

        saveData += VAR_PREFIX + "winsByMonth" + "\n";  // new addition for 2024
        for (int i = 0; i < 12; i++)
            saveData += to_string(p->winsByMonth[i]) + "\n";
        saveData += "\n";

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

    ofstream saveFile;
    if (usingBlankSaveData) {
        saveFile.open(BLANK_SAVE_FILE_OUTPUT_NAME, ios::out);
        cout << "Saving data to 'test_save_data_output.txt' . . .  ";
    }
    else {
        saveFile.open(SAVE_FILE_NAME, ios::out);
        cout << "Saving data to 'cribulator_player_save_data.txt' . . .  ";
    }
    saveFile << saveData;
    saveFile.close();
    cout << "success :)\n\n\n\n";
}


string getlineMine() {
// performs getline(cin, string) but does not allow null input.
// It forces the user to enter something.
    string userInput;
    do {
        getline(cin, userInput);
        if (userInput != "") {
            if (search(userInput, CORRECTION_VARIATIONS)) {
                userWantsToCorrectRound = true;
                // printCentered(0, "~  correction  ~", 1);  <-- for a more centered presentation
                printCenteredManual(2, "~  correction  ~", 1, SCREEN_WIDTH_NARROW, 16);
            }
            return userInput;
        }
        // printCentered(1, "Woops: looks like you hit ENTER by mistake.", 2);  <-- for a more centered presentation
        printCenteredManual(2, "Woops: looks like you hit ENTER by mistake.", 2, SCREEN_WIDTH_NARROW, 43);
        cout << "What did you mean to input?  ";
    } while(true);
}


void backupVariables() {
    //if (backupWasJustRestored)
        //return;  // no need to backup again: we've just restored this data and are performing a correction
    
    cribberBackup = cribber;
    nonCribberBackup = nonCribber;

    for (int i = 0; i < players.size(); i++) {
        //vector<int> histoCutsForFirstCribAT {vector<int>(13,0)};
        playerBackups[i]->histoCutsForFirstCribAT = players[i]->histoCutsForFirstCribAT;

        // vector<int> histoCutsForMyCribAT {vector<int>(13,0)};  // aka starter cards
        playerBackups[i]->histoCutsForMyCribAT = players[i]->histoCutsForMyCribAT;

        // vector<int> histoHandPtsAT {vector<int>(30,0)};
        playerBackups[i]->histoHandPtsAT = players[i]->histoHandPtsAT;

        // vector<int> histoCribPtsAT {vector<int>(30,0)};
        playerBackups[i]->histoCribPtsAT = players[i]->histoCribPtsAT;

        // vector<int> histoWinMarginsAT {vector<int>(120,0)};
        playerBackups[i]->histoWinMarginsAT = players[i]->histoWinMarginsAT;

        playerBackups[i]->numFirstCribsAT = players[i]->numFirstCribsAT;
        playerBackups[i]->numFirstCribsWonAT = players[i]->numFirstCribsWonAT;
        playerBackups[i]->peggedPtsAt = players[i]->peggedPtsAt;

        // vector<int> unusedPtsAT = {0,0,0,0};
        playerBackups[i]->unusedPtsAT = players[i]->unusedPtsAT;

        playerBackups[i]->macrogamePtsAT = players[i]->macrogamePtsAT;
        playerBackups[i]->macrogamePtsByMonth = players[i]->macrogamePtsByMonth;
        playerBackups[i]->numSessionsPlayedAT = players[i]->numSessionsPlayedAT;
        playerBackups[i]->numHandsAT = players[i]->numHandsAT;
        playerBackups[i]->numCribsAT = players[i]->numCribsAT;
        playerBackups[i]->gamesPlayedAT = players[i]->gamesPlayedAT;
        playerBackups[i]->gamesPlayedByMonth = players[i]->gamesPlayedByMonth;
        playerBackups[i]->winsAT = players[i]->winsAT;
        playerBackups[i]->winsByMonth = players[i]->winsByMonth;
        playerBackups[i]->nobsAT = players[i]->nobsAT;
        playerBackups[i]->FOKsAT = players[i]->FOKsAT;
        playerBackups[i]->flushesAT = players[i]->flushesAT;
        playerBackups[i]->superFlushesAT = players[i]->superFlushesAT;

        // vector<int> cutsForFirstCribToday;
        playerBackups[i]->cutsForFirstCribToday = players[i]->cutsForFirstCribToday;

        // vector<int> histoCutsForMyCribToday {vector<int>(13,0)};
        playerBackups[i]->histoCutsForMyCribToday = players[i]->histoCutsForMyCribToday;

        // vector<int> histoHandPtsToday {vector<int>(30,0)};
        playerBackups[i]->histoHandPtsToday = players[i]->histoHandPtsToday;
        
        // vector<int> histoCribPtsToday {vector<int>(30,0)};
        playerBackups[i]->histoCribPtsToday = players[i]->histoCribPtsToday;
        
        // vector<int> winMarginsToday;
        playerBackups[i]->winMarginsToday = players[i]->winMarginsToday;

        playerBackups[i]->numFirstCribsToday = players[i]->numFirstCribsToday;
        playerBackups[i]->numFirstCribsWonToday = players[i]->numFirstCribsWonToday;
        playerBackups[i]->peggedPtsToday = players[i]->peggedPtsToday;

        //vector<int> unusedPtsToday = {0,0,0,0};
        playerBackups[i]->unusedPtsToday = players[i]->unusedPtsToday;
        
        playerBackups[i]->losingPositionToday = players[i]->losingPositionToday;
        playerBackups[i]->macrogamePtsToday = players[i]->macrogamePtsToday;
        playerBackups[i]->numHandsToday = players[i]->numHandsToday;
        playerBackups[i]->numCribsToday = players[i]->numCribsToday;
        playerBackups[i]->gamesPlayedToday = players[i]->gamesPlayedToday;
        playerBackups[i]->winsToday = players[i]->winsToday;
        playerBackups[i]->nobsToday = players[i]->nobsToday;
        playerBackups[i]->FOKsToday = players[i]->FOKsToday;
        playerBackups[i]->flushesToday = players[i]->flushesToday;
        playerBackups[i]->superFlushesToday = players[i]->superFlushesToday;
        playerBackups[i]->handPtsThisGame = players[i]->handPtsThisGame;
        playerBackups[i]->cribPtsThisGame = players[i]->cribPtsThisGame;
        playerBackups[i]->numNibsThisGame = players[i]->numNibsThisGame;
        playerBackups[i]->peggedPtsThisGame = players[i]->peggedPtsThisGame;

        //vector<int> unusedPtsThisGame = {0,0,0,0};
        playerBackups[i]->unusedPtsThisGame = players[i]->unusedPtsThisGame;
        
        playerBackups[i]->numHandsThisGame = players[i]->numHandsThisGame;
        playerBackups[i]->numCribsThisGame = players[i]->numCribsThisGame;
    }
    userWantsToCorrect1stCribCut = false;  // if we've reached the call to this function,
                                           // the correction request doesn't have to do with 
                                           // fixing the cut to see who gets crib first
}


void restoreBackupOfVariables() {
    cribber = cribberBackup;
    nonCribber = nonCribberBackup;

    for (int i = 0; i < players.size(); i++) {
        //vector<int> histoCutsForFirstCribAT {vector<int>(13,0)};
        players[i]->histoCutsForFirstCribAT = playerBackups[i]->histoCutsForFirstCribAT;

        // vector<int> histoCutsForMyCribAT {vector<int>(13,0)};  // aka starter cards
        players[i]->histoCutsForMyCribAT = playerBackups[i]->histoCutsForMyCribAT;

        // vector<int> histoHandPtsAT {vector<int>(30,0)};
        players[i]->histoHandPtsAT = playerBackups[i]->histoHandPtsAT;

        // vector<int> histoCribPtsAT {vector<int>(30,0)};
        players[i]->histoCribPtsAT = playerBackups[i]->histoCribPtsAT;

        // vector<int> histoWinMarginsAT {vector<int>(120,0)};
        players[i]->histoWinMarginsAT = playerBackups[i]->histoWinMarginsAT;

        players[i]->numFirstCribsAT = playerBackups[i]->numFirstCribsAT;
        players[i]->numFirstCribsWonAT = playerBackups[i]->numFirstCribsWonAT;
        players[i]->peggedPtsAt = playerBackups[i]->peggedPtsAt;

        // vector<int> unusedPtsAT = {0,0,0,0};
        players[i]->unusedPtsAT = playerBackups[i]->unusedPtsAT;

        players[i]->macrogamePtsAT = playerBackups[i]->macrogamePtsAT;
        players[i]->macrogamePtsByMonth = playerBackups[i]->macrogamePtsByMonth;
        players[i]->numSessionsPlayedAT = playerBackups[i]->numSessionsPlayedAT;
        players[i]->numHandsAT = playerBackups[i]->numHandsAT;
        players[i]->numCribsAT = playerBackups[i]->numCribsAT;
        players[i]->gamesPlayedAT = playerBackups[i]->gamesPlayedAT;
        players[i]->gamesPlayedByMonth = playerBackups[i]->gamesPlayedByMonth;
        players[i]->winsAT = playerBackups[i]->winsAT;
        players[i]->winsByMonth = playerBackups[i]->winsByMonth;
        players[i]->nobsAT = playerBackups[i]->nobsAT;
        players[i]->FOKsAT = playerBackups[i]->FOKsAT;
        players[i]->flushesAT = playerBackups[i]->flushesAT;
        players[i]->superFlushesAT = playerBackups[i]->superFlushesAT;

        // vector<int> cutsForFirstCribToday;
        players[i]->cutsForFirstCribToday = playerBackups[i]->cutsForFirstCribToday;

        // vector<int> histoCutsForMyCribToday {vector<int>(13,0)};
        players[i]->histoCutsForMyCribToday = playerBackups[i]->histoCutsForMyCribToday;

        // vector<int> histoHandPtsToday {vector<int>(30,0)};
        players[i]->histoHandPtsToday = playerBackups[i]->histoHandPtsToday;
        
        // vector<int> histoCribPtsToday {vector<int>(30,0)};
        players[i]->histoCribPtsToday = playerBackups[i]->histoCribPtsToday;
        
        // vector<int> winMarginsToday;
        players[i]->winMarginsToday = playerBackups[i]->winMarginsToday;

        players[i]->numFirstCribsToday = playerBackups[i]->numFirstCribsToday;
        players[i]->numFirstCribsWonToday = playerBackups[i]->numFirstCribsWonToday;
        players[i]->peggedPtsToday = playerBackups[i]->peggedPtsToday;

        //vector<int> unusedPtsToday = {0,0,0,0};
        players[i]->unusedPtsToday = playerBackups[i]->unusedPtsToday;
        
        players[i]->losingPositionToday = playerBackups[i]->losingPositionToday;
        players[i]->macrogamePtsToday = playerBackups[i]->macrogamePtsToday;
        players[i]->numHandsToday = playerBackups[i]->numHandsToday;
        players[i]->numCribsToday = playerBackups[i]->numCribsToday;
        players[i]->gamesPlayedToday = playerBackups[i]->gamesPlayedToday;
        players[i]->winsToday = playerBackups[i]->winsToday;
        players[i]->nobsToday = playerBackups[i]->nobsToday;
        players[i]->FOKsToday = playerBackups[i]->FOKsToday;
        players[i]->flushesToday = playerBackups[i]->flushesToday;
        players[i]->superFlushesToday = playerBackups[i]->superFlushesToday;
        players[i]->handPtsThisGame = playerBackups[i]->handPtsThisGame;
        players[i]->cribPtsThisGame = playerBackups[i]->cribPtsThisGame;
        players[i]->numNibsThisGame = playerBackups[i]->numNibsThisGame;
        players[i]->peggedPtsThisGame = playerBackups[i]->peggedPtsThisGame;

        //vector<int> unusedPtsThisGame = {0,0,0,0};
        players[i]->unusedPtsThisGame = playerBackups[i]->unusedPtsThisGame;
        
        players[i]->numHandsThisGame = playerBackups[i]->numHandsThisGame;
        players[i]->numCribsThisGame = playerBackups[i]->numCribsThisGame;
    }
    cribber->histoCutsForMyCribToday[lastCutCardIndex]--;  // jank correction needed due to where the backup snapshot is taken
    cribber->histoCutsForMyCribAT[lastCutCardIndex]--;
    userWantsToCorrectRound = false;
    //backupWasJustRestored = true;
    // printCentered(0, "Re-enter that round", 1);  <-- for a more centered presentation
    printCenteredManual(0, "Re-enter that round", 3, SCREEN_WIDTH_NARROW, 19);
    cout << cribber->name << "'s crib\n";
}


void undoLastDrawForWhoGets1stCrib() {
    for (Player* p : players) {
        p->histoCutsForFirstCribAT[cardIndexOf(p->cutsForFirstCribToday.back())]--;
        p->cutsForFirstCribToday.pop_back();
    }
}


bool userWantsToCorrect1stCribDraw() {
    if (players[0]->numHandsThisGame != 0)
        return false;

    cout << "\n   What do you want to correct?\n\n"
         << "      1) previous game's last hand\n"
         << "      2) card draw for this game's first crib\n\n"
         << "   Enter choice:  ";
    string correctionChoice = getlineMine();
    cout << "\n\n";
    if (correctionChoice == "1")
        return false;

    userWantsToCorrectRound = false;
    userWantsToCorrect1stCribCut = true;
    undoLastDrawForWhoGets1stCrib();
    return true;
}


int mystoi(string st)  {
// an error-catching version of stoi(), used to catch cases where user enters letters
// ...before the number, so that the program doesn't crash. Forces the user to
// ...re-enter the input in those cases.
    int intVersion;
    try {
       intVersion = stoi(st);
    } catch(invalid_argument) {  // can also use catch(...) to catch ALL exception types
        if (st == previousInvalidInput)
            return previousInvalidInputFix;

        previousInvalidInput = st;
        // printCentered(1, "Error: invalid input '" + st + "' detected", 2);  <-- for a more centered presentation
        cout << "\n Error: invalid input '" + st + "' detected\n\n";
        cout << "What did you mean?  ";
        string input;
        getline(cin, input);
        intVersion = mystoi(input);
    }

    previousInvalidInputFix = intVersion;
    return intVersion;
}


bool handleStatsRequests(string userInput) {
// Precondition:  players is {*molly, *johnny}, in that order
// Postcondition:  Displays any stats requested. 
//                 Returns true if they were requested, false otherwise.
    if (search(userInput, HISTO_VARIATIONS)) {
        printHistogram(userInput);
        userWantsToCorrectRound = false;  // ignore any correction requests in input that has a histo request
        return true;
    }
    return false;
}