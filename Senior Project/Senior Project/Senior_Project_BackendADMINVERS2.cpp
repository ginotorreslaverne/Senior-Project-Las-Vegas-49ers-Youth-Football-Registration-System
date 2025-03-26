// -----------------------------------------------------------------------------------------------------------------------
//                                               Name Gino Torres 
//                                               Course CMPS 499, Spring 2025
//                                               Senior Project
//                                               
// Purpose: 
/* 
Admin Backend Development for Senior Project: 
Admins can view, access, and edit the database with features to see registered players, edit player information, and remove players.
*/
// -----------------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <sqlite3.h>
#include <string>

using namespace std;

// Function prototypes
void viewPlayers(sqlite3 *db);
void editPlayer(sqlite3 *db);
void removePlayer(sqlite3 *db);

int main() {
    sqlite3 *db;
    int exit = sqlite3_open("registration.db", &db); // Connect to the new database "registration.db"

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

// Function to view all players in the database
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

// Function to edit a player's information in the database
void editPlayer(sqlite3 *db) {
    int id;
    string column, newValue;

    cout << "Enter Player ID to edit: ";
    cin >> id;
    cin.ignore();

    cout << "Enter the field to edit (Name, DOB, Guardian, Phone, Email, Division): ";
    getline(cin, column);

    cout << "Enter the new value: ";
    getline(cin, newValue);

    string query = "UPDATE Players SET " + column + " = ? WHERE ROWID = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, newValue.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            cout << "Player information updated successfully.\n";
        } else {
            cerr << "Error updating player information: " << sqlite3_errmsg(db) << endl;
        }
    } else {
        cerr << "Error preparing query: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

// Function to remove a player from the database
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