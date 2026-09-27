// Author: Kareem Dahrouj
// Project: HOLMES & THE SHADOW DETECTIVE
// Description: In this game you are a detective who has a case to solve in 24 hours, you go to the crime scene
//              and interrogate suspects. If you don't solve the case you're off the team.
// Created: 1/10/2025
// Modified: 10/2026 - fixed input bugs, made time limit end the game, made it run on Windows/Mac/Linux,
//                      replaced copy-pasted input validation with shared helper functions

#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

struct Suspect {
    string name;           // Name of the suspect
    string motive;         // Motive of the suspect
    bool interrogated;     // Whether the suspect has been interrogated or not
    string questionAnswer; // Last answer the suspect gave
};

struct Location {
    string name;
    string description;
    bool visited;
};

const int STATION = 4; // Location index used to mean "back at the station"

// ---------- Function declarations (now match the definitions below) ----------
void clearScreen();
void pauseSeconds(int seconds);
void pressEnter();
int getValidInput(int min, int max, const string& errorMessage);
bool askYesNo(const string& prompt);
void spendTime(int& timeRemaining, int hours);
void displayQuestions();
void guessSuspect(Suspect suspects[], int numSuspects);
void askQuestion(int question, Suspect& selectedSuspect, int& timeRemaining, int& confidence);
void interrogateSuspects(Suspect suspects[], int numSuspects, int& timeRemaining, int& confidence, int& numOfClues);
void navigateLocations(Location locations[], int& currentLocation, int& timeRemaining, int& numOfClues, int& confidence);
int displayMenu(const string& question, const string& option1, const string& option2, const string& option3, const string& errorMessage);

// ---------- Helper functions ----------

// Clears the terminal on any operating system.
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Portable replacement for sleep() from <unistd.h>, which doesn't exist on Windows.
void pauseSeconds(int seconds) {
    this_thread::sleep_for(chrono::seconds(seconds));
}

void pressEnter() {
    cout << "\nPress Enter to continue...";
    string line;
    getline(cin, line);
}

// Reads a full line and keeps asking until it is a single whole number between min and max.
// Replaces the ~8 copy-pasted cin.fail() blocks, and fixes the menus that froze on letter input.
int getValidInput(int min, int max, const string& errorMessage) {
    string line;
    while (true) {
        if (!getline(cin, line)) {
            exit(0); // Input stream closed (e.g. Ctrl+D), so quit cleanly
        }
        stringstream ss(line);
        int value;
        char extra;
        if (ss >> value && !(ss >> extra) && value >= min && value <= max) {
            return value;
        }
        cout << errorMessage << " ";
    }
}

bool askYesNo(const string& prompt) {
    string line;
    while (true) {
        cout << prompt;
        if (!getline(cin, line)) {
            exit(0);
        }
        if (line == "y" || line == "Y") return true;
        if (line == "n" || line == "N") return false;
        cout << "Please enter a valid option y or n\n";
    }
}

// Subtracts time without ever letting it go below zero.
void spendTime(int& timeRemaining, int hours) {
    timeRemaining -= hours;
    if (timeRemaining < 0) {
        timeRemaining = 0;
    }
}

// ---------- Game functions ----------

// Function to display the list of available questions to ask the suspect.
void displayQuestions() {
    cout << "Choose a question to ask the suspect:\n";
    cout << "1. Where were you during the night of murder?\n";
    cout << "2. Why do you look so worried?\n";
    cout << "3. Can you tell me where you were on the night of the incident?\n";
    cout << "4. How well did you know the victim?\n";
    cout << "5. What kind of relationship did you have with the victim?\n";
}

