

#include <iostream>
#include <string>
#include <httpserver.hpp>
#include <sqlite3.h>
#include <ctime>


using namespace httpserver;

std::string big;
sqlite3 *db;
char *zErrMsg = 0;


int callback(void *NotUsed, int argc, char **argv, char **azColName){ //for sqlite a
  std::time_t time_temp;
  int i;
  for(i = 0; i < argc; i++){
    if(strcmp (azColName[i], "Time") == 0){
      std::time_t time_correct = std::stoi(argv[i]);
      time_temp = time_correct;
      big = big + " ";
    } else {
      big = big + argv[i] + "  |  ";
      if(strcmp (azColName[i], "Humid") == 0){
        big = big + ctime(&time_temp);
      }
    }
  }

  return 0;
}

class weather_res : public http_resource {
public:
    const std::shared_ptr<http_response> render(const http_request&) {
      // yeeeeeees
      //take stuff from database
      big.clear();

      int errrr;
      std::string get_data = "SELECT * FROM WEATHER;";

      errrr = sqlite3_exec(db, get_data.c_str(), callback, 0, &zErrMsg);

      // stuff to draw the graph and show the database in general yes
      return std::shared_ptr<http_response>(new string_response(" Temp  |  Humid |  Time\n" + big));
    }
};



int main() {

  int rc;
  char theName[50];
  char theExtentionedName[53];

  std::string sql_create_it = "CREATE TABLE IF NOT EXISTS WEATHER(Time long int, Temp float, Humid float)";



  printf("Enter the database file name to use for web server:\n");
  scanf("%s", theName);

  sprintf(theExtentionedName, "%s.db", theName);

  rc = sqlite3_open(theExtentionedName, &db);

  if(rc){
    fprintf(stderr, "Couldnt open the database: %s\n", sqlite3_errmsg(db));
    return 1;
  } else {
    fprintf(stderr, "The database has been opened successfully.\n");
  }

  rc = sqlite3_exec(db, sql_create_it.c_str(), callback, 0, &zErrMsg);

  if(rc){
    fprintf(stderr, "Failed creating a table.\n");
  } else {
    fprintf(stderr, "The table exists now, or it did, its there.\n");
  }

  // end of sqlite setting up
  webserver ws = create_webserver(8080);



  weather_res hwr;
  ws.register_resource("/weather", &hwr);
  ws.start(true);

  sqlite3_close(db);

  return 0;
}
