#ifndef COMMANDS_H_
#define COMMANDS_H_

/**
* Definitions of commands, statuses and errors
* used in the application protocol
*
*/

#define PRODUCTION 0

#define CLIENT_LOGGER "backupServer-client"
#define SERVER_LOGGER "backupServer-server"
#define TESTSUITE_LOGGER "backupServer-testSuite"

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

#define E_CONNECTION -10
#define E_PARSE -20

/* Identification */

#define CLIENT 123
#define SERVER 456

#define RESOURCE_MAX 32

#endif
