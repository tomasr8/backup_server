#ifndef COMMANDS_H_
#define COMMANDS_H_

/**
* Definitions of commands, statuses, errors and other constants
* used in the application
*
*/

// if set to 1, turns off logging to stderr
// will still log to syslog
#define PRODUCTION 0

// used in openlog() to determine where to log messages
#define CLIENT_LOGGER "backupServer-client"
#define SERVER_LOGGER "backupServer-server"
#define TESTSUITE_LOGGER "backupServer-testSuite"

/* Commands used in requests */

// if sent by client returns the newest resource
// if sent by server returns the specified resource
#define GET 1

// if sent by client, overwrites resource on both servers
// if sent by server, overwrites resource on the current server
#define SET 4

/* Statuses used in responses */

// returned with optional message on success
#define OK 200
// returned with message on error
#define ERROR 400
// placeholder command
#define UNKNOWN 0

/* Errors */

#define E_CONNECTION -10
#define E_PARSE -20

/* Identification */

// when client or server initiates a connection it sends an identification
#define CLIENT 123
#define SERVER 456

// maximum resource number
#define RESOURCE_MAX 32

#endif