// Function to handle the process of guessing the suspect in the courtroom.
void guessSuspect(Suspect suspects[], int numSuspects) {
    cout << "Your time of investigating is up. Get ready to go to court!\n";
    pauseSeconds(3);

    clearScreen();
    cout << "====================================\n";
    cout << "          COURTROOM SCENE           \n";
    cout << "====================================\n";
    cout << "The courtroom is silent. The judge looks at you intently.\n";
    cout << "Judge: Detective, your time is up. We need an answer now.\n";
    cout << "Judge: Who do you believe is the culprit? The court demands your final decision.\n";
    pressEnter();

    // Display suspects list and prompt user to select a suspect.
    cout << "\nHere is the list of suspects:\n";
    for (int i = 0; i < numSuspects; i++) {
        cout << "[" << i + 1 << "] " << suspects[i].name << " (" << suspects[i].motive << ")\n";
    }

    cout << "Judge: Choose wisely, detective. The fate of a human life depends on your answer.\n";
    cout << "Enter your suspect's number: ";
    int choice = getValidInput(1, numSuspects,
        "Judge: It must be someone in the room, detective. Focus!\nEnter a valid suspect number:");

    cout << "====================================\n";
    cout << "Judge: You have accused " << suspects[choice - 1].name << ".\n";
    cout << "Judge: The court is now adjourned.\n";
    cout << "====================================\n";
    pressEnter();

    clearScreen();
    cout << "After the court hearing, you return to the station.\n";
    cout << "====================================\n";
    cout << "Chief: Well, detective, you wrapped up a tough case.\n";
    cout << "Chief: The courtroom was intense, but you handled yourself well.\n";
    cout << "Detective: Thank you, Chief. It was a challenging investigation, but I gave it my all.\n";
    cout << "Chief: Now get some rest, detective. You've earned it.\n";
    pressEnter();
    clearScreen();

    if (suspects[choice - 1].name == "Wife") {
        cout << "====================================\n";
        cout << "Congratulations! You solved the case and put the real suspect behind bars!\n";
        cout << "====================================\n";
    } else {
        cout << "====================================\n";
        cout << "You lose. The real culprit was the Wife, but you failed to identify her.\n";
        cout << "Better luck next time, detective.\n";
        cout << "====================================\n";
    }
    cout << "\nThis is the end for now...\n";
}

// Function to answer the player's question and charge time for it.
void askQuestion(int question, Suspect& selectedSuspect, int& timeRemaining, int& confidence) {
    if (timeRemaining <= 0) {
        cout << "You have no time left to ask questions!\n";
        return;
    }

    if (selectedSuspect.name == "Gardener") {
        if (question == 1) {
            selectedSuspect.questionAnswer = "\nI was pruning the bushes in the garden, but I heard strange noises from the house around midnight.\n";
        } else if (question == 2) {
            selectedSuspect.questionAnswer = "\nI'm not nervous, but I did notice the back door was left open when I was walking by the house.\n";
        } else if (question == 3) {
            selectedSuspect.questionAnswer = "\nI was in the garden all night. I didn't see anyone leave or enter, except the dog running outside.\n";
        } else if (question == 4) {
            selectedSuspect.questionAnswer = "\nI worked with the victim for years, maintaining the garden. They were kind, but often seemed distracted by something.\n";
        } else if (question == 5) {
            selectedSuspect.questionAnswer = "\nI was employed by the victim, but I heard them arguing with someone the night before the incident. They seemed worried.\n";
        }
    } else if (selectedSuspect.name == "Wife") {
        if (question == 1) {
            selectedSuspect.questionAnswer = "\nI was at a dinner party. But I received a strange phone call earlier in the evening, and I had to leave the room for a while.\n";
        } else if (question == 2) {
            selectedSuspect.questionAnswer = "\nI'm not nervous, I just... I just wish things could have been different but I can't explain why.\n";
        } else if (question == 3) {
            selectedSuspect.questionAnswer = "\nI was at the spa, but I didn't know the victim had a visitor that night. I should have paid more attention to the messages.\n";
        } else if (question == 4) {
            selectedSuspect.questionAnswer = "\nI met the victim years ago, and we had a brief affair. We stopped seeing each other, but we still kept in touch occasionally.\n";
        } else if (question == 5) {
            selectedSuspect.questionAnswer = "\nWe were married, but it wasn't a perfect marriage. There were times when the victim would go off and disappear for days, but I didn't question them.\n";
        }
    } else if (selectedSuspect.name == "Brother") {
        if (question == 1) {
            selectedSuspect.questionAnswer = "\nI was out of town visiting family. I didn't hear anything unusual about that night, but I did get a strange text from the victim's phone.\n";
        } else if (question == 2) {
            selectedSuspect.questionAnswer = "\nI'm fine. But there was something off about the way the victim was acting the last time I spoke to them.\n";
        } else if (question == 3) {
            selectedSuspect.questionAnswer = "\nI was attending a conference, but I did overhear some people at the conference talking about the victim's financial troubles. Maybe that's why they seemed so stressed.\n";
        } else if (question == 4) {
            selectedSuspect.questionAnswer = "\nI know the victim from years ago, but we've had some financial disagreements. They were supposed to pay me back some money, but I'm not sure if they ever did.\n";
        } else if (question == 5) {
            selectedSuspect.questionAnswer = "\nWe were close at one point, but we drifted apart after a business deal went sour. The victim became increasingly paranoid, like they were hiding something from me.\n";
        }
    }

    cout << "\nAnswer: " << selectedSuspect.questionAnswer << endl;

    // 25 confidence lowers the cost of a question from 2 hours to 1.
    if (confidence >= 25) {
        spendTime(timeRemaining, 1);
        confidence -= 25;
    } else {
        spendTime(timeRemaining, 2);
    }
}

