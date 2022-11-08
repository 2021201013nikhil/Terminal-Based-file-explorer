#include<bits/stdc++.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <stack>
#include <unordered_map>
#include <dirent.h>
#include <pwd.h>
#include <cstdint>
#include <filesystem>
#include <grp.h>
using namespace std;

struct termios orig_termios;
void die(const char *s) {
    perror(s);
    exit(1);
}
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}
class EXPLORER {
    public:
    string home;                                // home with slash
    string homeWOSlash;                         // home without slash
    int refresh = 0;
    stack<string> forward,backward;             // forward movement and backward movement stacks
    unordered_map <string,int> pathToFiles;     // map for storing path of some important files 
    vector <vector<string> > allDirList;        // maintain all visited directories and link of files to where inside files are present
    vector<string> commandVector;
    int typeOfMode = 1;			                // 1-normal mode
    int numOfFilesToShow;   /* row_len */       // number of files to display on terminal
    int pointTofile = 0;                        // directory ki list mein se abhi konsi file k upar cursor hai
    int indexx = 0;                             // place where need to insert, word as a key for directory in pathToFiles
    int filesize = 0; /* dir_len */             // files in a particular directory
    string presentDirectory;                    // presentDirectory
    struct winsize ws;                          // window size
    int start = 0, end;                         // start and end location of a folder we are printing on terminal
    char input;                                 // key input taken on terminal
    int kabhiHomepresshuathakinahi = 0;
    string realHOME;                             // real home with slash
    string realHOMEWOSlash;                      // real home without slash
    string commandExecuted = "";
    string commandString;
    int noslash = 0;
    bool specialSearch = false;

    void printAllInfo (string s) {
        struct stat sb;
        string fullPath = presentDirectory+s;
        int n = fullPath.length()+1;
        char curPath[n+1];
        strcpy(curPath, fullPath.c_str());
        if (stat(curPath, &sb) == -1) {
            perror("Error on print All info");
            exit(EXIT_FAILURE);
        }
        for(int i=0;i<20;i++) {
            if(i<s.size()) {
                cout<<s[i];
            } else {
                cout<<" ";
            }
        }
        cout<<"\t";
        printf((sb.st_mode & S_IRUSR)? "r":"-");
        printf((sb.st_mode & S_IWUSR)? "w":"-");
        printf((sb.st_mode & S_IXUSR)? "x":"-");
        printf(" ");
        printf((sb.st_mode & S_IRGRP)? "r":"-");
        printf((sb.st_mode & S_IWGRP)? "w":"-");
        printf((sb.st_mode & S_IXGRP)? "x":"-");
        printf(" ");
        printf((sb.st_mode & S_IROTH)? "r":"-");
        printf((sb.st_mode & S_IWOTH)? "w":"-");
        printf((sb.st_mode & S_IXOTH)? "x":"-");
        cout<<"\t";
            
        struct passwd *pwd = getpwuid(sb.st_uid);
        if(!pwd) {
            perror("error on not getting pwid");
            exit(EXIT_FAILURE);
        } 
        cout<<pwd->pw_name;
        cout<<"\t";
        struct group *grp;
        grp = getgrgid(sb.st_gid);
        if (grp == NULL) {
            perror("error on not getting groupid");
            exit(EXIT_FAILURE);
        } else {
            cout<<grp->gr_name;
        }
        cout<<"\t";
        stringstream ss;
        ss << ctime(&sb.st_mtime);
        string ts = ss.str();
        int i=0,len=ts.size();
        string mod_time = "";
        while(i<len-1){
            mod_time = mod_time + ts[i];
            i++;
        }
        cout<<float(sb.st_size/1024)<<"kB";
        cout<<"\t\t"<<mod_time<<"\r\n";
    }

    void printFiles(vector<string> filess) {
        cout<<"\033c";
        for (int i = start; i < end; i++) {
            printAllInfo(filess[i]);
        }
    }
    void makeEmptystack(stack<string> A) {
        while(!A.empty()) {
            A.pop();
        }
    }

