# LocalWiki -- Jared Asch and Colin Hosking #

## Project Description ##
LocalWiki is a wiki for culminating any and all knowledge on assorted topics. Users will be able to add their browse existing pages, search all of the pages, add their own pages, and edit existing pages. Our wiki will be available over a network connection and all of the data will be stored locally on the server. When first connecting to our server, clients will be asked a nickname to go by, and we may add in accounts in later versions.

## User Interface ##
When first connecting to our application, users will be asked to enter in a nickname to go by. After the initial connection, the application will run a menu in a loop, where the user will be prompted to enter a number corresponding to what action they want to take. When editing existing pages, the terminal will open an already installed text editor (probably vim, emacs, or nano) to edit the page. When a user adds a page, they will be prompted for the title of the page, and then the edit interface will come up. For any other actions, the user will be prompted for any query information after they enter the action. The pages will store information about users who have contributed.

## Technical Design ##
This project will use numerous concepts learned throughout the semester. Users will access the wiki through a network connection, which will require a client/server design. We will also use semaphores to ensure that no two files are being edited at the same time, to ensure that data is not corrupted. We will also need to use exec or similar commands to invoke a text editor from our application. Each page will be stored in a file, which will require file I/O to read and write to the database.

## Expected Timeline ##
* Get forking server up and running (local server) - 1/2/2019 - Both
* Create basic text interface - 1/3/2019 - Jared
* Add pages from a client (no editing them yet) - 1/4/2019 - Colin
* Edit pages over local server (included semaphores) - 1/5/2019 - Jared
* Search and browse page titles - 1/6/2019 - Colin
* Search and browse through page contents - 1/7/2019 - Jared
* Adapt design for socket connections - 1/8/2019 or after learned - Colin
* Edit pages over socket connections (may require some more logistic work) - 1/9/2019 or after learned - Jared
* Add user accounts instead of nicknames - 1/12/2019 - Both
