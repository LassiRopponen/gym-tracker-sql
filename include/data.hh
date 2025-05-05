#include <string>
#include <vector>
#include <format>

#include "sqlite3.h"

using namespace std;

enum State {
    nothing,
    exercise,
    muscle
};

class Data {
    public:
        Data(string name);
        ~Data();
        bool select_exercise(string name);
        bool select_muscle(string name);
        void show_exercises(State current, string item);
        void show_muscles(State current, string item);
        void show_sets(State current, string item);
        bool add_exercise(
            string name, const char* compound, vector<string> primary, vector<string> secondary);
        bool add_muscle(string name, const char* upper, string group);
        bool add_set(string exercise, string weight, string reps, string date);

        string output = "";
    private:
        int send_query(const char* query, int (*callback)(void*, int, char**, char**));
        static int list_callback(void* ptr, int argc, char** argv, char** azColName);
        static int single_callback(void* ptr, int argc, char** argv, char** azColName);
        static int id_callback(void* ptr, int argc, char** argv, char** azColName);
        bool find_muscle_ids(const vector<string> &muscles, vector<string> &ids);
        bool convert_date_input(const string &date_input, string &date);

        sqlite3* db;
};