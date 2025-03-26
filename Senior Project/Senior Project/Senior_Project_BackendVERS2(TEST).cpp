// -----------------------------------------------------------------------------------------------------------------------
//                                               Name Gino Torres 
//                                               Course CMPS 499, Spring 2025
//                                               Senior Project
//                                               
// Purpose: 
/* 
Backend Devleopment for Senior Project: 
User Input Handling: Collect and validate player details.
Age Calculation: Determine player's age as of July 31st.
Division Assignment: Assign player to the correct division (7U–13U).
Basic Data Storage: Store validated player data in a structured format (can be a simple file or database later).
Save player data to a file (players.txt) – This ensures data persistence even if the program is restarted.
Store player data in an SQLite database (players.db) – This allows better data management and retrieval.
Load existing players from the database on startup – So previously registered players are not lost.
*/
//------------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <regex>
#include <sqlite3.h>

using namespace std;

// Structure to store player data
struct Player {
    string name;
    string dob;
    string guardian;
    string phone;
    string email;
    string division;
};

// Function to validate phone number format
bool isValidPhone(const string& phone) {
    regex phoneRegex("\\d{10}"); // Basic validation for 10-digit numbers
    return regex_match(phone, phoneRegex);
}

// Function to validate email format
bool isValidEmail(const string& email) {
    regex emailRegex("^[\\w.-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
    return regex_match(email, emailRegex);
}

// Function to calculate age as of July 31st of the current year
int calculateAge(const string& dob) {
    int birthYear, birthMonth, birthDay;
    sscanf(dob.c_str(), "%d-%d-%d", &birthYear, &birthMonth, &birthDay);
    
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    int currentYear = now->tm_year + 1900;
    int age = currentYear - birthYear;
    
    // Adjust age if birthdate is after July 31st
    if (birthMonth > 7 || (birthMonth == 7 && birthDay > 31)) {
        age--;
    }
    return age;
}

// Function to determine the player's division
string assignDivision(int age) {
    if (age <= 7) return "7U";
    else if (age <= 8) return "8U";
    else if (age <= 9) return "9U";
    else if (age <= 10) return "10U";
    else if (age <= 11) return "11U";
    else if (age <= 12) return "12U";
    else return "13U";
}

// Function to initialize SQLite database
void initializeDatabase(sqlite3* &db) {
    sqlite3_open("players.db", &db);
    string createTable = "CREATE TABLE IF NOT EXISTS Players (Name TEXT, DOB TEXT, Guardian TEXT, Phone TEXT, Email TEXT, Division TEXT);";
    sqlite3_exec(db, createTable.c_str(), nullptr, nullptr, nullptr);
}

// Function to insert player data into SQLite database
void insertPlayerIntoDB(sqlite3* db, const Player& p) {
    string query = "INSERT INTO Players (Name, DOB, Guardian, Phone, Email, Division) VALUES ('" + p.name + "', '" + p.dob + "', '" + p.guardian + "', '" + p.phone + "', '" + p.email + "', '" + p.division + "');";
    sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
}

// Function to load players from SQLite database
void loadPlayersFromDB(sqlite3* db, vector<Player>& players) {
    sqlite3_stmt* stmt;
    string query = "SELECT * FROM Players;";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Player p;
            p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            p.dob = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            p.guardian = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            p.phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            p.email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            p.division = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            players.push_back(p);
        }
    }
    sqlite3_finalize(stmt);
}

// Function to register a player
void registerPlayer(vector<Player>& players, sqlite3* db) {
    Player p;
    
    cout << "Enter Player Name: ";
    getline(cin, p.name);
    
    cout << "Enter Date of Birth (YYYY-MM-DD): ";
    getline(cin, p.dob);
    
    cout << "Enter Guardian Name: ";
    getline(cin, p.guardian);
    
    do {
        cout << "Enter Phone Number (10 digits): ";
        getline(cin, p.phone);
    } while (!isValidPhone(p.phone));
    
    do {
        cout << "Enter Email: ";
        getline(cin, p.email);
    } while (!isValidEmail(p.email));
    
    int age = calculateAge(p.dob);
    p.division = assignDivision(age);
    
    players.push_back(p);
    insertPlayerIntoDB(db, p);
    
    cout << "\nPlayer registered successfully! Assigned to Division: " << p.division << "\n";
}

// Function to display all registered players
void displayPlayers(const vector<Player>& players) {
    if (players.empty()) {
        cout << "No players registered yet.\n";
        return;
    }
    
    cout << "\nRegistered Players:\n";
    for (const auto& p : players) {
        cout << "Name: " << p.name << " | DOB: " << p.dob << " | Guardian: " << p.guardian
             << " | Phone: " << p.phone << " | Email: " << p.email << " | Division: " << p.division << "\n";
    }
}

int main() {
    sqlite3* db;
    initializeDatabase(db);
    
    vector<Player> players;
    loadPlayersFromDB(db, players);
    
    int choice;
    
    while (true) {
        cout << "\nYouth Football Registration System";
        cout << "\n1. Register a Player";
        cout << "\n2. View Registered Players";
        cout << "\n3. Exit";
        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1:
                registerPlayer(players, db);
                break;
            case 2:
                displayPlayers(players);
                break;
            case 3:
                cout << "Exiting system. Goodbye!\n";
                sqlite3_close(db);
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}