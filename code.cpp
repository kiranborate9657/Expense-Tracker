#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
using namespace std;

string loggedInUser;

// Encrypt password
string encryptPassword(string password) {
    for (char &c : password) c += 3;
    return password;
}

// Decrypt password
string decryptPassword(string password) {
    for (char &c : password) c -= 3;
    return password;
}

// Register user
void registerUser() {
    string username, password;
    cout << "Enter a new username: ";
    cin >> username;
    cout << "Enter a new password: ";
    cin >> password;

    password = encryptPassword(password);
    ofstream file("users.txt", ios::app);
    if (file) {
        file << username << " " << password << endl;
        file.close();
        cout << "Registration successful! Please log in.\n";
    } else {
        cout << "Error opening user file!\n";
    }
}

// Login user
bool loginUser() {
    string username, password, fileUser, filePass;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    password = encryptPassword(password);
    ifstream file("users.txt");
    if (!file) {
        cout << "No users registered yet!\n";
        return false;
    }

    while (file >> fileUser >> filePass) {
        if (fileUser == username && filePass == password) {
            file.close();
            loggedInUser = username;
            cout << "Login successful! Welcome, " << username << ".\n";
            return true;
        }
    }

    file.close();
    cout << "Invalid username or password! Try again.\n";
    return false;
}

// Convert date string "YYYY-MM-DD" to time_t
time_t stringToTimeT(const string &dateStr) {
    tm timeStruct = {};
    istringstream ss(dateStr);
    ss >> get_time(&timeStruct, "%Y-%m-%d");
    if (ss.fail()) return -1;
    return mktime(&timeStruct);
}

// Expense class
class Expense {
public:
    string type;
    string category;
    float amount;
    string date;
    string description;

    void inputExpense() {
        cin.ignore();
        cout << "Enter type (Personal/Business): ";
        getline(cin, type);
        cout << "Enter category: ";
        getline(cin, category);
        cout << "Enter amount: ";
        cin >> amount;
        cin.ignore();
        cout << "Enter date (YYYY-MM-DD): ";
        getline(cin, date);
        cout << "Enter description: ";
        getline(cin, description);
    }

    void displayExpense() {
        cout << "\n-----------------------------";
        cout << "\nType: " << type
             << "\nCategory: " << category
             << "\nAmount: " << fixed << setprecision(2) << amount
             << "\nDate: " << date
             << "\nDescription: " << description;
    }
};

// Add expense
void addExpense() {
    Expense exp;
    exp.inputExpense();

    ofstream file("expenses_" + loggedInUser + ".txt", ios::app);
    if (file) {
        file << exp.type << "|" << exp.category << "|" << exp.amount << "|" << exp.date << "|" << exp.description << "\n";
        file.close();
        cout << "Expense added successfully!\n";
    } else {
        cout << "Error opening file!\n";
    }
}

// View all expenses
void viewExpenses() {
    ifstream file("expenses_" + loggedInUser + ".txt");
    if (!file) {
        cout << "No expenses found!\n";
        return;
    }

    string line;
    cout << "\n--- Expense List ---";
    while (getline(file, line)) {
        stringstream ss(line);
        Expense exp;
        string amountStr;

        getline(ss, exp.type, '|');
        getline(ss, exp.category, '|');
        getline(ss, amountStr, '|');
        getline(ss, exp.date, '|');
        getline(ss, exp.description);

        try {
            exp.amount = stof(amountStr);
        } catch (...) {
            exp.amount = 0;
        }

        exp.displayExpense();
    }
    cout << "\n-----------------------------\n";
    file.close();
}

// Search by category
void searchExpenseByCategory(string searchCategory) {
    ifstream file("expenses_" + loggedInUser + ".txt");
    if (!file) {
        cout << "No expenses found!\n";
        return;
    }

    string line;
    Expense exp;
    bool found = false;
    while (getline(file, line)) {
        stringstream ss(line);
        string amountStr;
        getline(ss, exp.type, '|');
        getline(ss, exp.category, '|');
        getline(ss, amountStr, '|');
        getline(ss, exp.date, '|');
        getline(ss, exp.description);

        try {
            exp.amount = stof(amountStr);
        } catch (...) {
            exp.amount = 0;
        }

        if (exp.category == searchCategory) {
            exp.displayExpense();
            found = true;
        }
    }

    if (!found) {
        cout << "No expenses found for category: " << searchCategory << endl;
    }
    file.close();
}

