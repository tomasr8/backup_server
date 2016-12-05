#ifndef COMMANDS_H_
#define COMMANDS_H_

#define GET 1 // gets from both servers if possible
#define GET_ONCE 2 // gets from current server
#define LAST_MODIFIED 3
#define SET 4 // sets on both servers
#define SET_ONCE 5 // sets on current server
#define SET_MULTIPART 6
#define SET_MULTIPART_FINISHED 7

#define OK 50
#define OK_DATA_MULTIPART 51
#define OK_DATA_MULTIPART_FINISHED 52
#define ERROR 53

#define UNKNOWN 0

#endif
