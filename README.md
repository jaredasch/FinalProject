# Slicky-Wiki #
## Jared Asch and Colin Hosking - Systems Level Programming Period 5 ##

## Project Description ##
Slicky-Wiki is a wiki for culminating any and all knowledge on assorted topics. Users can add their browse existing pages, search all of the pages (by title or keyword), add their own pages, and edit existing pages. Our wiki will be available over a network connection and all of the data will be stored locally on a server. When first connecting to our server, clients will be asked to login, or prompted to create an account.

## Running the Server ##

1. Clone repository.
2. $ make
3. To run server: $ ./server
4. To connect as client from test IP: $ ./client
4. To connect as client: $ ./client server-address

## User-proofing Features ##

* A User cannot signup with a username that is in use already
* A User cannot create a page with a name of an existing page
* A User who enters in incorrect (or empty) command will be displayed an error message
* A User cannot attempt to edit pages outside the page directory

## How to Use the Wiki ##
After connecting to the server, the user will encounter a command prompt.

Login Commands:
* **signup -username- -password-**
  - Creates account
* **login -username- -password-**
  - Logs into existing account
* **help**
  - Displays help page
* **exit**
  - Terminates client

User Commands (once logged in):

* **create-page -name-**
  - Creates page with filename -name- and prompts user to enter contents in emacs
* **get-page -name-**
  - Displays page with filename -name-
* **edit-page -name-**
  - Opens page with filename -name- in emacs
* **search-titles -keyword-**
  - Displays list of pages beginning with -keyword-
* **search-contents -keyword-**
  - Searches every page and displays number of instances of -keyword- in each page
* **show-pages**
  - Displays list of all page names
* **help**
  - Displays help page
* **exit**
  - Terminates client

## Known Bugs ##

If the server terminates before all clients have exited, the server will not run again because of the address being in use. To work around this, either wait until the OS allows the address to open up or modify the port number in networking.h

## Attempted Features ##

We tried implementing semaphores to ensure no user could edit the same page at the same time. Whenever the edit-page function was called, the server created a semaphore using the page name to generate a unique key. edit-page also checked if such a semaphore existed, and if it was available. After the user edited, the semaphore was deleted. We got this feature working locally on MacOS, but it did not run on Linux. It also was very buggy when connecting not using the test IP, and the buffers would not send at the right time.