    /******** enable raw mode and handling **********/
    void enableRawMode() {
        struct termios oldt;
        if(tcgetattr(STDIN_FILENO, &oldt) == -1)
            die("tcgetattr");
        struct termios newt;
        newt = oldt;
        newt.c_iflag &= ~(ICRNL | IXON);
        newt.c_lflag &= ~(ICANON | ECHO); 
        if(tcsetattr(STDIN_FILENO, TCSANOW, &newt) == -1)
            die("tcsetattr");
        input = cin.get();
        if(tcsetattr(STDIN_FILENO, TCSANOW, &oldt) == -1)
            die("tcsetattr"); 
    }
    /******** enable raw mode and handling **********/
    void editorMoveCursor(int key) {
        switch (key) {
            case 'A':
                // cout<<"arror up pressed";
                if (pointTofile != start) {
                    printf("\033[1A");
                    pointTofile--;
                }
                break;
            case 'B':
                // cout<<"arror down pressed";
                if (pointTofile != end - 1) {
                    printf("\033[1B");
                    pointTofile++;
                }
                break;
            case 'k':
                // cout<<"k key pressed";
                if( pointTofile == start && start != 0) {
                    start--;
                    end--;
                    pointTofile--;
                    printFiles(allDirList[indexx - 1]);
                    for(int i=0;i!=end;i++)
                        printf("\033[1A");
                }
                break;
            case 'l':
                // cout<<"l key pressed";
                if( pointTofile == end -1 && end != filesize) {
                    start++;
                    end++;
                    pointTofile++;
                    printFiles(allDirList[indexx - 1]);
                    printf("\033[1A");
                }
                break;
        }
    }
    void changeFolder(string folderToMove) {
        int n = folderToMove.length();
        char char_array_folderToMove[n + 1];
        strcpy(char_array_folderToMove, folderToMove.c_str());
        if( chdir(char_array_folderToMove)<0 ){
            cout<<"error in changefolder"<<endl;
            exit(0);
        }
        
        DIR *directory = opendir(char_array_folderToMove);
        if (directory == NULL) {
            return;
        }
        dirent *fileInDir;
        vector <string> filesToDisplay;
        filesToDisplay.clear();
        filesToDisplay.push_back(".");
        filesToDisplay.push_back("..");
        while((fileInDir=readdir(directory))!=NULL){
            string fileName(fileInDir->d_name);
            if(fileName[0]!='.' )
                filesToDisplay.push_back(fileName);
        }
        pathToFiles[folderToMove]=indexx++;
        allDirList.push_back(filesToDisplay);
        filesToDisplay.clear();
        filesize = allDirList[indexx - 1].size();
        presentDirectory = folderToMove;
        pointTofile = start =0;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	    numOfFilesToShow = ws.ws_row - 10;
        if( filesize > numOfFilesToShow ) {
            end = numOfFilesToShow;
        } else {
            end = filesize;
        }
        printFiles(allDirList[indexx - 1]);
        cout << "\033[H";
    }
    void makeCommandVector() {
        commandVector.clear();
        string tempstring = commandString, parts;
        tempstring+=" ";
        for(int i=0;i<tempstring.size();i++) {
            if(tempstring[i] == ' ') {
                commandVector.push_back(parts);
                parts = "";
            } else {
                parts.push_back(commandString[i]);
            }
        }
    }
    void makeLocationNoslash(string &path, string appendString) { //for no slash
        string givenPath = commandVector[commandVector.size() - 1];
        if(givenPath[0] == '/') {
            if(givenPath[givenPath.size() - 1] == '/') {
                givenPath.pop_back();
            }
            path = givenPath + appendString;
        } else if (givenPath[0] == '.') {
            if(givenPath.length() == 1 || (givenPath.length() == 2 && givenPath[0]=='.' && givenPath[1]=='/')){
                path = presentDirectory + appendString;
            } else {
                if(givenPath[givenPath.size() - 1] == '/')
                    givenPath.pop_back();
                string toadd = givenPath.substr(2);
                path = presentDirectory + toadd + appendString;      
            }
        } else if (givenPath[0] == '~') {
            if(givenPath.length() == 1 || (givenPath.length() == 2 && givenPath[0]=='~' && givenPath[1]=='/')){
                path = realHOME + appendString;
            } else {
                if(givenPath[givenPath.size() - 1] == '/')
                    givenPath.pop_back();
                string toadd = givenPath.substr(2);
                path = realHOME + toadd + appendString;
            }
        }
    }
    void makeLocationOther(string &path, string appendString) { //for slash
        string givenPath = commandVector[commandVector.size() - 1];
        if(givenPath[0] == '/') {
            if(givenPath[givenPath.size() - 1] != '/') {
                givenPath+='/';
            }
            path = givenPath + appendString;
        } else if (givenPath[0] == '.') {
            if(givenPath.length() == 1 || (givenPath.length() == 2 && givenPath[0]=='.' && givenPath[1]=='/')){
                path = presentDirectory + appendString;
            } else {
                if(givenPath[givenPath.size() - 1] != '/')
                    givenPath+='/';
                string toadd = givenPath.substr(2);
                path = presentDirectory + toadd + appendString;      
            }
        } else if (givenPath[0] == '~') {
            if(givenPath.length() == 1 || (givenPath.length() == 2 && givenPath[0]=='~' && givenPath[1]=='/')){
                path = realHOME + appendString;
            } else {
                if(givenPath[givenPath.size() - 1] != '/')
                    givenPath+='/';
                string toadd = givenPath.substr(2);
                path = realHOME + toadd + appendString;
            }
        }
    }
    void makeLocation(string &path) {
        string givenPath = commandVector[commandVector.size() - 1];
        if(givenPath[0] == '/') {
            if(givenPath[givenPath.size() - 1] != '/') {
                givenPath+='/';
            }
            if(givenPath == presentDirectory) {
                noslash = 1;   
            }
            path = givenPath;
        } else if (givenPath[0] == '.') {
            if(givenPath.length() == 1 || (givenPath.length() == 2 && givenPath[0]=='.' && givenPath[1]=='/')){
                noslash = 1;
                path = presentDirectory;
            } else {
                if(givenPath[givenPath.size() - 1] != '/')
                    givenPath+='/';
                path = presentDirectory + givenPath.substr(1);      //given location end with "/"
            }
        } else if (givenPath[0] == '~' || (givenPath.length() == 2 && givenPath[0]=='~' && givenPath[1]=='/')) {
            if(givenPath.length() == 1){
                path = realHOME;
                noslash = 1;
            } else {
                if(givenPath[givenPath.size() - 1] != '/')
                    givenPath+='/';
                path = realHOME + givenPath.substr(1);
            }
        }
    }
    void createFile() {
        int sizee = commandVector.size();
        if(sizee == 3) {
            string newFileName = commandVector[1];
            string location;
            makeLocation(location);
            // cout<<"location = {"<<location<<endl;
            string parent = location;
            if(noslash == 0) {
                parent.pop_back();
                int sizeLocation = parent.size();
                while(sizeLocation) {
                    if(parent[sizeLocation - 1] == '/') {
                        break;
                    }
                    parent.pop_back();
                    sizeLocation--;
                }
                // cout<<"baahr aaya parent = {"<<parent;
            } else {
                parent = presentDirectory;
            }
            noslash = 0;
            location += commandVector[1];
            int n = location.length() + 1;
            char char_array_location[n + 1];
            strcpy(char_array_location, location.c_str());
            int status = open(char_array_location,O_RDONLY | O_CREAT| O_WRONLY);
            if(status == -1) {
                cout<<"error in making filr"<< location <<endl;
                return;
            }
            close(status);
            chmod(char_array_location,S_IRUSR|S_IWUSR);
            // cout<<"    parent = {"<<parent<<"}"<<"presentDirectory = {"<<presentDirectory<<"}";
            // int nikhil;
            // cin>>nikhil;
            if(presentDirectory == parent) {
                // refresh = 1;
                // cout<<"andar aayaparent = {"<<parent<<"} pwd = {"<<presentDirectory<<"}";
                allDirList[pathToFiles[parent]].push_back(newFileName);
                filesize = allDirList[pathToFiles[parent]].size();
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
                numOfFilesToShow = ws.ws_row - 10;
                start = pointTofile = 0;
                if( filesize > numOfFilesToShow ) {
                    end = numOfFilesToShow;
                } else {
                    end = filesize;
                } 
            }
        }
    }
    void createFolder() {
        int sizee = commandVector.size();
        if(sizee == 3) {
            string newFolderName = commandVector[1];
            string location;
            makeLocation(location);
            // cout<<"location = {"<<location<<endl;
            string parent = location;
            if(noslash == 0) {
                parent.pop_back();
                int sizeLocation = parent.size();
                while(sizeLocation) {
                    if(parent[sizeLocation - 1] == '/') {
                        break;
                    }
                    parent.pop_back();
                    sizeLocation--;
                }
                // cout<<"baahr aaya parent = {"<<parent;
            } else {
                parent = presentDirectory;
            }
            noslash = 0;
            location += commandVector[1];
            int n = location.length() + 1;
            char char_array_location[n + 1];
            strcpy(char_array_location, location.c_str());
            if(mkdir(char_array_location,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP |S_IROTH|S_IXUSR) == -1) {
                cout<<"error in making folder"<< location <<endl;
                return;
            }
            // cout<<"    parent = {"<<parent<<"}"<<"presentDirectory = {"<<presentDirectory<<"}";
            // int nikhil;
            // cin>>nikhil;
            if(presentDirectory == parent) {
                // refresh = 1;
                // cout<<"andar aayaparent = {"<<parent<<"} pwd = {"<<presentDirectory<<"}";
                allDirList[pathToFiles[parent]].push_back(newFolderName);
                filesize = allDirList[pathToFiles[parent]].size();
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
                numOfFilesToShow = ws.ws_row - 10;
                start = pointTofile = 0;
                if( filesize > numOfFilesToShow ) {
                    end = numOfFilesToShow;
                } else {
                    end = filesize;
                }
                
            }
        }
    }
    void moveEntities() {
        int numOfEntitytoMOVE = commandVector.size() - 2;
        for(int i=1; i < numOfEntitytoMOVE + 1;i++) 
        {
            string oldName = presentDirectory + commandVector[i];
            string newName ;

            makeLocationOther(newName, commandVector[i]);

            int n1 = oldName.length() + 1;
            char char_array_oldName[n1 + 1];
            strcpy(char_array_oldName, oldName.c_str());
            int n2 = newName.length() + 1;
            char char_array_newName[n2 + 1];
            strcpy(char_array_newName, newName.c_str());

            if(rename(char_array_oldName,char_array_newName)) {
                perror("Error in renaming entity");
            }
            // cout<<"oldname = {"<<oldName<<"} newname = {"<<newName<<"}"<<endl;
        }
        // int nikhil;
        // cin>>nikhil;
    }
    void copyFile(string source,string destination,string filee) {
        int n1 = source.length() + 1;
        char char_array_source[n1 + 1];
        strcpy(char_array_source, source.c_str());
        int n2 = destination.length() + 1;
        char char_array_destination[n2 + 1];
        strcpy(char_array_destination, destination.c_str());
        string line;
        struct stat oldpath,newpath;
        fstream fs;
        fs.open(char_array_source,ios::in | ios::binary);
        fstream fout;
        fout.open(char_array_destination,ios::out | ios::binary);
        if (fs && fout) {
            while(getline(fs,line)){
                fout << line << "\n";
            }
        } 
        if(stat(char_array_source,&oldpath)<0) {
            perror("error in stat");
        }
        if(stat(char_array_destination,&newpath)<0) {
            perror("error in stat");
        }
        int ownership = chown(char_array_destination,oldpath.st_uid,newpath.st_gid);
        if(ownership == 0) {
            ownership = chmod(char_array_destination,oldpath.st_mode);
            if(ownership == 0) {
                fs.close();
                fout.close();    // error
            }
        }
    }
    void copyFolder(string source,string destination,string filee) {
        int n1 = source.length() + 1;
        char char_array_source[n1 + 1];
        strcpy(char_array_source, source.c_str());
        DIR* dir=opendir(char_array_source);
        if(dir==NULL)
            return;
        struct  dirent* entity;
        entity=readdir(dir);
        int n2 = destination.length() + 1;
        char char_array_destination[n2 + 1];
        strcpy(char_array_destination, destination.c_str());
        mkdir(char_array_destination,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IXOTH);
        while(entity!=NULL){
        if(entityType(destination) == 2 && strcmp(entity->d_name,".")!=0 && strcmp(entity->d_name,"..")!=0){
                copyFolder(source,destination,entity->d_name);
            }
            else if(strcmp(entity->d_name,".")!=0 && strcmp(entity->d_name,"..")!=0){
                copyFile(source,destination,entity->d_name);
            }
            entity = readdir(dir);
        }
        closedir(dir);
    }
    void copyEntities() {
        int numOfEntitytoMOVE = commandVector.size() - 2;
        for(int i=1; i < numOfEntitytoMOVE + 1;i++) 
        {
            string oldName = presentDirectory + commandVector[i];
            string newName ;

            makeLocationOther(newName, commandVector[i]);

            if(entityType(oldName) == 2) {
                copyFile(oldName,newName,commandVector[i]);
            } else {
                copyFolder(oldName,newName,commandVector[i]);
            }
            // cout<<"oldname = {"<<oldName<<"} newname = {"<<newName<<"}"<<endl;
        }
        // int nikhil;
        // cin>>nikhil;
    }
    void renameEntity() {   
        if(commandVector.size() == 3) {
            string oldName = commandVector[1],newName = commandVector[2];

            int n1 = oldName.length() + 1;
            char char_array_oldName[n1 + 1];
            strcpy(char_array_oldName, oldName.c_str());
            int n2 = newName.length() + 1;
            char char_array_newName[n2 + 1];
            strcpy(char_array_newName, newName.c_str());

            if(rename(char_array_oldName,char_array_newName)) {
                perror("Error in renaming entity");
            }
            // refresh = 1;
        }
    }
    void deleteFile() {
        string location;
        makeLocationNoslash(location,"");
        int n = location.length() + 1;
        char char_array_location[n + 1];
        strcpy(char_array_location, location.c_str());
        // cout<<char_array_location;
        // int nik;
        // cin>>nik;
        if(remove(char_array_location) != 0) {
            perror("error in file deletion");
        }
    }
    void deleteFold(string location) {
        // string location;
        // makeLocationNoslash(location,"");
        int n = location.length() + 1;
        char char_array_location[n + 1];
        strcpy(char_array_location, location.c_str());
        // cout<<char_array_location;
        // int nik;
        // cin>>nik;
        if(remove(char_array_location) != 0) {
            perror("error in file deletion");
        }
    }
    void deleteFolderLoop(string location) {
        DIR *folder;
        dirent *fileInDir;
        int n = location.length() + 1;
        char char_array_location[n + 1];
        strcpy(char_array_location, location.c_str());
        // cout<<char_array_location;
        // int nik;
        // cin>>nik;
        folder=opendir(char_array_location);
        if(folder){
            while((fileInDir = readdir(folder))!=NULL){
                string entityName(fileInDir->d_name);
                if(entityName[0] != '.') {
                    if(entityType(location + entityName) == 2) {
                        // cout<<"{"<<presentDirectory + entityName<<"}";
                        // deleteFold(searchName);
                    } else if(entityType(location + entityName + '/') == 1) {
                        deleteFolderLoop(location + entityName + '/');
                        string tempp = location + entityName + '/';
                        int n = tempp.length() + 1;
                        char char_array_tempp[n + 1];
                        strcpy(char_array_tempp, tempp.c_str());
                        // rmdir(char_array_tempp);
                    }
                }
            }
            closedir(folder);
        }
    }

