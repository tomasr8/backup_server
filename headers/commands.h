#ifndef COMMANDS_H_
#define COMMANDS_H_

/**
* Definitions of commands, statuses and errors
* used in the application protocol
*
*/

/* Commands used in requests */

// returns resource from both servers if possible
#define GET 1
// returns ctime from current server
#define LAST_MODIFIED 3
// sets resource on both servers
#define SET 4

/* Statuses used in responses */

// returned with optional message on success
#define OK 200
// returned from server on error
#define ERROR 400
// placeholder command
#define UNKNOWN 0

/* Erros */

#define E_DISCONNECTED -10

#endif
