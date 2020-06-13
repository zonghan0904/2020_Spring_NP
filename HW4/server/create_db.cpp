#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i < argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int create_db(int rc, char *sql, char *zErrMsg, sqlite3 *db){
   /* Open database */
   rc = sqlite3_open("users.db", &db);

   if(rc) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   }
   else {
      fprintf(stdout, "Opened database successfully\n");
   }

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

   if(rc != SQLITE_OK){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Table created successfully\n");
   }
   sqlite3_close(db);

}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;

   /* Create SQL statement */
   sql = "CREATE TABLE IF NOT EXISTS USERS("  \
      "UID INTEGER PRIMARY KEY     AUTOINCREMENT," \
      "Username        TEXT    NOT NULL UNIQUE," \
      "Email           TEXT    NOT NULL," \
      "Password        TEXT    NOT NULL);";
   create_db(rc, sql, zErrMsg, db);

   sql = "CREATE TABLE IF NOT EXISTS BOARDS("  \
      "UID INTEGER PRIMARY KEY     AUTOINCREMENT," \
      "Boardname       TEXT    NOT NULL UNIQUE," \
      "Username        TEXT    NOT NULL);";
   create_db(rc, sql, zErrMsg, db);

   sql = "CREATE TABLE IF NOT EXISTS POSTS("  \
      "UID INTEGER PRIMARY KEY     AUTOINCREMENT," \
      "Title           TEXT    NOT NULL," \
      "Author          TEXT    NOT NULL," \
      "Content         TEXT    NOT NULL," \
      "Boardname       TEXT    NOT NULL," \
      "Comment         TEXT    ," \
      "Date            TEXT    NOT NULL);";
   create_db(rc, sql, zErrMsg, db);

   return 0;
}
