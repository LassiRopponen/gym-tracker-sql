#include "data.hh"

#include <iostream>

Data::Data(string name) {
    if(sqlite3_open(name.c_str(), &db) != SQLITE_OK) {
        throw sqlite3_errmsg(db);
    }

    const char* query = "PRAGMA foreign_keys = ON;"  \
                        "CREATE TABLE IF NOT EXISTS exercise(" \
                            "id INTEGER PRIMARY KEY NOT NULL," \
                            "name VARCHAR(100) UNIQUE NOT NULL," \
                            "compound BOOLEAN NOT NULL);" \
                        "CREATE TABLE IF NOT EXISTS muscle(" \
                            "id INTEGER PRIMARY KEY NOT NULL," \
                            "name VARCHAR(100) UNIQUE NOT NULL," \
                            "upper BOOLEAN NOT NULL," \
                            "muscle_group VARCHAR(50));" \
                        "CREATE TABLE IF NOT EXISTS trains(" \
                            "exercise_id INTEGER NOT NULL," \
                            "muscle_id INTEGER NOT NULL," \
                            "main BOOLEAN NOT NULL," \
                            "PRIMARY KEY (exercise_id, muscle_id))" \
                            "FOREIGN KEY (exercise_id) REFERENCES exercise (id) " \
                            "ON DELETE CASCADE" \
                            "FOREIGN KEY (muscle_id) REFERENCES muscle (id) " \
                            "ON DELETE CASCADE;" \
                        "CREATE TABLE IF NOT EXISTS working_set(" \
                            "id INTEGER PRIMARY KEY NOT NULL," \
                            "weight FLOAT NOT NULL," \
                            "reps INTEGER NOT NULL," \
                            "date VARCHAR(9) DEFAULT (date('now', 'localtime'))," \
                            "exercise_id INTEGER NOT NULL," \
                            "FOREIGN KEY (exercise_id) REFERENCES exercise (id)) " \
                            "ON DELETE CASCADE;";
    send_query(query, NULL);
}

Data::~Data() {
    sqlite3_close(db);
}

bool Data::select_exercise(string name) {
    string query = format("SELECT name, compound FROM exercise WHERE name='{}';", name);
    if (send_query(query.c_str(), single_callback) != SQLITE_OK) {
        return false;
    }
    if (output == "") {
        output = format("No exercise with name {}.\n", name);
        return false;
    }
    return true;
}

bool Data::select_muscle(string name) {
    string query = format("SELECT name, upper, muscle_group FROM muscle WHERE name='{}';", name);
    if (send_query(query.c_str(), single_callback) != SQLITE_OK) {
        return false;
    }
    if (output == "") {
        output = format("No muscle with name {}.\n", name);
        return false;
    }
    return true;
}

void Data::show_exercises(State current, string item) {
    if (current == muscle) {
        string query = format("SELECT e.name, e.compound, t.main FROM exercise e " \
                        "INNER JOIN trains t ON e.id = t.exercise_id " \
                        "INNER JOIN muscle m ON t.muscle_id = m.id " \
                        "AND m.name = '{}';", item);
        send_query(query.c_str(), list_callback);
    }
    else {
        const char* query = "SELECT name, compound FROM exercise;";
        send_query(query, list_callback);
    }
    if (output == "") {
        output = "No exercises.\n";
    }
}

void Data::show_muscles(State current, string item) {
    if (current == exercise) {
        string query = format("SELECT m.name, m.upper, m.muscle_group, t.main FROM muscle m " \
                        "INNER JOIN trains t ON m.id = t.muscle_id " \
                        "INNER JOIN exercise e ON t.exercise_id = e.id " \
                        "AND e.name = '{}';", item);
        send_query(query.c_str(), list_callback);
    }
    else {
        const char* query = "SELECT name, upper, muscle_group FROM muscle;";
        send_query(query, list_callback);
    }
    if (output == "") {
        output = "No muscles.\n";
    }
}

