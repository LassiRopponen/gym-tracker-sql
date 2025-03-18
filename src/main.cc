#include <iostream>

#include "data.hh"

using namespace std;

// git

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
                getline(cin, current_exercise);
                if (!database.select_exercise(current_exercise)) {
                    current_exercise = "";
                }
                cout << endl;
                cout << database.output << endl;
            }
            else if (input_type == "add") {
                string add_type;
                cin >> add_type;
                if (add_type == "muscle") {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    string name;
                    cout << "name: ";
                    getline(cin, name);

                    const char* upper;
                    cout << "upper body (y/n): ";
                    char upper_char;
                    cin >> upper_char;
                    switch(upper_char) {
                        case('y'):
                            upper = "TRUE";
                            break;
                        case('n'):
                            upper = "FALSE";
                            break;
                        default:
                            continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    string group;
                    cout << "muscle group: ";
                    getline(cin, group);

                    if (database.add_muscle(name, upper, group)) {
                        cout << "Added muscle succesfully." << endl;
                    }
                    else {
                        cout << "Adding muscle failed." << endl;
                        cout << database.output << endl;
                    }
                }
                else if (add_type == "exercise") {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    string name;
                    cout << "name: ";
                    getline(cin, name);

                    const char* compound;
                    cout << "compound exercise (y/n): ";
                    char compound_char;
                    cin >> compound_char;
                    switch(compound_char) {
                        case('y'):
                            compound = "TRUE";
                            break;
                        case('n'):
                            compound = "FALSE";
                            break;
                        default:
                            continue;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    vector<string> primary;
                    string muscle;
                    cout << "primary muscles (type 'done' to finish)" << endl;
                    while (true) {
                        cout << ">";
                        getline(cin, muscle);
                        if (muscle == "done") {
                            break;
                        }
                        primary.push_back(muscle);
                    }
                    vector<string> secondary;
                    muscle = "";
                    cout << "secondary muscles (type 'done' to finish)" << endl;
                    while (true) {
                        cout << ">";
                        getline(cin, muscle);
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
                else {
                    cout << "Add needs to be followed by 'muscle' or 'exercise'." << endl;
                }
            }
            else if (input_type == "quit") {
                break;
            }
        }
    }
    catch(std::string err_msg) {
        cerr << format("Opening database failed: {}", err_msg) << endl;
        return 1;
    }

    return 0;
}