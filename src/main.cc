#include <iostream>
#include <cctype>

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
            cout << database.output;
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
            cout << database.output;
        }
    }
    catch(...) {
        return;
    }
}

void update_bool(Data &database, string current_item, const char* table, const char* col) {
    const char* new_bool;
    char input_char = prompt_input("y/n: ")[0];
    switch(input_char) {
        case('y'):
            new_bool = "TRUE";
            break;
        case('n'):
            new_bool = "FALSE";
            break;
        default:
            return;
    }
    database.update_bool(current_item, table, col, new_bool);
    cout << "Done." << endl;
}

void update_muscles(Data &database, string current_item, const char* main) {
    vector<string> muscles;
    string muscle;
    cout << "type 'done' to finish" << endl;
    while (true) {
        muscle = prompt_input(">");
        if (muscle == "done") {
            break;
        }
        muscles.push_back(muscle);
    }
    if (database.update_muscles(current_item, main, muscles)) {
        cout << "Done." << endl;
    }
    else {
        cout << database.output;
    }
}

int main() {
    try {
        Data database = Data("test.db");
        State current = nothing;
        string current_item = "";
        string input;

        while (true) {
            cout << format("{}>", current_item);
            string input_type;
            cin >> input_type;

            if (input_type == "exercise") {
                current_item = read_input();
                current = exercise;
                if (!database.select_exercise(current_item)) {
                    current_item = "";
                    current = nothing;
                }
                else {
                    current = exercise;
                }
                cout << database.output;
            }
            else if (input_type == "muscle") {
                current_item = read_input();
                current = muscle;
                if (!database.select_muscle(current_item)) {
                    current_item = "";
                    current = nothing;
                }
                cout << database.output;
            }
            else if (input_type == "exercises") {
                database.show_exercises(current, current_item);
                cout << database.output;
            }
            else if (input_type == "muscles") {
                database.show_muscles(current, current_item);
                cout << database.output;
            }
            else if (input_type == "sets") {
                if (!iscntrl(cin.peek())) {
                    string date = read_input();
                    database.show_sets_for_date(current, current_item, date);
                }
                else {
                    database.show_sets(current, current_item);
                }
                cout << database.output;
            }
            else if (input_type == "add") {
                string add_type;
                cin >> add_type;
                if (add_type == "muscle") {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    get_muscle_input(database);
                }
                else if (add_type == "exercise") {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    get_exercise_input(database);
                }
                else if (add_type == "set") {
                    if (current != exercise) {
                        cout << "An exercise has to be selected to add set." << endl;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    string weight, reps, date;
                    cin >> weight >> reps >> date;
                    if (database.add_set(current_item, weight, reps, date)) {
                        cout << "Added set successfully." << endl; 
                    }
                    else {
                        cout << "Adding set failed." << endl;
                        cout << database.output;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                else {
                    cout << "Add needs to be followed by 'muscle', 'exercise' or 'set'." << endl;
                }
            }
            else if (input_type == "delete") {
                if (!iscntrl(cin.peek())) {
                    string id = read_input();
                    database.delete_set(id);
                    continue;
                }
                if (current == nothing) {
                    cout << "No item to delete." << endl;
                    continue;
                }
                database.delete_by_name(current, current_item);
                current = nothing;
                current_item = "";
            }
            else if (input_type == "update") {
                string update_type = read_input();
                if (current == exercise) {
                    if (update_type == "name") {
                        string new_name = prompt_input("new name: ");
                        database.update_text(current_item, "exercise", "name", new_name);
                        current_item = new_name;
                    }
                    else if (update_type == "compound") {
                        update_bool(database, current_item, "exercise", "compound");
                    }
                    else if (update_type == "primary") {
                        update_muscles(database, current_item, "TRUE");
                    }
                }
            }
            else if (input_type == "back") {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                current = nothing;
                current_item = "";
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