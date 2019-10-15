#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <ctime>


int callback(void *NotUsed, int argc, char **argv, char **azColName){



  return 0;
}


int main(int argc, char* argv[]){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  char theName[50];
  char theExtentionedName[53];

  std::string sql_create_it = "CREATE TABLE IF NOT EXISTS WEATHER(Time long int, Temp float, Humid float)";



  printf("Enter the database file name:\n");
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



  // ======================================================== //
  // here serial comes in

  int fd;

  fd = open("/dev/ttyACM0",O_RDWR | O_NOCTTY);


  if(fd == -1)
         printf("\n  Error! in Opening ttyACM0  ");
  else
         printf("\n  ttyUSB0 Opened Successfully ");


  struct termios SerialPortSettings;	/* Create the structure                          */

  tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

  /* Setting the Baud rate */
  cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
  cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

  /* 8N1 Mode */
  SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
  SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
  SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
  SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

  SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
  SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


  SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
  SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

  SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

  /* Setting Time outs */
  SerialPortSettings.c_cc[VMIN] = 11; /* Read at least x characters */
  SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */


  if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
    printf("\n  ERROR ! in Setting attributes");
  else
    printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");

  printf("\n \n");


  const char mode = 'r';

  FILE *not_fd = fdopen(fd, &mode);


  tcflush(fd, TCIFLUSH);


  while(1){
    // sssssssssssssss

    int temp = 0;
    int humid = 0;

    char *the_damn_line = NULL;
    size_t len = 11;


    getline(&the_damn_line, &len, not_fd);

    sscanf(the_damn_line, "%i %i", &temp, &humid);


    float temp_f = (float)temp / 100;
    float humid_f = (float)humid / 100;

    time_t now = time(0);

    char* human_time = ctime(&now);

    std::cout << human_time;
    printf("temp - %.3f humid - %.3f \n \n", temp_f, humid_f);

    char entry_char[50];
    sprintf(entry_char, "INSERT INTO WEATHER VALUES(%li, %.3f, %.3f)", now, temp_f, humid_f);

    rc = sqlite3_exec(db, entry_char, callback, 0, &zErrMsg);

    if(rc){
      fprintf(stderr,"couldnt insert values\n \n");
    } else {
      fprintf(stderr,"The values are in the database\n \n");
    }

    printf("\n");
  }

  // ======================================================== //


  sqlite3_close(db);
  close(fd);
  return 0;
}
