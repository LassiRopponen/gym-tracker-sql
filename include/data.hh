#include <string>
#include <vector>
#include <format>
#include <sstream>
#include <string.h>

#include "sqlite3.h"

using namespace std;

/**
 * Used to signify if the user has currently selected an exercise or muscle.
 */
enum State {
    nothing,
    exercise,
    muscle
};

class Data {
    public:
        /**
         * Opens database connection with given name and initializes tables if necessary.
         */
        Data(const string &name);
        /**
         * Closes database connection.
         */
        ~Data();
        /**
         * Searches the database for an exercise with the given name using send_query with
         * single_callback as callback function.
         * Returns true if exercise was found.
         */
        bool select_exercise(const string &name);
        /**
         * Searches the database for a muscle with the given name using send_query with
         * single_callback as callback function.
         * Returns true if muscle was found.
         */
        bool select_muscle(const string &name);
        /**
         * Searches the database for exercises with a possible requirement using send_query with
         * list_callback as callback function.
         */
        void show_exercises(State current, const string &item);
         /**
         * Searches the database for muscles with a possible requirement using send_query with
         * list_callback as callback function
         */
        void show_muscles(State current, const string &item);
         /**
         * Searches the database for sets with a possible requirement using send_query with
         * list_callback as callback function
         */
        void show_sets(State current, const string &item);
         /**
         * Searches the database for exercises on a given date with a possible requirement using
         * send_query with list_callback as callback function
         */
        void show_sets_for_date(State current, const string &item, const string &date);
        /** 
         * Adds an exercise with the given information to the database.
         * Returns true if the operation was successful.
         */
        bool add_exercise(const string &name, const char* compound, const vector<string> &primary,
            const vector<string> &secondary);
        /**
         * Adds a muscle with the given information to the database.
         * Returns true if the operation was successful.
         */
        bool add_muscle(const string &name, const char* upper, const string &group);
        /**
         * Adds a set with the given information to the database.
         * Returns true if the operation was successful.
         */
        bool add_set(
            const string &exercise, const string &weight, const string &reps, const string &date);
        /**
         * Deletes an exercise or muscle with the given name from the database.
         */
        void delete_by_name(State current, const string &item);
        /**
         * Deletes a set with the given id from the database.
         */
        void delete_set(const string &id);
        /**
         * Updates the given boolean type value of an exercise or muscle in the database.
         */
        void update_bool(
            const string &name, const char* table, const char* col, const string &value);
        /** 
         * Updates the given text type value of an exercise or muscle in the database.
         */
        void update_text(
            const string &name, const char* table, const char* col, const string &value);
        /**
         * Updates the trains table with the given information.
         */
        bool update_muscles(const string &name, const char* main, const vector<string> &muscles);

        /**
         * Stores the output of the last database operation as a string.
         */
        string output = "";
    private:
        /**
         * Sends the given query to the database using the second parameter as the callback 
         * function.
         * Stores the output of the query or a possible error message to the output variable.
         */
        int send_query(const char* query, int (*callback)(void*, int, char**, char**));
        /**
         * Stores the output of a query in list format (one row on one line).
         */
        static int list_callback(void* ptr, int argc, char** argv, char** azColName);
        /**
         * Stores the output of a query in single format (one attribute on one line).
         */
        static int single_callback(void* ptr, int argc, char** argv, char** azColName);
        /** 
         * Stores the first element of the output of a query.
         * When used with the correct query, will store only the id of a row.
         */
        static int id_callback(void* ptr, int argc, char** argv, char** azColName);
        /**
         * Finds the corresponding ids for a given list of muscle names.
         */
        bool find_muscle_ids(const vector<string> &muscles, vector<string> &ids);
        /**
         * Converts date input in format DD.MM.YYYY (with no filler zeros) to ISO 8601 (YYYY-MM-DD)
         */
        bool convert_date_input(const string &date_input, string &date);
        /**
         * Converts dates and boolean values of a query's output to nicer format. Leaves other
         * values untouched.
         */
        static string convert_output(const char* col_name, const char* content);

        /**
         * The current database connection.
         */
        sqlite3* db;
};