    void deleteFolder() {
        string location;
        makeLocationNoslash(location,"");
        deleteFolderLoop(location);
    }
    void gotoLocation() {
        string location;
        makeLocation(location);
        int n = location.length() + 1;
        char char_array_location[n + 1];
        strcpy(char_array_location, location.c_str());
        if(chdir(char_array_location)<0){
            cout<<"error in gotoLocation"<<endl;
            exit(0);
        }
        makeEmptystack(forward);
        backward.push(presentDirectory);
        changeFolder(location);
        presentDirectory = location;
    }
    int entityType(string path) {
        struct stat sb;
        int n = path.length() + 1;
        char char_array_path[n + 1];
        strcpy(char_array_path, path.c_str());
        // cout<<char_array_location;
        // int nik;
        // cin>>nik;
        if (stat(char_array_path, &sb) == -1) {
            // cout<<"{"<<char_array_path<<"}";
            perror("error on not getting stat");
            exit(EXIT_FAILURE);
        }
        int type;
        switch (sb.st_mode & S_IFMT) {
            case S_IFDIR:  
                    type = 1;
                    break;
            case S_IFREG:  
                    type = 2;
                    break;
        }
        // cout<<char_array_path<<"   type="<<type<<endl;
        // int nikhil;
        // cin>>nikhil;
        return type;
    }

