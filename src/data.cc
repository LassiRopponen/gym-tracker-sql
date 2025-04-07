#include <iostream>

#include "data.hh"

Data::Data(string name) {
    if(sqlite3_open(name.c_str(), &db) != SQLITE_OK) {
        throw sqlite3_errmsg(db);
    }

    const char* query = "CREATE TABLE IF NOT EXISTS exercise(" \
                            "id INTEGER PRIMARY KEY NOT NULL," \
                            "name VARCHAR(100) UNIQUE NOT NULL," \
                            "compound BOOLEAN NOT NULL);" \
                        "CREATE TABLE IF NOT EXISTS muscle(" \
                            "id INTEGER PRIMARY KEY NOT NULL," \
                            "name VARCHAR(100) UNIQUE NOT NULL," \
                            "upper BOOLEAN NOT NULL," \
                            "muscle_group VARCHAR(50));"
                        "CREATE TABLE IF NOT EXISTS trains(" \
                            "exercise_id INTEGER NOT NULL," \
                            "muscle_id INTEGER NOT NULL," \
                            "main BOOLEAN NOT NULL," \
                            "PRIMARY KEY (exercise_id, muscle_id));" \
                        "CREATE TABLE IF NOT EXISTS set(" \
                            "id INTEGER PRIMARY KEY NOT NULL," \
                            "weight FLOAT," \
                            "reps INTEGER NOT NULL," \
                            "date varchar(9) DEFAULT date('now', 'localtime');";                 

    send_query(query, NULL);
}

Data::~Data() {
    sqlite3_close(db);
}

bool Data::select_exercise(string name) {
    string query = format("SELECT * FROM exercise WHERE name='{}';", name);
    if (send_query(query.c_str(), single_callback) != SQLITE_OK) {
        return false;
    }
    if (output == "") {
        output = format("No exercise with name {}.", name);
        return false;
    }
    return true;
}

bool Data::select_muscle(string name) {
    string query = format("SELECT * FROM muscle WHERE name='{}';", name);
    if (send_query(query.c_str(), single_callback) != SQLITE_OK) {
        return false;
    }
    if (output == "") {
        output = format("No muscle with name {}", name);
        return false;
    }
    return true;
}

void Data::show_exercises() {
    const char* query = "SELECT * FROM exercise;";
    send_query(query, list_callback);
}

void Data::show_muscles() {
    const char* query = "SELECT * FROM muscle;";
    send_query(query, list_callback);
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
            output = format("Exercise with name {} already exists.", name);
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
            output = format("Muscle with name {} already exists.", name);
        }
        return false;
    }
    return true;
}

int Data::send_query(const char* query, int (*callback)(void*, int, char**, char**)) {
    output = "";
    char* err_msg;
    int result = sqlite3_exec(db, query, callback, &output, &err_msg);
    if (result != SQLITE_OK) {
        sqlite3_free(err_msg);
        output = sqlite3_errmsg(db);
    }
    return result;
}

int Data::list_callback(void* ptr, int argc, char** argv, char** azColName)
{
    std::string* output = static_cast<std::string*>(ptr);
    for (int i = 0; i < argc; i++) {
        std::string col = azColName[i];
        col += ": ";
        col += argv[i] ? argv[i] : "0";
        col += "; ";
        output->append(col);
    }
    output->append("\n");
    return 0;
}

int Data::single_callback(void* ptr, int argc, char** argv, char** azColName)
{
    std::string* output = static_cast<std::string*>(ptr);
    for (int i = 0; i < argc; i++) {
        std::string col = azColName[i];
        col += ": ";
        col += argv[i] ? argv[i] : "0";
        col += "\n";
        output->append(col);
    }
    return 0;
}

int Data::id_callback(void* ptr, int argc, char** argv, char** azColName)
{
    std::string* output = static_cast<std::string*>(ptr);
    if (argc > 0) {
        output->append(argv[0]);
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
            output = format("No muscle with name {}.", muscle);
            return false;
        }
        ids.push_back(output);
    }
    return true;
}