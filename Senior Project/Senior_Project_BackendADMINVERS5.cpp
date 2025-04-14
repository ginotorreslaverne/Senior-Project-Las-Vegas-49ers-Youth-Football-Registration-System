// -----------------------------------------------------------------------------------------------------------------------
//                                               Name Gino Torres 
//                                               Course CMPS 499, Spring 2025
//                                               Senior Project
//                                               
// Purpose: 
/* 
Admin Backend Development for Senior Project: 
Admins must log in with credentials before accessing functionality.
Can view, access, edit player information, including division, and remove players.
*/
// -----------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <sqlite3.h>
#include <string>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

// Function prototypes
void viewPlayers(sqlite3 *db);
void editPlayer(sqlite3 *db);
void removePlayer(sqlite3 *db);
bool login();
string getHiddenInput();

// Function to calculate age as of July 31st
int calculateAgeOnJuly31(const string& dob) {
    int birthYear, birthMonth, birthDay;
    char dash;

    istringstream iss(dob);
    iss >> birthYear >> dash >> birthMonth >> dash >> birthDay;

    // Get current year
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int currentYear = 1900 + ltm->tm_year;

    // Calculate age as of July 31
    int age = currentYear - birthYear;
    if (birthMonth > 7 || (birthMonth == 7 && birthDay > 31)) {
        age--; // hasn't had birthday yet this year by July 31
    }

    return age;
}

// Function to determine division based on age
string determineDivision(int age) {
    if (age <= 7) return "7U";
    if (age == 8) return "8U";
    if (age == 9) return "9U";
    if (age == 10) return "10U";
    if (age == 11) return "11U";
    if (age == 12) return "12U";
    return "13U";
}

// Main function
int main() {
    // Login authentication
    if (!login()) {
        cout << "Too many failed login attempts. Exiting...\n";
        return 0;
    }

    sqlite3 *db;
    int exit = sqlite3_open("registration.db", &db); // Connect to the database

    if (exit) {
        cerr << "Error opening database: " << sqlite3_errmsg(db) << endl;
        return -1;
    } else {
        cout << "Database opened successfully.\n";
    }

    int choice;
    do {
        cout << "\nAdmin Menu:\n";
        cout << "1. View Players\n";
        cout << "2. Edit Player Information\n";
        cout << "3. Remove Player\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear input buffer

        switch (choice) {
            case 1:
                viewPlayers(db);
                break;
            case 2:
                editPlayer(db);
                break;
            case 3:
                removePlayer(db);
                break;
            case 4:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 4);

    sqlite3_close(db);
    return 0;
}

// Function to get password input without echo
string getHiddenInput() {
    string input;
    char ch;

#ifdef _WIN32
    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b') { // Backspace
            if (!input.empty()) {
                input.pop_back();
                cout << "\b \b";
            }
        } else {
            input.push_back(ch);
            cout << '*';
        }
    }
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    getline(cin, input);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    cout << endl;
    return input;
}

// Secure admin login function
bool login() {
    const string correctUsername = "gtorres88";
    const string correctPassword = "GFamily88!";

    string username, password;
    int attempts = 0;
    const int maxAttempts = 3;
    int lockoutTime = 30; // seconds

    while (true) {
        cout << "\n=== Admin Login ===\n";
        cout << "Username: ";
        getline(cin, username);
        cout << "Password: ";
        password = getHiddenInput();

        if (username == correctUsername && password == correctPassword) {
            cout << "Login successful. Welcome, Admin!\n";
            return true;
        } else {
            cout << "Incorrect username or password.\n";
            attempts++;

            if (attempts == maxAttempts) {
                cout << "Too many failed attempts. Locked out for " << lockoutTime << " seconds.\n";
                this_thread::sleep_for(chrono::seconds(lockoutTime));
                attempts++;  // Go past maxAttempts to start exponential lockouts
            } else if (attempts > maxAttempts) {
                cout << "Still incorrect. Locked out for " << lockoutTime << " seconds.\n";
                this_thread::sleep_for(chrono::seconds(lockoutTime));
                lockoutTime *= 2;
            }
        }
    }

    return false;
}

// Function to view all players
void viewPlayers(sqlite3 *db) {
    string query = "SELECT ROWID, Name, DOB, Guardian, Phone, Email, Division FROM Players;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Error executing query: " << sqlite3_errmsg(db) << endl;
        return;
    }

    cout << "\nPlayers List:\n";
    cout << "ID | Name | DOB | Guardian | Phone | Email | Division\n";
    cout << "---------------------------------------------------------\n";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        const char *dob = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        const char *guardian = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        const char *phone = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
        const char *email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
        const char *division = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

        cout << id << " | " << name << " | " << dob << " | " << guardian
             << " | " << phone << " | " << email << " | " << division << "\n";
    }

    sqlite3_finalize(stmt);
}

// Function to edit a player's information
void editPlayer(sqlite3 *db) {
    int id;
    string column, newValue;
    unordered_set<string> validFields = {"Name", "DOB", "Guardian", "Phone", "Email", "Division"};

    cout << "Enter Player ID to edit: ";
    cin >> id;
    cin.ignore();

    cout << "Enter the field to edit (Name, DOB, Guardian, Phone, Email, Division): ";
    getline(cin, column);

    if (validFields.find(column) == validFields.end()) {
        cout << "Invalid field. Valid fields are: Name, DOB, Guardian, Phone, Email, Division\n";
        return;
    }

    cout << "Enter the new value: ";
    getline(cin, newValue);

    string query;
    sqlite3_stmt *stmt;

    if (column == "DOB") {
        // Update DOB
        query = "UPDATE Players SET DOB = ? WHERE ROWID = ?;";
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, id);
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "DOB updated.\n";
            } else {
                cerr << "Error updating DOB: " << sqlite3_errmsg(db) << endl;
                sqlite3_finalize(stmt);
                return;
            }
            sqlite3_finalize(stmt);
        }

        // Auto-update Division based on new DOB
        int age = calculateAgeOnJuly31(newValue);
        string division = determineDivision(age);

        query = "UPDATE Players SET Division = ? WHERE ROWID = ?;";
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, division.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, id);
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << "Division auto-updated based on DOB: " << division << "\n";
            } else {
                cerr << "Error updating Division: " << sqlite3_errmsg(db) << endl;
            }
            sqlite3_finalize(stmt);
        }

    } else {
        // Regular manual update
        query = "UPDATE Players SET " + column + " = ? WHERE ROWID = ?;";
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 2, id);
            if (sqlite3_step(stmt) == SQLITE_DONE) {
                cout << column << " updated successfully.\n";
            } else {
                cerr << "Error updating " << column << ": " << sqlite3_errmsg(db) << endl;
            }
        } else {
            cerr << "Error preparing query: " << sqlite3_errmsg(db) << endl;
        }
        sqlite3_finalize(stmt);
    }
}

// Function to remove a player
void removePlayer(sqlite3 *db) {
    int id;

    cout << "Enter Player ID to remove: ";
    cin >> id;
    cin.ignore();

    string query = "DELETE FROM Players WHERE ROWID = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Player removed successfully.\n";
        } else {
            cerr << "Error removing player: " << sqlite3_errmsg(db) << std::endl;
        }
    } else {
        cerr << "Error preparing query: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
}