void Data::show_sets(State current, string item) {
    string query = "SELECT e.name AS exercise, s.date, s.weight, s.reps FROM working_set s " \
                    "INNER JOIN exercise e ON s.exercise_id = e.id;";
    switch (current) {
        case exercise:
            query = format("SELECT s.date, s.weight, s.reps FROM working_set s " \
                            "INNER JOIN exercise e ON s.exercise_id = e.id " \
                            "AND e.name = '{}';", item);
            break;
        case muscle:
            query =
                format("SELECT e.name AS exercise, s.date, s.weight, s.reps FROM working_set s " \
                    "INNER JOIN exercise e ON s.exercise_id = e.id " \
                    "INNER JOIN trains t ON e.id = t.exercise_id " \
                    "INNER JOIN muscle m ON t.muscle_id = m.id " \
                    "AND m.name = '{}';", item);
            break;
    }
    send_query(query.c_str(), list_callback);
    if (output == "") {
        output = "No sets.\n";
    }
}

void Data::show_sets_for_date(State current, string item, string date_input) {
    string date;
    if (!convert_date_input(date_input, date)) {
        return;
    }
    string query =
        format("SELECT e.name AS exercise, s.date, s.weight, s.reps FROM working_set s " \
            "INNER JOIN exercise e ON s.exercise_id = e.id AND s.date = '{}';", date);
    switch (current) {
        case exercise:
            query = 
                format("SELECT e.name AS exercise, s.date, s.weight, s.reps FROM working_set s " \
                        "INNER JOIN exercise e ON s.exercise_id = e.id " \
                        "AND e.name = '{}' AND s.date = '{}';", item, date);
            break;
        case muscle:
            query = 
                format("SELECT e.name AS exercise, s.date, s.weight, s.reps FROM working_set s " \
                        "INNER JOIN exercise e ON s.exercise_id = e.id " \
                        "INNER JOIN trains t ON e.id = t.exercise_id " \
                        "INNER JOIN muscle m ON t.muscle_id = m.id " \
                        "AND m.name = '{}' AND s.date = '{}';", item, date);
            break;
    }
    send_query(query.c_str(), list_callback);
    if (output == "") {
        output = "No sets.\n";
    }
}

bool Data::add_exercise(
        string name, const char* compound, vector<string> primary, vector<string> secondary
    ) {
    vector<string> primary_ids;
    if (!find_muscle_ids(primary, primary_ids)) {
        return false;
    }
    vector<string> secondary_ids;
    if (!find_muscle_ids(secondary, secondary_ids)) {
        return false;
    }

    string exercise_query = 
        format("INSERT INTO exercise(name,compound) VALUES('{}',{});", name, compound);
    int result = send_query(exercise_query.c_str(), NULL);
    if (result != SQLITE_OK) {
        if (result == SQLITE_CONSTRAINT) {
            output = format("Exercise with name {} already exists.\n", name);
        }
        return false;
    }

    int exercise_id = sqlite3_last_insert_rowid(db);
    for (string muscle_id : primary_ids) {
        string trains_query = 
            format("INSERT INTO trains(exercise_id,muscle_id,main) VALUES({},{},TRUE);",
            exercise_id, muscle_id);
        if (send_query(trains_query.c_str(), NULL) != SQLITE_OK) {
            return false;
        }
    }
    for (string muscle_id : secondary_ids) {
        string trains_query = 
            format("INSERT INTO trains(exercise_id,muscle_id,main) VALUES({},{},FALSE);",
            exercise_id, muscle_id);
        if (send_query(trains_query.c_str(), NULL) != SQLITE_OK) {
            return false;
        }
    }

    return true;
}

bool Data::add_muscle(string name, const char* upper, string group) {
    string query = format(
        "INSERT INTO muscle(name,upper,muscle_group) VALUES('{}',{},'{}');", name, upper, group);
    int result = send_query(query.c_str(), NULL);
    if (result != SQLITE_OK) {
        if (result == SQLITE_CONSTRAINT) {
            output = format("Muscle with name {} already exists.\n", name);
        }
        return false;
    }
    return true;
}