// Function to handle interrogating the suspects.
void interrogateSuspects(Suspect suspects[], int numSuspects, int& timeRemaining, int& confidence, int& numOfClues) {
    clearScreen();
    cout << "====================================\n";
    cout << "    LOCATION: INTERROGATION ROOM         \n";
    cout << "         Time Remaining: " << timeRemaining << endl;
    cout << "         Your confidence: " << confidence << endl;
    cout << "         Amount of Clues: " << numOfClues << endl;
    cout << "====================================\n";
    cout << "Select a suspect to interrogate:\n";
    for (int i = 0; i < numSuspects; i++) {
        cout << "[" << i + 1 << "] Interrogate " << suspects[i].name << endl;
    }
    // "Go back" is always the option after the last suspect, instead of a hardcoded 4.
    int backOption = numSuspects + 1;
    cout << "[" << backOption << "] Go back to the station\n";
    cout << "(Note: 25 Confidence will lower the cost of time to 1 hour instead of 2.)\n";
    cout << "Please enter your choice: ";
    int choice = getValidInput(1, backOption, "Please enter a valid option:");

    if (choice == backOption) {
        cout << "Returning to the station...\n";
        pauseSeconds(1);
        clearScreen();
        return;
    }

    Suspect& selectedSuspect = suspects[choice - 1];

    if (selectedSuspect.interrogated) {
        cout << selectedSuspect.name << " has already been interrogated.\n";
        pauseSeconds(2);
        clearScreen();
        return;
    }

    clearScreen();
    cout << "Interrogating " << selectedSuspect.name << "...\n\n";

    bool askAnother = false;
    do {
        displayQuestions();
        cout << "\nPlease select your question: ";
        int question = getValidInput(1, 5, "Invalid question number. Please choose a number between 1 and 5:");

        askQuestion(question, selectedSuspect, timeRemaining, confidence);

        // Stop the interrogation as soon as time runs out.
        if (timeRemaining <= 0) {
            cout << "You're out of time! The court is waiting.\n";
            pauseSeconds(2);
            break;
        }

        askAnother = askYesNo("Do you want to ask another question? (y/n): ");
        clearScreen();
    } while (askAnother);

    selectedSuspect.interrogated = true;
    clearScreen();
}