    void searchLoop(string pwd,string searchName, bool &found) {
        DIR *folder;
        dirent *fileInDir;
        int n = pwd.length() + 1;
        char char_array_pwd[n + 1];
        strcpy(char_array_pwd, pwd.c_str());
        folder=opendir(char_array_pwd);
        if(folder){
            while((fileInDir = readdir(folder))!=NULL){
                string entityName(fileInDir->d_name);
                if(entityName[0] != '.') {
                    if(entityType(pwd + entityName) == 2) {
                        if(entityName == searchName) {
                            found = true;
                            return;
                        }
                    } else if(entityType(pwd + entityName + '/') == 1) {
                        // cout<<"{"<<entityName<<"}{"<<searchName<<"}\n";
                        if(entityName == searchName) {
                            found = true;
                            return;
                        }
                        searchLoop(pwd + entityName + '/', searchName,found);
                    }
                }
            }
            closedir(folder);
        }
    }
    void searchEntity() {
        if(commandVector.size() == 2) {
            searchLoop(presentDirectory,commandVector[1],specialSearch);
            // if(specialSearch) {
            //     cout<<"true";
            // } else {
            //     cout<<"false";
            // }
        }
        // int nikhil;
        // cin>>nikhil;
    }
    string executeCommandFunc() {
        makeCommandVector();
        // cout<<"command first location = "<< commandVector[0] <<" and ";
        if(commandVector[0] == "create_file") {
            createFile();       //command length = 3
            return "createFile";
        } else if(commandVector[0] == "create_dir") {
            createFolder();     //command length = 3
            return "createFolder";
        } else if(commandVector[0] == "move") {
            moveEntities();     //command length = n
            return "moveEntities";
        } else if(commandVector[0] == "copy") {
            copyEntities();     //command length = n
            return "copyEntities";
        } else if(commandVector[0] == "rename") {
            renameEntity();     //last m  //command length = 3
            return "renameEntity";
        } else if(commandVector[0] == "delete_file") {
            deleteFile();       //last m  //command length = 2
            return "deleteFile";
        } else if(commandVector[0] == "delete_dir") {
            deleteFolder();     //last m  //command length = 2
            return "deleteFolder";
        } else if(commandVector[0] == "goto") {
            gotoLocation();     //command length = 2 only for directory
            return "gotoLocation";
        } else if(commandVector[0] == "search") {
            searchEntity(); //command length = 2
            return "searchEntity";
        }
        // int nikhil;
        // cin>>nikhil;
        string toReturnDefaultString = "";
        return toReturnDefaultString;
    }
    void navigation() {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	    numOfFilesToShow = ws.ws_row - 10;
        if( filesize > numOfFilesToShow ) {
            end = numOfFilesToShow;
        } else {
            end = filesize;
        }
        printFiles(allDirList[indexx - 1]);
        cout << "\033[H";
        while(1){
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
            numOfFilesToShow = ws.ws_row - 10;
            if(typeOfMode) {
                NORMALMODE : //normal mode start location
                enableRawMode();
               
                if (input == 'q') {
                    // cout<<"q key pressed";
                    cout<<"\033c";
                    cout<<"File Explorer Successfully closed\r\n";
                    break;
                } else if ( (input == 'k') || (input == 'l')) {
                    editorMoveCursor(input);
                } else if (input == 'h') {
                    if( kabhiHomepresshuathakinahi == 0 ) {
                        kabhiHomepresshuathakinahi = 1;
                        if(presentDirectory != realHOME) {
                            makeEmptystack(forward);
                            backward.push(presentDirectory);
                            changeFolder(realHOME);
                            presentDirectory = realHOME;
                        }
                    } else {
                        if(presentDirectory != realHOME) {
                            makeEmptystack(forward);
                            backward.push(presentDirectory);
                            changeFolder(realHOME);
                            presentDirectory = realHOME;
                        }
                    }
                } else if (input == 13) {
                    // cout<<"Enter key pressed";
                    string path = presentDirectory + allDirList[indexx - 1][pointTofile];
                    int type = entityType(path);
                    if(type == 2) {
                        int n = (allDirList[indexx-1][pointTofile]).length() + 1;
                        char char_array_fileToOpen[n + 1];
                        strcpy(char_array_fileToOpen, (allDirList[indexx-1][pointTofile]).c_str());
                        int openFile = open(char_array_fileToOpen,O_WRONLY);
                        dup2(openFile,2);
                        close(openFile);
                        pid_t processID = fork();
                        if(processID == 0)
                        {
                            execlp("xdg-open","xdg-open",char_array_fileToOpen,NULL);
                            exit(0);
                        } 
                    } else {
                        path += '/';
                        int n = path.length() + 1;
                        char char_array_path[n + 1];
                        strcpy(char_array_path, path.c_str());
                        if( chdir(char_array_path)<0 ){
                            cout<<"error"<<endl;
                            exit(0);
                        }
                        backward.push(presentDirectory);
                        changeFolder(path);
                        cout << "\033[H";
                    }

                } else if (input == 127) {
                    // cout<<"backspace key pressed";
                    if(kabhiHomepresshuathakinahi == 0 && presentDirectory != realHOME) {

                        if(presentDirectory != home) {
                            makeEmptystack(forward);
                            string temp = presentDirectory;
                            temp.pop_back();
                            temp.pop_back();
                            int i=temp.length();
                            while(i>=0 && temp[i] != '/'){
                                i--;
                            }

                            string parent=presentDirectory.substr(0,i+1);
                            // cout<<presentDirectory<<"   "<<parent<<endl;
                            changeFolder(parent);
                            presentDirectory=parent;
                        }
                    } else {
                        if(presentDirectory != realHOME) {
                            makeEmptystack(forward);
                            string temp = presentDirectory;
                            temp.pop_back();
                            temp.pop_back();
                            int i=temp.length();
                            while(i>=0 && temp[i] != '/'){
                                i--;
                            }

                            string parent=presentDirectory.substr(0,i+1);
                            // cout<<presentDirectory<<"   "<<parent<<endl;
                            changeFolder(parent);
                            presentDirectory=parent;
                        }
                    }
                    
                } else if (input == 27) {
                    input = cin.get();
                    input = cin.get();
                    if( (input == 'A') || (input == 'B') ) {
                        editorMoveCursor(input);
                    } else if(input == 'C') {
                        // cout<<"arrow right pressed";
                        if( !forward.empty()) {
                            backward.push(presentDirectory);
                            string path = forward.top();
                            forward.pop();
                            changeFolder(path);
                            presentDirectory = path;
                        }
                    } else if(input == 'D') {
                        // cout<<"arrow left pressed";
                        if(!backward.empty()) {
                            forward.push(presentDirectory);
                            string path = backward.top();
                            backward.pop();
                            changeFolder(path);
                            presentDirectory = path;
                        }
                    }
                } else if (input == ':') {
                    COMMAND_MODE:
                    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	                int commandPosition = ws.ws_row - 3,cursorPos = pointTofile;
                    printf("\033[%dB",commandPosition - pointTofile);
                    cout<<":";
                    commandString = "";
                    typeOfMode = 2;  //COMMAND MODE
                    
                    while(1) {
                        enableRawMode();
                        switch(input) {
                            case 27:
                                cout << "\033[H";
                                typeOfMode = 1;
                                printFiles(allDirList[indexx - 1]);
                                cout << "\033[H";
                                goto NORMALMODE;
                                break;
                            case 127:
                                if(commandString.size()>0){
                                    commandString.pop_back();
                                    cout<<"\b \b";
                                }
                                break;
                            case 13:
                                // cout<< commandString;
                                commandExecuted = executeCommandFunc();
                                if(commandExecuted == "gotoLocation") {
                                    goto COMMAND_MODE;
                                }
                                if(refresh == 1) {
                                    // cout<<"refresh m aaya {"<<endl<<(indexx - 1)<<"}";
                                    refresh = pointTofile = 0;
                                    printFiles(allDirList[indexx - 1]);
                                    goto COMMAND_MODE;
                                } else {
                                    int backspaces = commandString.length();
                                    commandString = "";
                                    for(int i=0;i<backspaces;i++) {
                                        cout<<"\b \b";
                                    }
                                }
                                if(commandExecuted == "searchEntity") {
                                    if(specialSearch) {
                                        cout<<"true";
                                    }else {
                                        cout<<"false";
                                    }
                                    specialSearch = false;
                                }
                                commandString = "";
                                break;
                            default:
                                commandString.push_back(input);
                                cout<<input;
                                break;
                        }
                    }
                }
            }
        }
    }   