// Calculate total expense
void calculateTotalExpense() {
    ifstream file("expenses_" + loggedInUser + ".txt");
    if (!file) {
        cout << "No expenses found!\n";
        return;
    }

    string line;
    Expense exp;
    float total = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string amountStr;
        getline(ss, exp.type, '|');
        getline(ss, exp.category, '|');
        getline(ss, amountStr, '|');
        getline(ss, exp.date, '|');
        getline(ss, exp.description);

        try {
            exp.amount = stof(amountStr);
        } catch (...) {
            exp.amount = 0;
        }

        total += exp.amount;
    }

    cout << "Total Expenses: " << fixed << setprecision(2) << total << endl;
    file.close();
}

// Get current week start and end date
void getCurrentWeekRange(string &startDate, string &endDate) {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int wday = ltm->tm_wday;
    if (wday == 0) wday = 7;

    ltm->tm_mday -= (wday - 1);
    mktime(ltm);

    char buf[11];
    strftime(buf, 11, "%Y-%m-%d", ltm);
    startDate = buf;

    ltm->tm_mday += 6;
    mktime(ltm);
    strftime(buf, 11, "%Y-%m-%d", ltm);
    endDate = buf;
}

// Weekly report
void generateWeeklyReport() {
    string startDateStr, endDateStr;
    getCurrentWeekRange(startDateStr, endDateStr);
    time_t startDate = stringToTimeT(startDateStr);
    time_t endDate = stringToTimeT(endDateStr);

    cout << "\nWeekly Report (" << startDateStr << " to " << endDateStr << ")\n";

    ifstream file("expenses_" + loggedInUser + ".txt");
    if (!file) {
        cout << "No expenses found!\n";
        return;
    }

    string line;
    Expense exp;
    float total = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string amountStr;
        getline(ss, exp.type, '|');
        getline(ss, exp.category, '|');
        getline(ss, amountStr, '|');
        getline(ss, exp.date, '|');
        getline(ss, exp.description);

        try {
            exp.amount = stof(amountStr);
        } catch (...) {
            exp.amount = 0;
        }

        time_t expDate = stringToTimeT(exp.date);
        if (expDate >= startDate && expDate <= endDate) {
            exp.displayExpense();
            total += exp.amount;
        }
    }

    cout << "\nTotal Weekly Expenses: " << fixed << setprecision(2) << total << endl;
    file.close();
}

// Monthly report
void generateMonthlyReport() {
    string year, month;
    cout << "Enter year (YYYY): ";
    cin >> year;
    cout << "Enter month (MM): ";
    cin >> month;

    string targetPrefix = year + "-" + month;

    ifstream file("expenses_" + loggedInUser + ".txt");
    if (!file) {
        cout << "No expenses found!\n";
        return;
    }

    string line;
    Expense exp;
    float total = 0;
    cout << "\nMonthly Report for " << month << "/" << year << ":\n";
    while (getline(file, line)) {
        stringstream ss(line);
        string amountStr;
        getline(ss, exp.type, '|');
        getline(ss, exp.category, '|');
        getline(ss, amountStr, '|');
        getline(ss, exp.date, '|');
        getline(ss, exp.description);

        try {
            exp.amount = stof(amountStr);
        } catch (...) {
            exp.amount = 0;
        }

        if (exp.date.substr(0, 7) == targetPrefix) {
            exp.displayExpense();
            total += exp.amount;
        }
    }

    cout << "\nTotal Monthly Expenses: " << fixed << setprecision(2) << total << endl;
    file.close();
}

// Main menu
int main() {
    int choice;
    string category;
    bool loggedIn = false;

    cout << "Welcome to Expense Management System\n";

    while (true) {
        while (!loggedIn) {
            cout << "\n1. Register\n";
            cout << "2. Login\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1) registerUser();
            else if (choice == 2) loggedIn = loginUser();
            else cout << "Invalid choice! Try again.\n";
        }

        do {
            cout << "\nExpense Management Menu";
            cout << "\n1. Add Expense";
            cout << "\n2. View Expenses";
            cout << "\n3. Search Expense by Category";
            cout << "\n4. Calculate Total Expenses";
            cout << "\n5. Weekly Report";
            cout << "\n6. Monthly Report";
            cout << "\n7. Exit";
            cout << "\nEnter your choice: ";
            cin >> choice;

            switch (choice) {
                case 1: addExpense(); break;
                case 2: viewExpenses(); break;
                case 3:
                    cout << "Enter category to search: ";
                    cin.ignore();
                    getline(cin, category);
                    searchExpenseByCategory(category);
                    break;
                case 4: calculateTotalExpense(); break;
                case 5: generateWeeklyReport(); break;
                case 6: generateMonthlyReport(); break;
                case 7:
                    loggedIn = false;
                    cout << "You have logged out. Returning to login/register screen...\n";
                    break;
                default: cout << "Invalid choice! Try again.\n";
            }
        } while (loggedIn);
    }

    return 0;
}