bool Data::add_set(string exercise, string weight_input, string reps_input, string date_input) {
    string exercise_query = format("SELECT id FROM exercise WHERE name='{}';", exercise);
    if (send_query(exercise_query.c_str(), id_callback) != SQLITE_OK) {
        return false;
    }
    if (output == "") {
        output = format("No exercise with name {}.\n", exercise);
        return false;
    }
    string exercise_id = output;
    float weight;
    try {
        weight = stof(weight_input);
        if (weight < 0) {
            throw;
        }
    }
    catch (...) {
        output = "Weight must be a positive decimal number.\n";
        return false;
    }
    int reps;
    try {
        reps = stoi(reps_input);
        if (reps < 0) {
            throw;
        }
    }
    catch (...) {
        output = "Reps must be a natural number.\n";
        return false;
    }
    string date;
    if (!convert_date_input(date_input, date)) {
        return false;
    }
    string query = 
        format("INSERT INTO working_set(weight,reps,date,exercise_id) VALUES({:.2f},{},'{}',{});",
        weight, reps, date, exercise_id);
    if (send_query(query.c_str(), NULL) != SQLITE_OK) {
        return false;
    }
    return true;
}

void Data::delete_by_name(State current, string item) {
    const char* table = current == exercise ? "exercise" : "muscle";
    string query = format("DELETE FROM {} WHERE name = '{}';", table, item);
    send_query(query.c_str(), NULL);
}


int Data::send_query(const char* query, int (*callback)(void*, int, char**, char**)) {
    output = "";
    char* err_msg;
    int result = sqlite3_exec(db, query, callback, &output, &err_msg);
    if (result != SQLITE_OK) {
        sqlite3_free(err_msg);
        output = sqlite3_errmsg(db);
        output += "\n";
    }
    return result;
}

int Data::list_callback(void* ptr, int argc, char** argv, char** azColName)
{
    string* output = static_cast<string*>(ptr);
    for (int i = 0; i < argc; i++) {
        string col = azColName[i];
        col += ": ";
        col += convert_output(azColName[i], argv[i]);
        col += "; ";
        output->append(col);
    }
    output->append("\n");
    return 0;
}

int Data::single_callback(void* ptr, int argc, char** argv, char** azColName)
{
    string* output = static_cast<string*>(ptr);
    for (int i = 0; i < argc; i++) {
        string col = azColName[i];
        col += ": ";
        col += convert_output(azColName[i], argv[i]);
        col += "\n";
        output->append(col);
    }
    return 0;
}

int Data::id_callback(void* ptr, int argc, char** argv, char** azColName)
{
    string* output = static_cast<string*>(ptr);
    if (argc > 0) {
        output->append(argv[0] ? argv[0] : "NULL");
    }
    return 0;
}

bool Data::find_muscle_ids(const vector<string> &muscles, vector<string> &ids) {
    for (string muscle : muscles) {
        string muscle_query = format("SELECT id FROM muscle WHERE name='{}';", muscle);
        if (send_query(muscle_query.c_str(), id_callback) != SQLITE_OK) {
            return false;
        }
        if (output == "") {
            output = format("No muscle with name {}.\n", muscle);
            return false;
        }
        ids.push_back(output);
    }
    return true;
}

bool Data::convert_date_input(const string &date_input, string &date) {
    size_t first_period = date_input.find_first_of('.');
    size_t second_period = date_input.substr(first_period+1,string::npos).find_first_of('.');
    string day = date_input.substr(0, first_period);
    string month = date_input.substr(first_period+1, second_period);
    string year = date_input.substr(first_period+second_period+2, string::npos);
    if (day.size() < 2) {
        day.insert(0, "0");
    }
    if (month.size() < 2) {
        month.insert(0, "0");
    }
    while (year.size() < 4) {
        year.insert(0, "0");
    }
    date = year + "-" + month + "-" + day;
    if (send_query(format("SELECT date('{}');", date).c_str(), id_callback) != SQLITE_OK) {
        return false;
    }
    if (output == "NULL") {
        output = "Incorrect date.\n";
        return false;
    }
    return true;
}

string Data::convert_output(const char* col_name, const char* content) {
    if (strcmp(col_name, "date") == 0) {
        int day, month, year;
        char delim;
        istringstream iss(content);
        iss >> year >> delim >> month >> delim >> day;
        ostringstream date;
        date << day << '.' << month << '.' << year;
        return date.str();
    }
    if (strcmp(col_name, "compound") == 0 || 
        strcmp(col_name, "upper") == 0 || 
        strcmp(col_name, "main") == 0)
    {
        switch (content[0]) {
            case '1':
                return "yes";
            default:
                return "no";
        }
    }
    else {
        return content ? content : "NULL";
    }
}