#include <iostream>

#include "data.hh"

using namespace std;

string read_input() {
    string input;
    getline(cin, input);
    input.erase(0,input.find_first_not_of(' '));
    // will never throw even with no non-space chars as string::npos+1 overflows to 0
    input.erase(input.find_last_not_of(' ')+1,string::npos);
    return input;
}

string prompt_input(string prompt) {
    cout << prompt;
    string input = read_input();
    if (input == "cancel") {
        throw "cancel";
    }
    return input;
}

void get_muscle_input(Data &database) {
    try {
        string name = prompt_input("name: ");

        const char* upper;
        char upper_char = prompt_input("upper body (y/n): ")[0];
        switch(upper_char) {
            case('y'):
                upper = "TRUE";
                break;
            case('n'):
                upper = "FALSE";
                break;
            default:
                throw;
        }

        string group = prompt_input("muscle group: ");

        if (database.add_muscle(name, upper, group)) {
            cout << "Added muscle succesfully." << endl;
        }
        else {
            cout << "Adding muscle failed." << endl;
            cout << database.output << endl;
        }
    }
    catch(...) {
        return;
    }
}

void get_exercise_input(Data &database) {
    try {
        string name = prompt_input("name: ");

        const char* compound;
        char compound_char = prompt_input("compound exercise (y/n): ")[0];
        switch(compound_char) {
            case('y'):
                compound = "TRUE";
                break;
            case('n'):
                compound = "FALSE";
                break;
            default:
                throw;
        }

        vector<string> primary;
        string muscle;
        cout << "primary muscles (type 'done' to finish)" << endl;
        while (true) {
            muscle = prompt_input(">");
            if (muscle == "done") {
                break;
            }
            primary.push_back(muscle);
        }
        vector<string> secondary;
        muscle = "";
        cout << "secondary muscles (type 'done' to finish)" << endl;
        while (true) {
            muscle = prompt_input(">");
            if (muscle == "done") {
                break;
            }
            secondary.push_back(muscle);
        }

        if (database.add_exercise(name, compound, primary, secondary)) {
            cout << "Added exercise succesfully." << endl;
        }
        else {
            cout << "Adding exercise failed." << endl;
            cout << database.output << endl;
        }
    }
    catch(...) {
        return;
    }
}

int main() {
    try {
        Data database = Data("test.db");
        string current_exercise = "";
        string input;

        while (true) {
            cout << format("{}>", current_exercise);
            string input_type;
            cin >> input_type;

            if (input_type == "exercise") {
                current_exercise = read_input();
                if (!database.select_exercise(current_exercise)) {
                    current_exercise = "";
                }
                cout << database.output << endl;
            }
            else if (input_type == "exercises") {
                database.show_exercises();
                cout << database.output << endl;
            }
            else if (input_type == "muscles") {
                database.show_muscles();
                cout << database.output << endl;
            }
            else if (input_type == "add") {
                string add_type = read_input();
                if (add_type == "muscle") {
                    get_muscle_input(database);
                }
                else if (add_type == "exercise") {
                    get_exercise_input(database);
                }
                else {
                    cout << "Add needs to be followed by 'muscle' or 'exercise'." << endl;
                }
            }
            else if (input_type == "quit") {
                break;
            }
            else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Unknown command." << endl;
            }
        }
    }
    catch(string err_msg) {
        cerr << format("Opening database failed: {}", err_msg) << endl;
        return 1;
    }

    return 0;
}