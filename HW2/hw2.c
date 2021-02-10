#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

static const char *FILE_NAME = "ex2.txt"; // static to ensure no namespace issues
static const char *PASSWD = "supersecret";

struct User_s {
    char uid[8];
    char auths[4]; // "[read][write][execute][owner]"
};

typedef struct User_s User;
struct User_s users[4]; // initialize user array

void BuildUserList() {
    
    // Owner
    strncpy(users[0].uid, "bennen2", 8);
    strncpy(users[0].auths, "1111", 4);

    // User 1
    strncpy(users[1].uid, "fangj3", 7);
    strncpy(users[1].auths, "0011", 4);

    // User 2
    strncpy(users[2].uid, "losses", 7);
    strncpy(users[2].auths, "1100", 4);

    // Dev
    strncpy(users[3].uid, "root", 4);
    strncpy(users[3].auths, "1111", 4);


};

// get username from uid
// https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux
const char *GetCurUserName() {

  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  
  if ( pw ) {
    return pw->pw_name;
  }

  return "";

};

// decypher auths string
// returns true(1) if user has permission, false(0) if not
int CheckAuth( char *auths, char action ) {

    // read
    if ( action == 'r' ){
        if ( auths[0] == '1' ) 
            return 1;
        else 
            return 0;
    }
    // write
    else if ( action == 'w' ) {
        if ( auths[1] == '1' ) 
            return 1;
        else 
            return 0;
    }
    // execute
    else if ( action == 'x' ) {
        if ( auths[2] == '1' ) 
            return 1;
        else 
            return 0;
    }
    // clear
    else if ( action == 'c' ) {
        if ( auths[3] == '1' ) 
            return 1;
        else 
            return 0;
    }
    // error action not supported
    else {
        printf("Error: Action No Supported\n");
        exit(0);
    }

};

// check uid against all uid in user array
// use in if-statment
int VerifyUser(const char *uid) {

    int num_users = sizeof users / sizeof users[0];

    // loop through users and try to match uid
    for (int i=0; i < num_users; ++i) {

        if ( strcmp( users[i].uid, uid ) == 0) {
            return 1;
        }
    }

    return 0;

};

// look for uid and get auths from user array
// only call after verifying that a user exists
char* GetAuth(const char *uid)  {

    int num_users = sizeof users / sizeof users[0];

    // loop through users and try to match uid
    for (int i=0; i < num_users; ++i) {
        if ( strcmp( users[i].uid, uid ) == 0 ) {
            return users[i].auths;
        }
    }

    printf("Error: Can not complete GetAuth() user does not exist\n");
    exit(0);

};

int main( int argc, char *argv[] ) {

    BuildUserList();

    // check number of arguments
    if ( argc == 2 ) {
        
        const char *cur_uid = GetCurUserName();

        // check if current user in user array
        if ( VerifyUser(cur_uid) ) {
           
            // check argument command
            // READ
            if ( strcmp(argv[1], "read" ) == 0 ) {
                
                // check if user has permission
                if ( CheckAuth( GetAuth(cur_uid), 'r' ) )
                    system("less ex2.txt");
                else
                    printf("Error: No one said you could do that!\n");

            } 
            // WRITE
            else if ( strcmp(argv[1], "write" ) == 0 ) {

                // check if user has permission
                if ( CheckAuth( GetAuth(cur_uid), 'w' ) )
                    system("nano ex2.txt");
                else
                    printf("Error: No one said you could do that!\n");

            }
            // EXECUTE
            else if ( strcmp( argv[1], "execute" ) == 0 ) {
                
                // check if user has permission
                if ( CheckAuth( GetAuth(cur_uid), 'x' ) )
                    system("./ex2.txt");
                else
                    printf("Error: No one said you could do that!\n");
            }
            // CLEAR
            else if ( strcmp( argv[1], "clear" ) == 0 ) {
                
                // check if user has permission
                if ( CheckAuth( GetAuth(cur_uid), 'c' ) ) {
                    
                    // get password via stdin
                    char pass[16];

                    printf("Password: ");

                    if ( fgets( pass, 16, stdin ) != NULL ) {

                        pass[strcspn(pass, "\n")] = 0; // strip newline

                        // check if password correct
                        if ( strcmp( pass, PASSWD ) == 0 )
                            system("> ex2.txt");
                        else
                            printf("Error: Action Denied, Password Incorrect!\n");

                    }
                    // no password entered
                    else {
                        printf("Error: You must enter a password!\n");
                    }
                }
                else {
                    printf("Error: No one said you could do that!\n");
                }

            }
            else {
                printf("Error: Unrecognized Arguments");
            }

        }
        else {
            printf("Error: Beep boop beep... you're not on the list :(\n"); 
        }

        return 0;
   }
   else {
      printf("Error: Invalid Number of Arguments\n");
      return 0;
   }
}