// Function to travel between locations of the crime scene.
void navigateLocations(Location locations[], int& currentLocation, int& timeRemaining, int& numOfClues, int& confidence) {
    currentLocation = 0;

    do {
        // Display the current location's name and description
        clearScreen();
        cout << "====================================\n";
        cout << "     LOCATION: " << locations[currentLocation].name << endl;
        cout << "     Time Remaining: " << timeRemaining << endl;
        cout << "     Confidence: " << confidence << endl;
        cout << "     Amount of Clues: " << numOfClues << endl;
        cout << locations[currentLocation].description << endl;
        cout << "====================================\n";

        // Show the choices for this location and remember how many there are.
        int maxOption = 2;
        if (locations[currentLocation].name == "Crime Scene") {
            cout << "[1] Go Inside Home\n";
            cout << "[2] Go to Station\n";
        } else if (locations[currentLocation].name == "Home") {
            cout << "[1] Go to Kitchen\n";
            cout << "[2] Go to Living Room\n";
            cout << "[3] Go Back Outside\n";
            cout << "[4] Go to Station\n";
            maxOption = 4;
        } else {
            // Kitchen or Living Room
            cout << "[1] Go Back to Entrance\n";
            cout << "[2] Go to Station\n";
        }

        cout << "Choose your action: ";
        int userChoice = getValidInput(1, maxOption, "Invalid option for the current location. Please try again:");

        // Handle user input and update the current location based on the action chosen
        switch (userChoice) {
            case 1:
                if (locations[currentLocation].name == "Crime Scene") {
                    currentLocation = 1; // Go Inside Home
                } else if (locations[currentLocation].name == "Home") {
                    cout << "\n\nYou find the door to the kitchen locked. In order to unlock it you must answer this question.\n";
                    cout << "You find a muddy shoe print at the scene. What do you do?\n";
                    cout << "1) Measure the print for size and depth\n";
                    cout << "2) Check if the mud matches the area outside\n";
                    cout << "3) Ignore it and look for other clues\n";
                    cout << "4) Take a photo and leave the scene\n";
                    cout << "Your answer: ";
                    // Previously: while (answer < 1 && answer > 4), which is never true, so it never re-prompted.
                    int answer = getValidInput(1, 4, "Please enter a number from 1 to 4:");

                    spendTime(timeRemaining, 2);
                    if (answer == 2) {
                        currentLocation = 2; // Go to Kitchen
                        numOfClues += 1;
                        confidence += 25;
                        cout << "\nThat's the correct answer, you enter the Kitchen.\n\n";
                    } else {
                        confidence -= 10;
                        cout << "\nThat's the wrong answer, you did not enter the Kitchen.\n\n";
                    }
                    pauseSeconds(2);
                } else {
                    currentLocation = 1; // Go Back to Entrance
                }
                break;
            case 2:
                if (locations[currentLocation].name == "Home") {
                    cout << "\n\nThere is a fallen bookshelf in the way. In order to pass you must answer the question.\n";
                    cout << "What does the right to freedom of speech generally protect?\n";
                    cout << "1) The ability to say anything without consequences\n";
                    cout << "2) Spreading false information\n";
                    cout << "3) Defaming others without penalty\n";
                    cout << "4) Expressing opinions without government censorship\n";
                    cout << "Your answer: ";
                    int answer = getValidInput(1, 4, "Please enter a number from 1 to 4:");

                    spendTime(timeRemaining, 2);
                    if (answer == 4) {
                        currentLocation = 3; // Go to Living Room
                        numOfClues += 1;
                        confidence += 25;
                        cout << "\nThat's the correct answer, you enter the Living Room.\n\n";
                    } else {
                        confidence -= 10;
                        cout << "\nThat's the wrong answer, you did not enter the Living Room.\n\n";
                    }
                    pauseSeconds(2);
                } else {
                    currentLocation = STATION; // Crime Scene, Kitchen, or Living Room -> Station
                }
                break;
            case 3:
                currentLocation = 0; // Only offered at Home: go back outside
                break;
            case 4:
                currentLocation = STATION; // Only offered at Home: go to station
                break;
        }

    } while (timeRemaining > 0 && currentLocation != STATION);

    if (timeRemaining <= 0) {
        cout << "You've run out of time!\n";
    } else {
        cout << "Returning to the station...\n";
    }
    pauseSeconds(1);
    clearScreen();
}

// Function to display the main menu.
int displayMenu(const string& question, const string& option1, const string& option2, const string& option3, const string& errorMessage) {
    cout << "====================================\n";
    cout << question << endl;
    cout << "====================================\n";
    cout << "[1] " << option1 << endl;
    cout << "[2] " << option2 << endl;
    cout << "[3] " << option3 << endl;
    cout << "====================================\n";
    cout << "Choose an option: ";
    return getValidInput(1, 3, errorMessage);
}

