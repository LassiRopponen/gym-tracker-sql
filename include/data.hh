#include "sqlite3.h"
#include <string>
#include <vector>
#include <format>

using namespace std;

class Data {
    public:
        Data(string name);
        ~Data();
        bool select_exercise(string name);
        bool select_muscle(string name);
        void show_exercises();
        void show_muscles();
        bool add_exercise(
            string name, const char* compound, vector<string> primary, vector<string> secondary);
        bool add_muscle(string name, const char* upper, string group);

        string output = "";
    private:
        int send_query(const char* query, int (*callback)(void*, int, char**, char**));
        static int list_callback(void* ptr, int argc, char** argv, char** azColName);
        static int single_callback(void* ptr, int argc, char** argv, char** azColName);
        static int id_callback(void* ptr, int argc, char** argv, char** azColName);
        bool find_muscle_ids(const vector<string> &muscles, vector<string> &ids);

        sqlite3* db;
};