#include <string>
#include <vector>
#include <format>
#include <sstream>
#include <string.h>

#include "sqlite3.h"

using namespace std;

enum State {
    nothing,
    exercise,
    muscle
};

class Data {
    public:
        Data(const string &name);
        ~Data();
        bool select_exercise(const string &name);
        bool select_muscle(const string &name);
        void show_exercises(State current, const string &item);
        void show_muscles(State current, const string &item);
        void show_sets(State current, const string &item);
        void show_sets_for_date(State current, const string &item, const string &date);
        bool add_exercise(const string &name, const char* compound, const vector<string> &primary,
            const vector<string> &secondary);
        bool add_muscle(const string &name, const char* upper, const string &group);
        bool add_set(
            const string &exercise, const string &weight, const string &reps, const string &date);
        void delete_by_name(State current, const string &item);
        void delete_set(const string &id);
        void update_bool(
            const string &name, const char* table, const char* col, const string &value);
        void update_text(
            const string &name, const char* table, const char* col, const string &value);
        bool update_muscles(const string &name, const char* main, const vector<string> &muscles);

        string output = "";
    private:
        int send_query(const char* query, int (*callback)(void*, int, char**, char**));
        static int list_callback(void* ptr, int argc, char** argv, char** azColName);
        static int single_callback(void* ptr, int argc, char** argv, char** azColName);
        static int id_callback(void* ptr, int argc, char** argv, char** azColName);
        bool find_muscle_ids(const vector<string> &muscles, vector<string> &ids);
        bool convert_date_input(const string &date_input, string &date);
        static string convert_output(const char* col_name, const char* content);

        sqlite3* db;
};