int main() {
    string userName;
    int userChoice;          // Stores user's main menu choice.
    int timeRemaining = 24;  // Tracks the remaining time (set to 24 initially).
    int currentLocation = 0; // Stores player's current location
    int confidence = 50;
    int numOfClues = 0;
    int numSuspects = 3;

    Suspect suspects[3];

    suspects[0] = {"Gardener", "Had a secret history with Mark and worked in the garden.", false, ""};
    suspects[1] = {"Wife", "Was having an affair with Mark", false, ""};
    suspects[2] = {"Brother", "Had a financial conflict with Mark", false, ""};

    Location locations[4];

    locations[0] = {"Crime Scene", "You are at the crime scene. It's a cold, eerie place. You can either enter the home or head back to the station.", false};
    locations[1] = {"Home", "You are in the victim's home. There are multiple rooms to explore. You find the Brother's business card with a note that says 'Let's meet tomorrow.' which was the day of the murder.", false};
    locations[2] = {"Kitchen", "This is the kitchen. It's well-kept, but there are signs of a struggle. You find a small piece of jewelry under the cabinet with the engraving 'M'. Could it stand for Mark, the victim's name? Or someone else's?", false};
    locations[3] = {"Living Room", "The living room is cozy, but something feels off here. You notice a broken acrylic nail, but it had Mark's initial on it. Could these lead to new clues?", false};

    do {
        userChoice = displayMenu("HOLMES & THE SHADOW DETECTIVE", "Start Game", "Learn more", "Quit", "That isn't a valid option. Choose 1, 2, or 3:");

        if (userChoice == 1) {
            clearScreen();
            cout << "====================================\n";
            cout << "     TRAVELING TO THE LOCATION      \n";
            cout << "====================================\n";
            cout << "Make sure to take note of all the clues you gather!\n";
            cout << "You have " << timeRemaining << " Hours remaining\n\n";

            cout << "Please enter your name detective: ";
            getline(cin, userName);
            clearScreen();

            cout << "====================================\n";
            cout << "You sit at your cluttered desk, sipping cold coffee, when your boss storms in.\n\n";
            cout << "Chief: " << userName << "! We've got a high-profile case on our hands, and all eyes are on us.\n\n";
            cout << "Chief: You've got 24 hours to solve this case, or you're off the team!\n\n";
            cout << "Chief: The governor's breathing down my neck, so no slip ups " << userName << "!\n\n";
            cout << "You grab your jacket, a pen, and a notebook. The clock is ticking...\n\n";
            cout << "Chief: " << userName << " don't take this case too lightly. I feel like this case could be bigger than it seems.\n";
            cout << "====================================\n";
            cout << "Press Enter to begin your investigation...";
            string enterKey;
            getline(cin, enterKey); // wait for Enter
            clearScreen();

            while (true) {
                // Running out of time now always sends you to court, no matter where it happened.
                if (timeRemaining <= 0) {
                    clearScreen();
                    guessSuspect(suspects, numSuspects);
                    return 0;
                }

                cout << "====================================\n";
                cout << "     LOCATION: THE STATION          \n";
                cout << "     Time Remaining: " << timeRemaining << endl;
                cout << "     Confidence: " << confidence << endl;
                cout << "     Amount of Clues: " << numOfClues << endl;
                cout << "====================================\n";
                cout << "[1] View Case Details\n";
                cout << "[2] Go To The Crime Scene\n";
                cout << "[3] Interrogate Suspects\n";
                cout << "[4] Go To Court Early\n";
                cout << "[5] Quit\n";
                cout << "Choose an option: ";
                int menuChoice = getValidInput(1, 5, "Invalid choice. Please enter a number from 1 to 5:");

                switch (menuChoice) {
                    case 1:
                        clearScreen();
                        cout << "====================================\n";
                        cout << "Case Name: The Home Intrusion Murder\n";
                        cout << "Victim: Mark Thompson\n";
                        cout << "Age: 38\n";
                        cout << "Occupation: Software Developer\n";
                        cout << "Date of Death: January 15, 2025\n";
                        cout << "Time of Death (Estimated): Between 10:30 PM and 12:00 AM\n";
                        cout << "Location: Victim's home in a quiet suburban neighborhood, located at 24 Willow Creek Drive.\n";
                        cout << "====================================\n";
                        pressEnter();
                        clearScreen();
                        break;
                    case 2:
                        clearScreen();
                        navigateLocations(locations, currentLocation, timeRemaining, numOfClues, confidence);
                        break;
                    case 3:
                        interrogateSuspects(suspects, numSuspects, timeRemaining, confidence, numOfClues);
                        break;
                    case 4:
                        clearScreen();
                        guessSuspect(suspects, numSuspects);
                        return 0;
                    case 5:
                        clearScreen();
                        cout << "This is the end for now...\n";
                        return 0;
                }
            }
        } else if (userChoice == 2) {
            clearScreen();
            cout << "====================================" << endl;
            cout << "          GAME INSTRUCTIONS         " << endl;
            cout << "====================================" << endl;
            cout << "1. You have 24 hours to solve the case." << endl;
            cout << "2. Investigate locations, interrogate suspects, and gather clues." << endl;
            cout << "3. Every action you take costs time, so choose wisely." << endl;
            cout << "4. Use the evidence you gather to solve the case!" << endl;
            cout << "====================================" << endl;
            pressEnter();
            clearScreen();
        }
    } while (userChoice != 3);

    clearScreen();
    cout << "This is the end for now...\n";
    return 0;
}