    void operateExplorer() {
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("getcwd() error");
        
        if(chdir(cwd)<0){
            cout<<"error"<<endl;
            exit(0);
        }

        home = homeWOSlash = cwd;
        char ch = '/';
        home.push_back(ch);
        presentDirectory = home;

        struct passwd *pw = getpwuid(getuid());
        char *tmp = pw->pw_dir;
        realHOME = realHOMEWOSlash = tmp;
        realHOME.push_back(ch);

        const char* dirname = ".";
        DIR* dir = opendir(dirname);
        if (dir == NULL) {
            return;
        }
        struct dirent* entity;
        entity = readdir(dir);
        vector <string> filesToDisplay;
        filesToDisplay.clear();
        filesToDisplay.push_back(".");
        filesToDisplay.push_back("..");

        while (entity != NULL) {
            string temp = entity->d_name;
            if(temp[0] != '.')
                filesToDisplay.push_back(temp);
            entity = readdir(dir);
        }
        filesize = filesToDisplay.size();
        if(pathToFiles.find(home) == pathToFiles.end()) {
            pathToFiles[home] = indexx;
            indexx++;
        }
        allDirList.push_back(filesToDisplay);
        filesToDisplay.clear();
        navigation();
    }
};


int main() {
  EXPLORER ob;
  ob.operateExplorer();
  return 0;
}