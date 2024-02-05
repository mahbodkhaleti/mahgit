#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <limits.h>
// #include <unistd.h>
#include <sys/stat.h>
// #include <sys/types.h>
#include <windows.h>
#include <tchar.h>
#include <wchar.h>
#include <time.h>
int checkrepo();
int run_init();
void findmah();
void searchAndDeleteFileOrDirectory(const char *searchPath);
void copyDirectory(const char *srcPath, const char *destPath, FILE *file);
// void copyDirectory(const char *srcPath, const char *destPath, const char *stagingFilePath);
void copyFile(const char *sourcePath, const char *destinationPath);
// void printTreeShape(const char *path, int level);
void printTreeShape(const char *path, int level, int depth);
void scanDirectory(const char *directory, FILE *file);
// void copyDirectory(const char *srcPath, const char *destPath);
void trave(const char *basePath, FILE *stagingFile);
int add(int argc, char *const argv[]);
char temppath[200];
char searchfile[100];
int commit_count = 0;
int branchcount = 0;
void print_command(int argc, char *const argv[])
{
    for (int i = 0; i < argc; i++)
    {
        fprintf(stdout, "%s ", argv[i]);
    }
    fprintf(stdout, "\n");
}
int checkfileordir(char *path)
{
    DWORD attributes = GetFileAttributesA(path);
    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        printf("'%s' does not exist\n", path);
        return 2;
    }
    if (attributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        // directory it is
        return 1;
    }
    else
    {
        // file it is
        return 0;
    }
}

unsigned long hash_function(char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
int run_init()
{
    if (checkrepo() == 1)
    {
        printf("repository already exists\n");
        return 1;
    }

    char currentDirectory[200];
    _fullpath(currentDirectory, ".", 200);
    const char *mahPath = strcat(currentDirectory, "\\.mahgit");

    if (!CreateDirectory(mahPath, NULL))
    {
        printf("Failed to create .mahgit directory\n");
        return 1;
    }

    SetFileAttributes(mahPath, FILE_ATTRIBUTE_HIDDEN);
    chdir(mahPath);

    if (mkdir("master") != 0)
    {
        printf("Failed to create master directory\n");
        return 1;
    }

    char *masterPath = strcat(currentDirectory, "\\.mahgit\\master");

    chdir(mahPath);

    if (mkdir("master\\staging") != 0)
    {
        printf("Failed to create staging directory\n");
        return 1;
    }

    char *stagingPath = strcat(currentDirectory, "\\.mahgit\\master\\staging");
    SetFileAttributes(stagingPath, FILE_ATTRIBUTE_HIDDEN);

    FILE *file = fopen("master\\commit.txt", "w");
    fprintf(file, "0");
    fclose(file);
    file = fopen("branch.txt", "w");
    fprintf(file, "master");
    fclose(file);
    // file = fopen("master\\tracks.txt", "w");
    // fclose(file);

    chdir(currentDirectory);

    return 0;
}
int checkrepo()
{
    printf("vared repo shodam\n");
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    int flag = 0;
    char currentDirectory[200];
    _fullpath(currentDirectory, ".", 200);
    int u = 0;
    for (int i = 0; i < 10; i++)
    {
        char path[200];
        strcpy(path, currentDirectory);
        strcat(path, "\\.mahgit");
        LPCSTR folder = path;
        DWORD attributes = GetFileAttributesA(folder);
        if (attributes != INVALID_FILE_ATTRIBUTES)
        {
            flag = 1;
            break;
        }
        // Move to the parent directory
        if (_chdir("..") != 0)
        {
            break; // Break the loop if unable to move to the parent directory
        }
        memset(path, 0, sizeof(path));
        _fullpath(currentDirectory, ".", 200); // Update the current directory path
    }
    if (flag == 1)
    {
        chdir(cwd);
        return 1;
    }
    else
    {
        chdir(cwd);
        return 0;
    }
}
void findmah()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    int flag = 0;
    char currentDirectory[200];
    _fullpath(currentDirectory, ".", 200);
    int u = 0;
    for (int i = 0; i < 10; i++)
    {
        char path[200];
        strcpy(path, currentDirectory);
        strcat(path, "\\.mahgit");
        LPCSTR folder = path;
        DWORD attributes = GetFileAttributesA(folder);
        if (attributes != INVALID_FILE_ATTRIBUTES)
        {
            strcpy(temppath, currentDirectory);
            strcat(temppath, "\\.mahgit");
            flag = 1;
            break;
        }
        // Move to the parent directory
        if (_chdir("..") != 0)
        {
            break; // Break the loop if unable to move to the parent directory
        }
        memset(path, 0, sizeof(path));
        _fullpath(currentDirectory, ".", 200); // Update the current directory path
    }
    chdir(cwd);
    return;
}
char *getFileName(char *path)
{
    char *fileName = strrchr(path, '\\');
    if (fileName == NULL)
    {
        fileName = strrchr(path, '/');
    }
    if (fileName != NULL)
    {
        fileName++; // Move to the actual filename
    }
    else
    {
        fileName = path; // No path separator, so the whole path is the filename
    }
    return fileName;
}
int add(int argc, char *const argv[])
{
    if (argc < 3)
    {
        printf("Invalid command");
        return 0;
    }
    else if (strcmp(argv[2], "-f") == 0)
    {
        printf("vared add shodam\n");
        findmah();
        printf("%s\n", temppath);
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        for (int i = 3; i < argc; i++)
        {
            // printf("vared shodam\n");
            int flag = 0;
            // char path[MAX_PATH];
            char fullpath[MAX_PATH];
            char *file = argv[i];
            GetFullPathName(file, MAX_PATH, fullpath, NULL);
            char *filepath = fullpath;
            chdir(temppath);
            FILE *branch = fopen("branch.txt", "r");
            char branchpath[100];
            fgets(branchpath, 100, branch);
            char *tempbranch = strcat(branchpath, "\\staging");
            char tempbranchpath[MAX_PATH];
            GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
            if (GetFileAttributes(tempbranchpath) == INVALID_FILE_ATTRIBUTES)
            {
                printf("Folder does not exist.\n");
                return 0;
            }
            printf("%s\n", branchpath);
            chdir(tempbranch);
            printf("%s\n", filepath);
            if (checkfileordir(filepath) == 0)
            {
                printf("file bood\n");
                FILE *file = fopen("staging.txt", "r");
                if (file == NULL)
                    printf("hello you there\n");
                char line[1000];
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    int length = strlen(line);

                    // remove '\n'
                    if (length > 0 && line[length - 1] == '\n')
                    {
                        line[length - 1] = '\0';
                    }

                    if (strcmp(filepath, line) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                fclose(file);
                file = fopen("staging.txt", "w");
                if (file == NULL)
                    return 1;
                // fprintf(file, "%s\n", filepath);
                // fclose(file);
                copyFile(filepath, tempbranchpath);
                char *mahbodrad = getFileName(fullpath);
                char tempnemidonam[MAX_PATH];
                GetFullPathName(mahbodrad, MAX_PATH, tempnemidonam, NULL);
                fprintf(file, "%s\n", tempnemidonam);
                fclose(file);
            }
            if (checkfileordir(filepath) == 1)
            {
                // printf("%s\n", temppath);
                printf("directory bood\n");
                FILE *file = fopen("staging.txt", "w");
                char path[MAX_PATH];
                strcpy(path, temppath);
                strcat(path, "\\staging.txt");
                // trave(filepath, file);
                // const char *stagingFilePath = "staging.txt";
                copyDirectory(filepath, tempbranchpath, file);
                memset(path, 0, sizeof(path));
            }
            chdir(cwd);
        }
        memset(temppath, 0, sizeof(temppath));

        return 0;
    }
    else if (checkrepo() == 0)
    {
        printf("tir khordam\n");
        return 0;
    }
    else if (strcmp(argv[2], "-n") == 0)
    {
        char currentDir[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, currentDir);
        printTreeShape(currentDir, 0, atoi(argv[3]));
    }
    else
    {
        printf("vared add shodam\n");
        findmah();
        // printf("%s\n", temppath);
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        for (int i = 2; i < argc; i++)
        {
            // printf("vared shodam\n");
            int flag = 0;
            // char path[MAX_PATH];
            char fullpath[MAX_PATH];
            char *file = argv[i];
            GetFullPathName(file, MAX_PATH, fullpath, NULL);
            char *filepath = fullpath;
            chdir(temppath);
            FILE *branch = fopen("branch.txt", "r");
            char branchpath[100];
            fgets(branchpath, 100, branch);
            char *tempbranch = strcat(branchpath, "\\staging");
            char tempbranchpath[MAX_PATH];
            GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
            printf("%s\n", tempbranchpath);
            if (GetFileAttributes(tempbranchpath) == INVALID_FILE_ATTRIBUTES)
            {
                printf("Folder does not exist.\n");
                return 0;
            }
            chdir(tempbranchpath);
            printf("%s\n", filepath);
            if (checkfileordir(filepath) == 0)
            {
                printf("file bood\n");
                FILE *file = fopen("staging.txt", "r");
                if (file == NULL)
                    printf("hello you there\n");
                char line[1000];
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    int length = strlen(line);

                    // remove '\n'
                    if (length > 0 && line[length - 1] == '\n')
                    {
                        line[length - 1] = '\0';
                    }

                    if (strcmp(filepath, line) == 0)
                    {
                        flag = 1;
                        break;
                    }
                }
                fclose(file);
                file = fopen("staging.txt", "w");
                if (file == NULL)
                    return 1;
                // fprintf(file, "%s\n", filepath);
                // fclose(file);
                copyFile(filepath, tempbranchpath);
                char *mahbodrad = getFileName(fullpath);
                char tempnemidonam[MAX_PATH];
                GetFullPathName(mahbodrad, MAX_PATH, tempnemidonam, NULL);
                fprintf(file, "%s\n", tempnemidonam);
                fclose(file);
            }
            if (checkfileordir(filepath) == 1)
            {
                // printf("%s\n", temppath);
                printf("directory bood\n");
                FILE *file = fopen("staging.txt", "w");
                char path[MAX_PATH];
                strcpy(path, temppath);
                strcat(path, "\\staging.txt");
                // trave(filepath, file);
                // const char *stagingFilePath = "staging.txt";
                copyDirectory(filepath, tempbranchpath, file);
                memset(path, 0, sizeof(path));
            }
            chdir(cwd);
        }
        memset(temppath, 0, sizeof(temppath));
    }

    return 0;
}
void copyFile(const char *sourcePath, const char *destinationPath)
{
    char command[100];
    sprintf(command, "copy %s %s", sourcePath, destinationPath);

    if (system(command) != 0)
    {
        printf("Failed to copy file.\n");
    }
    else
    {
        printf("File copied successfully.\n");
    }
}
void updateStagingFile(const char *basePath, const char *path, FILE *stagingFile)
{
    char fullpath[256];
    sprintf(fullpath, "%s\\%s", basePath, path);
    GetFullPathNameA(fullpath, 256, fullpath, NULL);
    fprintf(stagingFile, "%s\n", fullpath);
}

void trave(const char *basePath, FILE *stagingFile)
{
    char searchPath[256];
    WIN32_FIND_DATAA findData;

    sprintf(searchPath, "%s/*", basePath);
    HANDLE hFind = FindFirstFileA(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
            {
                char newPath[256];
                sprintf(newPath, "%s/%s", basePath, findData.cFileName);

                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    updateStagingFile(basePath, findData.cFileName, stagingFile);
                    trave(newPath, stagingFile);
                }
                else
                {
                    updateStagingFile(basePath, findData.cFileName, stagingFile);
                }
            }
        } while (FindNextFileA(hFind, &findData));

        FindClose(hFind);
    }
}

void copyDirectory(const char *srcPath, const char *destPath, FILE *file)
{
    char command[256];
    sprintf(command, "xcopy /E /I \"%s\" \"%s\"", srcPath, destPath);
    system(command);
    if (file != NULL)
    {
        trave(destPath, file);
        fclose(file);
    }
}
int config(int argc, char *const argv[])
{
    if (checkrepo() == 0)
    {
        printf("No projects were found\n");
    }
    else if (checkrepo() == 1)
    {
        if (argc != 5 && argc != 4)
        {
            printf("invalid command\n");
        }
        else if (argc == 4 && strcmp(argv[2], "user.name") == 0)
        {
            printf("Configed succesfully \n");
            findmah();
            strcat(temppath, "\\username.txt");
            FILE *file = fopen(temppath, "w");
            if (file == NULL)
            {
                printf("sadge\n");
            }
            fprintf(file, "%s\n", argv[3]);
            fclose(file);
            memset(temppath, 0, sizeof(temppath));
        }
        else if (argc == 4 && strcmp(argv[2], "user.email") == 0)
        {
            printf("Configed succesfully \n");
            findmah();
            strcat(temppath, "\\useremail.txt");
            FILE *file = fopen(temppath, "w");
            if (file == NULL)
            {
                printf("sadge\n");
            }
            fprintf(file, "%s\n", argv[3]);
            fclose(file);
            memset(temppath, 0, sizeof(temppath));
        }
        else if (argc == 5 && strcmp(argv[3], "user.name") == 0 && strcmp(argv[2], "-global") == 0)
        {
            FILE *file = fopen("c:\\Users\\ASuS\\Desktop\\project\\globalname.txt", "w");
            if (file == NULL)
            {
                printf("sadge\n");
            }
            fprintf(file, "%s\n", argv[4]);
            fclose(file);
        }
        else if (argc == 5 && strcmp(argv[3], "user.email") == 0 && strcmp(argv[2], "-global") == 0)
        {
            FILE *file = fopen("c:\\Users\\ASuS\\Desktop\\project\\globalemail.txt", "w");
            if (file == NULL)
            {
                printf("sadge\n");
            }
            fprintf(file, "%s\n", argv[4]);
            fclose(file);
        }
        else
        {
            printf("Inavlid command\n");
        }
    }
}
void printTreeShape(const char *path, int level, int depth)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char newPath[MAX_PATH];

    // Create the search wildcard pattern
    sprintf(newPath, "%s\\*", path);

    // Start searching for files and directories
    hFind = FindFirstFileA(newPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error: Cannot access directory\n");
        return;
    }

    do
    {
        // Skip current directory (.) and parent directory (..)
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
        {
            continue;
        }
        // Indentation for tree shape
        for (int i = 0; i < level; i++)
        {
            printf("|   ");
        }

        // Print directory or file name
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("|___[DIR] %s\n", findData.cFileName);
            if (level < depth)
            {
                // Recursively call the function for subdirectories
                sprintf(newPath, "%s\\%s", path, findData.cFileName);
                printTreeShape(newPath, level + 1, depth);
            }
        }
        else
        {
            printf("|___[FILE] %s\n", findData.cFileName);
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}
void removeLine(FILE *file, const char *lineToRemove)
{
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        perror("Error opening temporary file");
        return;
    }
    char buffer[1000];

    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strlen(buffer) - 1] = '\0';
        if (strcmp(buffer, lineToRemove) != 0)
        {
            // buffer[strlen(buffer)]='\n';
            printf("%s\n", buffer);
            fprintf(tempFile, "%s\n", buffer);
            // memset(buffer, 0, sizeof(buffer));
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("staging.txt");
    rename("temp.txt", "staging.txt");

    printf("Line removed and file updated successfully.\n");
}
void removeLinehook(FILE *file, const char *lineToRemove)
{
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        perror("Error opening temporary file");
        return;
    }
    char buffer[1000];

    while (fgets(buffer, sizeof(buffer), file))
    {
        buffer[strlen(buffer) - 1] = '\0';
        if (strcmp(buffer, lineToRemove) != 0)
        {
            // buffer[strlen(buffer)]='\n';
            printf("%s\n", buffer);
            fprintf(tempFile, "%s\n", buffer);
            // memset(buffer, 0, sizeof(buffer));
        }
    }

    fclose(file);
    fclose(tempFile);

    remove("staging.txt");
    rename("temp.txt", "hooks.txt");

    printf("Line removed and file updated successfully.\n");
}
int deleteFile(const char *filePath)
{
    if (remove(filePath) == 0)
    {
        printf("File %s deleted successfully.\n", filePath);
        return 0; // Success
    }
    else
    {
        perror("Error deleting the file");
        return 1; // Failure
    }
}
void reset(int argc, char *const argv[])
{
    if (argc < 3)
    {
        printf("invalid command\n");
        return;
    }
    else if (strcmp(argv[2], "-f") == 0)
    {
        findmah();
        printf("%s\n", temppath);
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        for (int i = 3; i < argc; i++)
        {
            int flag = 0;
            // char path[MAX_PATH];
            char fullpath[MAX_PATH];
            char *file = argv[i];
            GetFullPathName(file, MAX_PATH, fullpath, NULL);
            char *filepath = fullpath;
            chdir(temppath);
            FILE *branch = fopen("branch.txt", "r");
            char branchpath[100];
            fgets(branchpath, 100, branch);
            char *tempbranch = strcat(branchpath, "\\staging");
            char tempbranchpath[MAX_PATH];
            GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
            printf("%s\n", tempbranchpath);
            chdir(tempbranch);
            // deleteFile(filepath);
            // printf("%s\n", filepath);
            if (checkfileordir(filepath) == 0)
            {
                printf("file bood\n");
                FILE *file = fopen("staging.txt", "r");
                if (file == NULL)
                    printf("hello you there\n");
                char line[1000];
                int templine = 0;
                /*while (fgets(line, sizeof(line), file) != NULL)
                {
                    if (strcmp(filepath, line) == 0)
                    {

                        break;
                    }
                    templine++;
                }*/
                removeLine(file, filepath);
                deleteFile(argv[i]);
            }
            if (checkfileordir(filepath) == 1)
            {
                // printf("%s\n", temppath);
                printf("directory bood\n");
                FILE *file = fopen("staging.txt", "r");
                if (file == NULL)
                    printf("hello you there\n");
                char line[1000];
                int templine = 0;
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    if (strcmp(filepath, line) == 0)
                    {
                        // removeLine(file, templine);
                        break;
                    }
                    templine++;
                }
            }
            chdir(cwd);
        }
        memset(temppath, 0, sizeof(temppath));
    }
    else if (argc == 3 && strcmp(argv[2], "-undo") == 0)
    {
        findmah();
        printf("%s\n", temppath);
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        chdir(temppath);
        FILE *branch = fopen("branch.txt", "r");
        char branchpath[100];
        fgets(branchpath, 100, branch);
        char *tempbranch = strcat(branchpath, "\\staging");
        char tempbranchpath[MAX_PATH];
        GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
        printf("%s\n", tempbranchpath);
        chdir(tempbranch);
        fclose(branch);
        branch = fopen("staging.txt", "r");
        char tempfilepath[100];
        while (fgets(tempfilepath, 100, branch) != NULL)
        {
            tempfilepath[strlen(tempfilepath) - 1] = '\0';
            char tempmahbod[MAX_PATH];
            GetFullPathName(tempfilepath, MAX_PATH, tempmahbod, NULL);
            deleteFile(tempmahbod);
        }
    }
    else
    {
        // printf("reset dar hal ejra\n");
        findmah();
        printf("%s\n", temppath);
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        chdir(temppath);
        FILE *branch = fopen("branch.txt", "r");
        char branchpath[100];
        fgets(branchpath, 100, branch);
        char *tempbranch = strcat(branchpath, "\\staging");
        char tempbranchpath[MAX_PATH];
        GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
        printf("%s\n", tempbranchpath);
        chdir(tempbranch);
        for (int i = 2; i < argc; i++)
        {
            int flag = 0;

            // char path[MAX_PATH];
            char fullpath[MAX_PATH];
            char *file = argv[i];
            GetFullPathName(file, MAX_PATH, fullpath, NULL);
            char *filepath = fullpath;

            // deleteFile(filepath);
            //  printf("%s\n", filepath);
            if (checkfileordir(filepath) == 0)
            {
                printf("file bood\n");
                FILE *file1 = fopen("staging.txt", "r");
                if (file1 == NULL)
                    printf("hello you there\n");
                char line[1000];
                int templine = 1;
                /*while (fgets(line, sizeof(line), file1) != NULL)
                {
                    if (strcmp(filepath, line) == 0)
                    {
                        removeLine(file1, templine);
                        break;
                    }
                    templine++;
                }*/
                removeLine(file1, filepath);
                deleteFile(argv[i]);
            }
            if (checkfileordir(filepath) == 1)
            {
                // printf("%s\n", temppath);
                printf("directory bood\n");
                FILE *file = fopen("staging.txt", "r");
                if (file == NULL)
                    printf("hello you there\n");
                char line[1000];
                int templine = 1;
                while (fgets(line, sizeof(line), file) != NULL)
                {
                    if (strcmp(filepath, line) == 0)
                    {
                        // removeLine(file, templine);
                        break;
                    }
                    templine++;
                }
                deleteFile(filepath);
            }
            chdir(cwd);
        }
        memset(temppath, 0, sizeof(temppath));
    }
}
void deleteFileOrDirectory(const char *path)
{
    if (RemoveDirectoryA(path) == 0)
    {
        if (GetLastError() == ERROR_DIR_NOT_EMPTY)
        {
            // Directory is not empty, so iteratively delete its contents
            char completeSearchPath[MAX_PATH];
            sprintf(completeSearchPath, "%s\\*", path);

            WIN32_FIND_DATAA findData;
            HANDLE findHandle = FindFirstFileA(completeSearchPath, &findData);

            if (findHandle != INVALID_HANDLE_VALUE)
            {
                do
                {
                    if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
                    {
                        char completeFilePath[MAX_PATH];
                        sprintf(completeFilePath, "%s\\%s", path, findData.cFileName);

                        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                        {
                            // Recursive call to delete subdirectory
                            deleteFileOrDirectory(completeFilePath);
                        }
                        else
                        {
                            // Delete file
                            if (DeleteFileA(completeFilePath) == 0)
                            {
                                printf("Failed to delete file: %s\n", completeFilePath);
                            }
                        }
                    }
                } while (FindNextFileA(findHandle, &findData));

                FindClose(findHandle);
            }
            // Retry deleting the empty directory after deleting its contents
            if (RemoveDirectoryA(path) == 0)
            {
                printf("Failed to delete directory: %s\n", path);
            }
        }
        else
        {
            printf("Failed to delete directory: %s\n", path);
        }
    }
}
void searchForFile(const char *fileName)
{
    char currentDir[MAX_PATH];
    char parentDir[MAX_PATH];

    // Get the current directory
    GetCurrentDirectory(MAX_PATH, currentDir);

    // Search for the file in current directory and its parent directories until root
    while (strcmp(currentDir, parentDir) != 0)
    {
        // Check if the file exists in the current directory
        char filePath[MAX_PATH];
        sprintf(filePath, "%s\\%s", currentDir, fileName);
        FILE *file = fopen(filePath, "r");

        if (file != NULL)
        {
            strcpy(searchfile, filePath);
            printf("Found file at path: %s\n", filePath);
            fclose(file);
            break;
        }

        // Move to the parent directory
        strcpy(parentDir, currentDir);
        SetCurrentDirectory("..");
        GetCurrentDirectory(MAX_PATH, currentDir);
    }
}
void compareFileModificationTimes(const char *file1, const char *file2, FILE *file, char *const argv[], int argc, char branch[], int value)
{
    printf("vaght nadarim\n");
    struct stat attr1, attr2;

    // Get the file attributes
    stat(file1, &attr1);
    stat(file2, &attr2);

    // Compare the file modification times
    if (attr1.st_mtime > attr2.st_mtime)
    {
        FILE *fp = fopen(file1, "r");
        if (fp == NULL)
        {
            fprintf(stderr, "Error: Failed to open %s\n", file1);
        }

        char line[100];
        fgets(line, sizeof(line), fp);
        printf("%s was modified later than %s\n", file1, file2);
        fprintf(file, "%s\n", line);
        for (int i = 3; i < argc; i++)
        {
            fprintf(file, "%s ", argv[i]);
        }
        fprintf(file, "\n");
        fprintf(file, "%s\n", branch);
        fprintf(file, "%d\n", value);
        time_t currentTime;
        time(&currentTime);

        // Convert the time to a string
        char *timeString = ctime(&currentTime);

        // Write the time to the file
        fprintf(file, "%s\n", timeString);
    }
    else if (attr1.st_mtime < attr2.st_mtime)
    {
        FILE *fp = fopen(file2, "r");
        if (fp == NULL)
        {
            fprintf(stderr, "Error: Failed to open %s\n", file1);
        }

        char line[100];
        fgets(line, sizeof(line), fp);
        printf("%s was modified later than %s\n", file2, file1);
        fprintf(file, "%s\n", line);
        for (int i = 3; i < argc; i++)
        {
            fprintf(file, "%s ", argv[i]);
        }
        fprintf(file, "\n");
        fprintf(file, "%s\n", branch);
        fprintf(file, "%d\n", value);
        time_t currentTime;
        time(&currentTime);

        // Convert the time to a string
        char *timeString = ctime(&currentTime);

        // Write the time to the file
        fprintf(file, "%s\n", timeString);
    }
    else
    {
        printf("The modification times of %s and %s are the same\n", file1, file2);
    }
}
void createFolderAndMoveFiles(const char *folderName)
{
    // Create a new folder
    CreateDirectory(folderName, NULL);

    // Move all files to the new folder
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*.*", &findFileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                char moveCommand[300];
                sprintf(moveCommand, "move \"%s\" \"%s\\%s\"", findFileData.cFileName, folderName, findFileData.cFileName);
                system(moveCommand);
            }
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
}
void copyFilesToParentDirectory(const char *directory)
{
    char command[256];
    sprintf(command, "cmd /c xcopy \"%s\\*.*\" \"%s\\..\\\"", directory, directory);
    system(command);
}
void copyFiles(const char *srcPath, const char *destPath)
{
    char command[1000];

    // Create command to copy files using xcopy command
    sprintf(command, "xcopy /S /Y \"%s\\*.*\" \"%s\"", srcPath, destPath);

    // Execute the command using system function
    system(command);
}
void createFolderAndCopyFiles(const char *folderName)
{
    // Create the folder
    if (CreateDirectory(folderName, NULL) ||
        GetLastError() == ERROR_ALREADY_EXISTS)
    {
        printf("Successfully created folder: %s\n", folderName);

        // Get the current directory
        char currentDir[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, currentDir);

        // Iterate through files in the current directory
        DIR *dp;
        struct dirent *entry;
        dp = opendir(currentDir);
        if (dp != NULL)
        {
            while ((entry = readdir(dp)) != NULL)
            {
                // Skip directories and system files
                if (entry->d_type != DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    // Create the source and destination file paths
                    char srcPath[MAX_PATH];
                    char destPath[MAX_PATH];
                    sprintf(srcPath, "%s\\%s", currentDir, entry->d_name);
                    sprintf(destPath, "%s\\%s\\%s", currentDir, folderName, entry->d_name);

                    // Copy the file
                    if (!CopyFile(srcPath, destPath, FALSE))
                        printf("Failed to copy file: %s\n", entry->d_name);
                    else
                        printf("Successfully copied file: %s\n", entry->d_name);
                }
            }
            closedir(dp);
        }
        else
        {
            printf("Failed to open the directory.\n");
        }
    }
    else
    {
        printf("Failed to create the folder.\n");
    }
}
void moveFiles(const char *srcPath, const char *destPath)
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char searchPath[MAX_PATH], srcFilePath[MAX_PATH], destFilePath[MAX_PATH];

    // Create search path for source directory
    snprintf(searchPath, sizeof(searchPath), "%s/*", srcPath);

    // Find the first file in the source directory
    hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Could not find files in the source directory.\n");
        return;
    }

    do
    {
        // Ignore directories
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // Create full source and destination file paths
            snprintf(srcFilePath, sizeof(srcFilePath), "%s/%s", srcPath, findData.cFileName);
            snprintf(destFilePath, sizeof(destFilePath), "%s/%s", destPath, findData.cFileName);

            // Move or copy the file
            if (!CopyFileA(srcFilePath, destFilePath, FALSE))
            {
                printf("Failed to move or copy file: %s\n", srcFilePath);
            }
            else
            {
                printf("Moved or copied file: %s\n", srcFilePath);
                if (!DeleteFileA(srcFilePath))
                {
                    printf("Failed to delete file: %s\n", srcFilePath);
                }
                else
                {
                    printf("Deleted file: %s\n", srcFilePath);
                }
            }
        }
    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
}
void copyFolderToParentDir(const char *folderName)
{
    char currentDir[MAX_PATH];
    char parentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);

    if (strlen(currentDir) == 3)
    {
        printf("Cannot copy folder from root directory.\n");
        return;
    }

    // Get parent directory
    strncpy(parentDir, currentDir, MAX_PATH);
    char *lastBackslash = strrchr(parentDir, '\\');
    *lastBackslash = '\0';

    char destinationDir[MAX_PATH];
    sprintf(destinationDir, "%s\\%s", parentDir, folderName);

    // Create destination folder
    if (!CreateDirectory(destinationDir, NULL))
    {
        printf("Failed to create destination folder.\n");
        return;
    }

    // Copy all files and subfolders
    char sourceDir[MAX_PATH];
    sprintf(sourceDir, "%s\\%s", currentDir, folderName);
    char command[MAX_PATH * 2 + 20];
    sprintf(command, "xcopy /s /e \"%s\" \"%s\"", sourceDir, destinationDir);

    if (system(command) == 0)
    {
        printf("Folder has been successfully copied to the parent directory.\n");
    }
    else
    {
        printf("Failed to copy folder.\n");
    }
}
void replaceLineWithArray(FILE *file, const char *searchWord, const char *replaceArray, int flag)
{
    // Temporary file to store the modified contents
    FILE *tmpFile = tmpfile();
    if (tmpFile == NULL)
    {
        printf("Failed to create temporary file.");
        return;
    }

    char line[1000];
    int wordFound = 0;

    while (fgets(line, sizeof(line), file))
    {
        // Check if the line contains the search word
        if (strstr(line, searchWord) != NULL)
        {
            wordFound = 1;
            if (flag == 1)
            {
                // Replace the line with the new array content
                fputs(replaceArray, tmpFile);
                fputc('\n', tmpFile);
            }
        }
        else
        {
            fputs(line, tmpFile);
        }
    }

    if (!wordFound)
    {
        printf("Shortcut not found\n");
    }

    // Copy the modified contents from temporary file back to the original file
    rewind(tmpFile);
    rewind(file);

    int ch;
    while ((ch = fgetc(tmpFile)) != EOF)
    {
        fputc(ch, file);
    }

    fclose(tmpFile);
}
int commitkon(int argc, char *const argv[])
{
    if (argc < 4)
    {
        printf("invalid command\n");
    }
    else if (strlen(argv[3]) > 72)
    {
        printf("Message limit\n");
        return 0;
    }
    else if (checkrepo() == 0)
    {
        printf("Please create a repo\n");
    }
    else if (strcmp(argv[1], "set") == 0)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 1;
        chdir(temppath);
        FILE *shortcut = fopen("shortcut.txt", "a");
        fprintf(shortcut, "%s:%s\n", argv[5], argv[3]);
        fclose(shortcut);
    }
    else if (strcmp(argv[1], "replace") == 0)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 0;
        chdir(temppath);
        FILE *shortcut = fopen("shortcut.txt", "r");
        char line[100];
        sprintf(line, "%s:%s", argv[5], argv[3]);
        replaceLineWithArray(shortcut, argv[5], line, 1);
    }
    else if (strcmp(argv[1], "remove") == 0)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 0;
        chdir(temppath);
        FILE *shortcut = fopen("shortcut.txt", "r");
        char line[100];
        sprintf(line, "%s:%s", argv[5], argv[3]);
        replaceLineWithArray(shortcut, argv[5], line, 0);
        fclose(shortcut);
    }
    else if (strcmp(argv[2], "-s") == 0)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag2 = 0;
        chdir(temppath);
        FILE *shortcut = fopen("shortcut.txt", "r");
        char line[100];
        char message[100];
        while (fgets(line, 100, shortcut) != NULL)
        {
            if (strstr(line, argv[3]) != NULL)
            {
                int mahal;
                for (int i = 0; i < strlen(line) - 1; i++)
                {
                    if (line[i] == ':')
                    {
                        mahal = i + 1;
                        break;
                    }
                }
                int index = 0;
                for (int i = mahal; i < strlen(line) - 1; i++)
                {
                    message[index] = line[i];
                }
                flag2 = 1;
                break;
            }
        }
        if (flag2 == 0)
        {
            printf("Shortcut was not found\n");
            return 0;
        }
        fclose(shortcut);
        int flag = 1;
        char branchname[100];
        FILE *branch = fopen("branch.txt", "r");
        char branchpath[100];
        fgets(branchpath, 100, branch);
        strcpy(branchname, branchpath);
        fclose(branch);
        branch = fopen("username.txt", "r");
        if (branch != NULL)
        {
            flag = 0;
        }
        char name[100];
        if (flag == 1)
        {
            fgets(name, 100, branch);
        }
        fclose(branch);
        char mahbod[MAX_PATH];
        GetFullPathName(branchpath, MAX_PATH, mahbod, NULL);
        chdir(mahbod);
        FILE *file = fopen("commit.txt", "r+");
        if (file == NULL)
        {
            perror("Error opening the file");
        }
        int value;
        fscanf(file, "%d", &value); // Read the integer from the file
        fseek(file, 0, SEEK_SET);   // Reset file position to the beginning
        fprintf(file, "%d", value + 1);
        fclose(file);
        chdir(temppath);
        char *tempbranch = strcat(branchpath, "\\staging");
        char tempbranchpath[MAX_PATH];
        GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
        printf("%s\n", tempbranchpath);
        chdir(tempbranch);
        remove("staging.txt");
        remove("hooks.txt");
        chdir("..");
        char foldername[100];
        sprintf(foldername, "%s%d", branchname, value);
        createFolderAndCopyFiles(foldername);
        moveFiles(tempbranchpath, foldername);
        chdir(foldername);
        const char *file2 = "c:\\Users\\ASuS\\Desktop\\project\\globalname.txt";
        branch = fopen("config.txt", "w");
        if (flag == 0)
        {
            printf("using global username\n");
            FILE *fp = fopen(file2, "r");
            if (fp == NULL)
            {
                fprintf(stderr, "Error: Failed to open %s\n", file2);
            }
            char line[100];
            fgets(line, sizeof(line), fp);
            fprintf(branch, "%s", line);
            fprintf(file, "%s ", message);

            fprintf(branch, "\n");
            fprintf(branch, "%s%d\n", branchname, value);
            time_t currentTime;
            time(&currentTime);

            // Convert the time to a string
            char *timeString = ctime(&currentTime);

            // Write the time to the file
            fprintf(branch, "%s\n", timeString);
        }
        if (flag == 1)
        {

            fprintf(branch, "%s\n", name);

            fprintf(file, "%s ", message);

            fprintf(branch, "\n");
            fprintf(branch, "%s%d\n", branchname, value);
            time_t currentTime;
            time(&currentTime);

            // Convert the time to a string
            char *timeString = ctime(&currentTime);

            // Write the time to the file
            fprintf(branch, "%s\n", timeString);
        }
        fclose(branch);
        chdir("..");
        copyFolderToParentDir(foldername);
        chdir("..");
        branch = fopen("commits.txt", "a");
        fprintf(branch, "%s\n", foldername);
        fclose(branch);
        branch = fopen("lastcommit.txt", "w");
        fprintf(branch, "%s", foldername);
        fclose(branch);
    }
    else
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 1;
        chdir(temppath);
        char branchname[100];
        FILE *branch = fopen("branch.txt", "r");
        char branchpath[100];
        fgets(branchpath, 100, branch);
        strcpy(branchname, branchpath);
        fclose(branch);
        branch = fopen("username.txt", "r");
        if (branch != NULL)
        {
            flag = 0;
        }
        char name[100];
        if (flag == 1)
        {
            fgets(name, 100, branch);
        }
        fclose(branch);
        char mahbod[MAX_PATH];
        GetFullPathName(branchpath, MAX_PATH, mahbod, NULL);
        chdir(mahbod);
        FILE *file = fopen("commit.txt", "r+");
        if (file == NULL)
        {
            perror("Error opening the file");
        }
        int value;
        fscanf(file, "%d", &value); // Read the integer from the file
        fseek(file, 0, SEEK_SET);   // Reset file position to the beginning
        fprintf(file, "%d", value + 1);
        fclose(file);
        chdir(temppath);
        char *tempbranch = strcat(branchpath, "\\staging");
        char tempbranchpath[MAX_PATH];
        GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
        printf("%s\n", tempbranchpath);
        chdir(tempbranch);
        remove("staging.txt");
        remove("hooks.txt");
        chdir("..");
        char foldername[100];
        sprintf(foldername, "%s%d", branchname, value);
        createFolderAndCopyFiles(foldername);
        moveFiles(tempbranchpath, foldername);
        chdir(foldername);
        const char *file2 = "c:\\Users\\ASuS\\Desktop\\project\\globalname.txt";
        branch = fopen("config.txt", "w");
        if (flag == 0)
        {
            printf("using global username\n");
            FILE *fp = fopen(file2, "r");
            if (fp == NULL)
            {
                fprintf(stderr, "Error: Failed to open %s\n", file2);
            }
            char line[100];
            fgets(line, sizeof(line), fp);
            fprintf(branch, "%s", line);
            for (int i = 3; i < argc; i++)
            {
                fprintf(file, "%s ", argv[i]);
            }
            fprintf(branch, "\n");
            fprintf(branch, "%s%d\n", branchname, value);
            time_t currentTime;
            time(&currentTime);

            // Convert the time to a string
            char *timeString = ctime(&currentTime);

            // Write the time to the file
            fprintf(branch, "%s\n", timeString);
        }
        if (flag == 1)
        {

            fprintf(branch, "%s\n", name);
            for (int i = 3; i < argc; i++)
            {
                fprintf(file, "%s ", argv[i]);
            }
            fprintf(branch, "\n");
            fprintf(branch, "%s%d\n", branchname, value);
            time_t currentTime;
            time(&currentTime);

            // Convert the time to a string
            char *timeString = ctime(&currentTime);

            // Write the time to the file
            fprintf(branch, "%s\n", timeString);
        }
        fclose(branch);
        chdir("..");
        copyFolderToParentDir(foldername);
        chdir("..");
        branch = fopen("commits.txt", "a");
        fprintf(branch, "%s\n", foldername);
        fclose(branch);
        branch = fopen("lastcommit.txt", "w");
        fprintf(branch, "%s", foldername);
        fclose(branch);
    }
}
void compareDirectories(const char *sourceDir, const char *pathDir, const char *otherDir)
{
    DIR *dir;
    struct dirent *entry;

    // Open the path directory
    if ((dir = opendir(pathDir)) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_REG)
            {
                int flag = 0;
                // Check if the file is in the source directory
                char sourceFilePath[256];
                sprintf(sourceFilePath, "%s/%s", sourceDir, entry->d_name);
                if (access(sourceFilePath, F_OK) != -1)
                {
                    // File exists in the source directory
                    // Check if it has been modified
                    struct stat sourceStat, pathStat;
                    stat(sourceFilePath, &sourceStat);
                    char pathFilePath[256];
                    sprintf(pathFilePath, "%s/%s", pathDir, entry->d_name);
                    stat(pathFilePath, &pathStat);
                    if (difftime(sourceStat.st_mtime, pathStat.st_mtime) > 0)
                    {
                        printf("M");
                        flag = 1; // File has been modified
                    }
                }
                else
                {
                    printf("A");
                    flag = 1; // File is in the path directory but not in the source directory
                }

                // Check if the file exists in another directory
                if (flag == 1)
                {
                    char otherFilePath[256];
                    sprintf(otherFilePath, "%s/%s", otherDir, entry->d_name);
                    if (access(otherFilePath, F_OK) != -1)
                    {
                        printf("+ %s\n", entry->d_name); // File exists in another directory
                    }
                    else
                    {
                        printf("- %s\n", entry->d_name);
                    }
                }
            }
        }
        closedir(dir);
    }

    // Open the source directory to check for files not in the path directory
    if ((dir = opendir(sourceDir)) != NULL)
    {
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_REG)
            {
                int flag = 0;
                char pathFilePath[256];
                sprintf(pathFilePath, "%s/%s", pathDir, entry->d_name);
                if (access(pathFilePath, F_OK) == -1)
                {
                    flag = 1;
                    printf("D"); // File is in the source directory but not in the path directory
                }
                if (flag == 1)
                {
                    char otherFilePath[256];
                    sprintf(otherFilePath, "%s/%s", otherDir, entry->d_name);
                    if (access(otherFilePath, F_OK) != -1)
                    {
                        printf("+ %s\n", entry->d_name); // File exists in another directory
                    }
                    else
                    {
                        printf("- %s\n", entry->d_name);
                    }
                }
            }
        }
        closedir(dir);
    }
}

int status(int argc, char *const argv[])
{
    if (checkrepo() == 0)
    {
        printf("please create a repository\n");
        return 0;
    }
    findmah();
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    int flag = 0;
    chdir(temppath);
    char branchname[100];
    FILE *branch = fopen("branch.txt", "r");
    char branchpath[100];
    fgets(branchpath, 100, branch);
    strcpy(branchname, branchpath);
    fclose(branch);
    char mahbod[MAX_PATH];
    GetFullPathName(branchpath, MAX_PATH, mahbod, NULL);
    chdir(mahbod);
    FILE *file = fopen("commit.txt", "r+");
    if (file == NULL)
    {
        perror("Error opening the file");
    }
    int value;
    fscanf(file, "%d", &value);
    value -= 1;
    char akharincommit[100];
    sprintf(akharincommit, "%s%d", branchname, value);
    char pathcommit[100];
    GetFullPathName(akharincommit, MAX_PATH, pathcommit, NULL);
    char staging[100];
    sprintf(staging, "staging");
    char stagingPath[100];
    GetFullPathName(staging, MAX_PATH, stagingPath, NULL);
    compareDirectories(pathcommit, mahbod, stagingPath);
}
void copyFilesforbranch(const char *sourceDir, const char *destinationDir)
{
    DIR *dir;
    struct dirent *entry;

    // Open source directory
    if ((dir = opendir(sourceDir)) == NULL)
    {
        fprintf(stderr, "Unable to open source directory: %s\n", sourceDir);
        return;
    }

    // Create destination directory if it doesn't exist
    // if (mkdir(destinationDir) != 0)
    // {
    //     fprintf(stderr, "Unable to create destination directory: %s\n", destinationDir);
    //     closedir(dir);
    //     return;
    // }

    // Read all files in the source directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Exclude '.' and '..' directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Create file paths
        char sourcePath[256];
        char destinationPath[256];
        sprintf(sourcePath, "%s/%s", sourceDir, entry->d_name);
        sprintf(destinationPath, "%s/%s", destinationDir, entry->d_name);

        // Copy file from source to destination
        FILE *sourceFile = fopen(sourcePath, "rb");
        FILE *destinationFile = fopen(destinationPath, "wb");
        if (sourceFile == NULL || destinationFile == NULL)
        {
            fprintf(stderr, "Unable to copy file: %s\n", entry->d_name);
            continue;
        }

        int ch;
        while ((ch = fgetc(sourceFile)) != EOF)
            fputc(ch, destinationFile);

        fclose(sourceFile);
        fclose(destinationFile);
    }

    closedir(dir);
}
int branch(int argc, char *const argv[])
{
    if (checkrepo() == 0)
    {
        printf("please create a repository\n");
        return 0;
    }
    findmah();
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    int flag = 0;
    chdir(temppath);
    char branchname[100];
    FILE *branch = fopen("branch.txt", "r");
    char branchpath[100];
    fgets(branchpath, 100, branch);
    strcpy(branchname, branchpath);
    fclose(branch);
    // char mahbod[MAX_PATH];
    // GetFullPathName(branchpath, MAX_PATH, mahbod, NULL);
    // chdir(mahbod);
    if (argc == 3)
    {
        char line[100];
        FILE *file = fopen("lastcommit.txt", "r");
        fgets(line, 100, file);
        fclose(file);
        char tempbranchpath[MAX_PATH];
        GetFullPathName(line, MAX_PATH, tempbranchpath, NULL);
        mkdir(argv[2]);
        char tempcopy[MAX_PATH];
        GetFullPathName(argv[2], MAX_PATH, tempcopy, NULL);
        copyFilesforbranch(tempbranchpath, tempcopy);
        chdir(tempcopy);
        mkdir("staging");
    }
    if (argc == 2)
    {
        printTreeShape(temppath, 2, 1);
    }
}
void searchDirectoriesForConfigFile(char targetDate[])
{
    WIN32_FIND_DATA findData;
    HANDLE hFind;
    char basePath[MAX_PATH];
    bool isConfigFound = false;

    GetCurrentDirectory(MAX_PATH, basePath);
    strcat(basePath, "\\*");

    hFind = FindFirstFile(basePath, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("Error finding the first file/directory!\n");
        return;
    }

    while (1)
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
            {
                char subDirectory[MAX_PATH];
                sprintf(subDirectory, "%s\\%s", basePath, findData.cFileName);

                FILE *configFile = fopen("config.txt", "r");
                if (configFile != NULL)
                {
                    // Check the modification date of config.txt
                    FILETIME configModifiedTime;
                    GetFileTime((HANDLE)_get_osfhandle(_fileno(configFile)), NULL, NULL, &configModifiedTime);

                    SYSTEMTIME configModifiedSt;
                    FileTimeToSystemTime(&configModifiedTime, &configModifiedSt);

                    SYSTEMTIME targetDateSt;
                    sscanf(targetDate, "%d-%d-%d", &targetDateSt.wYear, &targetDateSt.wMonth, &targetDateSt.wDay);
                    printf("%d-%d-%d\n", targetDateSt.wYear, targetDateSt.wMonth, targetDateSt.wDay);
                    FILETIME targetDateFt;
                    SystemTimeToFileTime(&targetDateSt, &targetDateFt);

                    if (CompareFileTime(&configModifiedTime, &targetDateFt) == -1)
                    {
                        isConfigFound = true;

                        printf("Directory: %s", findData.cFileName);
                        printf("config.txt:\n");

                        char ch;
                        while ((ch = fgetc(configFile)) != EOF)
                        {
                            putchar(ch);
                        }
                        printf("\n");

                        printf("--------------------\n");
                    }
                    fclose(configFile);
                }

                // Recursively search subdirectories
                SetCurrentDirectory(findData.cFileName);
                searchDirectoriesForConfigFile(targetDate);
                SetCurrentDirectory("..");
            }
        }

        if (!FindNextFile(hFind, &findData))
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                break;
            }
            else
            {
                printf("Error finding the next file/directory!\n");
                break;
            }
        }
    }

    FindClose(hFind);

    if (!isConfigFound)
    {
        printf("No config.txt file found modified before the provided date.\n");
    }
}
int logkon(int argc, char *const argv[])
{
    if (checkrepo() == 0)
    {
        printf("please create a repository\n");
        return 0;
    }
    findmah();
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    int flag = 0;
    chdir(temppath);
    if (argc == 2)
    {
        FILE *file = fopen("commits.txt", "r");
        if (file == NULL)
        {
            printf("No commits have been made\n");
            return 0;
        }

        fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file

        long int position = ftell(file); // Get the current position of the file pointer
        char line[100];
        while (position)
        {
            fseek(file, --position, SEEK_SET); // Move the file pointer backward by one position
            int c = fgetc(file);               // Read the character
            if (c == '\n' || position == 0)
            {
                if (position != 0)
                {
                    fseek(file, 1, SEEK_CUR); // Move the file pointer one position forward to read the newline character
                }
                if (position == 0)
                {
                    fseek(file, 0, SEEK_SET); // If it's the first line, move the file pointer to the beginning of the file
                }
                if (fgets(line, 100, file) != NULL)
                {
                    // Process the line as needed
                    char tempbranchpath[MAX_PATH];
                    GetFullPathName(line, MAX_PATH, tempbranchpath, NULL);
                    tempbranchpath[strlen(tempbranchpath) - 1] = '\0';
                    printf("%s", line);
                    chdir(tempbranchpath);
                    FILE *config = fopen("config.txt", "r");
                    char templine[100];
                    while (fgets(templine, 100, config) != NULL)
                    {
                        printf("%s", templine);
                    }
                    fclose(config); // Close the "config.txt" file after reading
                }
            }
            // Store the characters to form the lines as needed
        }
    }
    else if (argc == 4 && strcmp(argv[2], "-n") == 0)
    {
        int tedad = atoi(argv[3]);
        FILE *file = fopen("commits.txt", "r");
        if (file == NULL)
        {
            printf("No commits have been made\n");
            return 0;
        }

        fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file

        long int position = ftell(file); // Get the current position of the file pointer
        char line[100];
        while (position && tedad > 0)
        {
            fseek(file, --position, SEEK_SET); // Move the file pointer backward by one position
            int c = fgetc(file);               // Read the character
            if (c == '\n' || position == 0)
            {
                if (position != 0)
                {
                    fseek(file, 1, SEEK_CUR); // Move the file pointer one position forward to read the newline character
                }
                if (position == 0)
                {
                    fseek(file, 0, SEEK_SET); // If it's the first line, move the file pointer to the beginning of the file
                }
                if (fgets(line, 100, file) != NULL)
                {
                    // Process the line as needed
                    char tempbranchpath[MAX_PATH];
                    GetFullPathName(line, MAX_PATH, tempbranchpath, NULL);
                    tempbranchpath[strlen(tempbranchpath) - 1] = '\0';
                    printf("%s", line);
                    chdir(tempbranchpath);
                    FILE *config = fopen("config.txt", "r");
                    char templine[100];
                    while (fgets(templine, 100, config) != NULL)
                    {
                        printf("%s", templine);
                    }
                    fclose(config); // Close the "config.txt" file after reading
                }
            }
            tedad--;
            // Store the characters to form the lines as needed
        }
    }
    if (argc == 4 && strcmp(argv[2], "-branch") == 0)
    {
        DWORD fileAttributes = GetFileAttributes(argv[3]);
        if (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // printf("Directory '%s' exists in the current directory.\n", directoryName);
        }
        else
        {
            printf("Directory '%s' does not exist in the current directory.\n", argv[3]);
            return 0;
        }
        char tempbranchpath[MAX_PATH];
        GetFullPathName(argv[3], MAX_PATH, tempbranchpath, NULL);
        chdir(tempbranchpath);
        char tempcommit[100];
        int tedad;
        FILE *fp = fopen("commit.txt", "r");
        fscanf(fp, "%d", &tedad);
        fclose(fp);
        tedad--;
        while (tedad >= 0)
        {
            char tempcommit[100];
            sprintf(tempcommit, "%s%d", argv[3], tedad);
            char tempbranchcommit[MAX_PATH];
            GetFullPathName(tempcommit, MAX_PATH, tempbranchcommit, NULL);
            // tempbranchpath[strlen(tempbranchpath) - 1] = '\0';
            // printf("%s", line);
            chdir(tempbranchcommit);
            FILE *config = fopen("config.txt", "r");
            char templine[100];
            while (fgets(templine, 100, config) != NULL)
            {
                printf("%s", templine);
            }
            tedad--;
        }
    }
    else if (argc == 4 && strcmp(argv[2], "-author") == 0)
    {
        FILE *file = fopen("commits.txt", "r");
        if (file == NULL)
        {
            printf("No commits have been made\n");
            return 0;
        }

        fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file

        long int position = ftell(file); // Get the current position of the file pointer
        char line[100];
        while (position)
        {
            fseek(file, --position, SEEK_SET); // Move the file pointer backward by one position
            int c = fgetc(file);               // Read the character
            if (c == '\n' || position == 0)
            {
                if (position != 0)
                {
                    fseek(file, 1, SEEK_CUR); // Move the file pointer one position forward to read the newline character
                }
                if (position == 0)
                {
                    fseek(file, 0, SEEK_SET); // If it's the first line, move the file pointer to the beginning of the file
                }
                if (fgets(line, 100, file) != NULL)
                {
                    // Process the line as needed
                    char tempbranchpath[MAX_PATH];
                    GetFullPathName(line, MAX_PATH, tempbranchpath, NULL);
                    tempbranchpath[strlen(tempbranchpath) - 1] = '\0';

                    chdir(tempbranchpath);
                    FILE *config = fopen("config.txt", "r");
                    char tempauthor[100];
                    fgets(tempauthor, 100, config);
                    tempauthor[strlen(tempauthor) - 1] = '\0';
                    if (strcmp(tempauthor, argv[3]) == 0)
                    {
                        printf("%s", line);
                        printf("%s\n", tempauthor);
                        char templine[100];
                        while (fgets(templine, 100, config) != NULL)
                        {
                            printf("%s", templine);
                        }
                    }

                    fclose(config); // Close the "config.txt" file after reading
                }
            }
            // Store the characters to form the lines as needed
        }
    }
    else if (argc == 4 && strcmp(argv[2], "-since") == 0)
    {
        SYSTEMTIME targetDateSt;
        sscanf(argv[3], "%d-%d-%d", &targetDateSt.wYear, &targetDateSt.wMonth, &targetDateSt.wDay);
        FILETIME targetDateFt;
        SystemTimeToFileTime(&targetDateSt, &targetDateFt);

        searchDirectoriesForConfigFile(argv[3]);
    }
    else if (argc == 4 && strcmp(argv[2], "-search") == 0)
    {
        FILE *file = fopen("commits.txt", "r");
        if (file == NULL)
        {
            printf("No commits have been made\n");
            return 0;
        }

        fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file

        long int position = ftell(file); // Get the current position of the file pointer
        char line[100];
        while (position)
        {
            fseek(file, --position, SEEK_SET); // Move the file pointer backward by one position
            int c = fgetc(file);               // Read the character
            if (c == '\n' || position == 0)
            {
                if (position != 0)
                {
                    fseek(file, 1, SEEK_CUR); // Move the file pointer one position forward to read the newline character
                }
                if (position == 0)
                {
                    fseek(file, 0, SEEK_SET); // If it's the first line, move the file pointer to the beginning of the file
                }
                if (fgets(line, 100, file) != NULL)
                {
                    // Process the line as needed
                    char tempbranchpath[MAX_PATH];
                    GetFullPathName(line, MAX_PATH, tempbranchpath, NULL);
                    tempbranchpath[strlen(tempbranchpath) - 1] = '\0';

                    chdir(tempbranchpath);
                    FILE *config = fopen("config.txt", "r");
                    char tempauthor[100];
                    fgets(tempauthor, 100, config);
                    // printf("%s", line);
                    // printf("%s\n", tempauthor);
                    char tempmessage[100];
                    fgets(tempmessage, 100, config);
                    char *found = strstr(tempmessage, argv[3]);
                    if (found != NULL)
                    {
                        printf("%s", line);
                        printf("%s", tempauthor);
                        printf("%s", tempmessage);
                        char templine[100];
                        while (fgets(templine, 100, config) != NULL)
                        {
                            printf("%s", templine);
                        }
                    }

                    fclose(config); // Close the "config.txt" file after reading
                }
            }
            // Store the characters to form the lines as needed
        }
    }
}
void searchWordInFile(const char *filePath, const char *word, int lineflag)
{
    FILE *file = fopen(filePath, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file '%s'\n", filePath);
        return;
    }
    char line[100];
    int lineNumber = 0;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        lineNumber++;
        char *found = strstr(line, word);
        if (found != NULL)
        {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            int index = found - line; // Calculate the index of the word in the line
            if (lineflag == 1)
            {
                printf("Line %d: ", lineNumber);
            }
            for (int i = 0; i < strlen(line); i++)
            {
                if (i >= index && i < index + strlen(word))
                {
                    SetConsoleTextAttribute(hConsole, 9); // Set color to blue
                    printf("%c", line[i]);                // Print the character in blue
                    SetConsoleTextAttribute(hConsole, 7); // Reset color to default
                }
                else
                {
                    printf("%c", line[i]); // Print the character in default color
                }
            }
        }
    }

    fclose(file);
}
int grep(int argc, char *const argv[])
{
    if (checkrepo() == 0)
    {
        printf("No repositoriy was found\n");
        return 0;
    }
    else if (argc == 6)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 0;
        chdir(temppath);
        char branchname[100];
        FILE *branch = fopen("branch.txt", "r");
        char branchpath[100];
        fgets(branchpath, 100, branch);
        strcpy(branchname, branchpath);
        fclose(branch);
        char mahbod[MAX_PATH];
        GetFullPathName(branchpath, MAX_PATH, mahbod, NULL);
        chdir(mahbod);
        char tempFile[MAX_PATH];
        GetFullPathName(argv[3], MAX_PATH, tempFile, NULL);
        // char ext[] = ".txt";
        // strcat(tempFile, ext);
        searchWordInFile(tempFile, argv[5], 0);
    }
    else if (argc == 7 && strcmp(argv[6], "-c") != 0 && strcmp(argv[6], "-n") == 0)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 0;
        chdir(temppath);
        char branchname[100];
        FILE *branch = fopen("branch.txt", "r");
        char branchpath[100];
        fgets(branchpath, 100, branch);
        strcpy(branchname, branchpath);
        fclose(branch);
        char mahbod[MAX_PATH];
        GetFullPathName(branchpath, MAX_PATH, mahbod, NULL);
        chdir(mahbod);
        char tempFile[MAX_PATH];
        GetFullPathName(argv[3], MAX_PATH, tempFile, NULL);
        // char ext[] = ".txt";
        // strcat(tempFile, ext);
        searchWordInFile(tempFile, argv[5], 1);
    }
    else if (argc == 8 && strcmp(argv[6], "-c") == 0)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 0;
        chdir(temppath);
        char branchname[100];
        char mahbod[MAX_PATH];
        GetFullPathName(argv[7], MAX_PATH, mahbod, NULL);
        chdir(mahbod);
        char tempFile[MAX_PATH];
        GetFullPathName(argv[3], MAX_PATH, tempFile, NULL);
        // char ext[] = ".txt";
        // strcat(tempFile, ext);
        searchWordInFile(tempFile, argv[5], 0);
    }
    else if (argc == 9)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 0;
        chdir(temppath);
        char branchname[100];
        char mahbod[MAX_PATH];
        GetFullPathName(argv[7], MAX_PATH, mahbod, NULL);
        chdir(mahbod);
        char tempFile[MAX_PATH];
        GetFullPathName(argv[3], MAX_PATH, tempFile, NULL);
        // char ext[] = ".txt";
        // strcat(tempFile, ext);
        searchWordInFile(tempFile, argv[5], 1);
    }
    else
    {
        printf("invalid command\n");
    }
}
int checkout(int argc, char *const argv[])
{
    if (argc < 3)
    {
        printf("invalid command\n");
        return 0;
    }
    if (checkrepo() == 0)
    {
        printf("No repo was found\n");
        return 0;
    }
    if (argc == 3)
    {
        if (strcmp(argv[2], "HEAD") == 0)
        {
            findmah();
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            int flag = 0;
            chdir(temppath);
            FILE *head = fopen("HEAD.txt", "r");
            char headpath[100];
            fgets(headpath, 100, head);
            fclose(head);
            FILE *branch = fopen("branch.txt", "w");
            fprintf(branch, "%s", headpath);
            fclose(branch);
        }
        int flag = 0;
        int mahal = 0;
        for (int i = 0; i < strlen(argv[2]); i++)
        {
            if (argv[2][i] >= '0' && argv[2][i] <= '9')
            {
                mahal = i - 1;
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            findmah();
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            int flag = 0;
            chdir(temppath);
            char branchname[100];
            FILE *branch = fopen("branch.txt", "r");
            char branchpath[100];
            fgets(branchpath, 100, branch);
            strcpy(branchname, branchpath);
            fclose(branch);
            char mahbod[MAX_PATH];
            GetFullPathName(branchpath, MAX_PATH, mahbod, NULL);
            chdir(mahbod);
            int tedad;
            FILE *fp = fopen("commit.txt", "r");
            fscanf(fp, "%d", &tedad);
            fclose(fp);
            if (tedad == 0)
            {
                printf("No commit has been made\n");
                return 0;
            }
            tedad--;
            char tempcommit[100];
            sprintf(tempcommit, "%s%d", branchpath, tedad);
            chdir("..");
            FILE *head = fopen("HEAD.txt", "w");
            fprintf(head, "%s", branchname);
            fclose(head);
            branch = fopen("branch.txt", "w");
            fprintf(branch, "%s", tempcommit);
            fclose(branch);
        }
        if (flag == 1)
        {
            findmah();
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            int flag = 0;
            chdir(temppath);
            char branchname[100];
            if (GetFileAttributes(argv[2]) == INVALID_FILE_ATTRIBUTES)
            {
                printf("Folder does not exist.\n");
                return 0;
            }

            for (int i = 0; i <= mahal; i++)
            {
                branchname[i] = argv[2][i];
            }
            branchname[mahal + 1] = '\0';
            FILE *branch = fopen("branch.txt", "w");
            fprintf(branch, "%s", argv[2]);
            fclose(branch);
            FILE *head = fopen("HEAD.txt", "w");
            fprintf(head, "%s", branchname);
            fclose(head);
        }
    }
    if (argc == 4)
    {
        findmah();
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        int flag = 0;
        chdir(temppath);
        FILE *head = fopen("HEAD.txt", "r");
        char headpath[100];
        fgets(headpath, 100, head);
        fclose(head);
        char mahbod[MAX_PATH];
        GetFullPathName(headpath, MAX_PATH, mahbod, NULL);
        chdir(mahbod);
        int tedad;
        FILE *fp = fopen("commit.txt", "r");
        fscanf(fp, "%d", &tedad);
        fclose(fp);
        tedad -= argv[3][5] - '0';
        if (tedad < 0)
        {
            printf("can not get to that commit\n");
            return 0;
        }
        char tempcommit[100];
        sprintf(tempcommit, "%s%d", headpath, tedad);
        chdir("..");
        FILE *branch = fopen("branch.txt", "w");
        fprintf(branch, "%s", tempcommit);
        fclose(branch);
    }
}
void todoCheck(const char *filePath)
{
    // Extract file extension
    const char *extension = strrchr(filePath, '.');
    if (extension == NULL)
    {
        printf("\x1b[34mTodo-check skipped (Invalid file format)\x1b[0m\n"); // Print in blue
        return;
    }
    extension++; // Skip the dot in the extension

    // Check file format
    if (strcmp(extension, "cpp") == 0 || strcmp(extension, "c") == 0)
    {
        // Open the file
        FILE *file = fopen(filePath, "r");
        if (file == NULL)
        {
            printf("\x1b[31mTodo-check skipped (Failed to open file)\x1b[0m\n"); // Print in red
            return;
        }

        // Check file content for TODO
        char line[256];
        while (fgets(line, sizeof(line), file))
        {
            // Look for TODO in comments
            if (strstr(line, "TODO") != NULL)
            {
                printf("\x1b[31mTodo-check failed\x1b[0m\n"); // Print in red
                fclose(file);
                return;
            }
        }

        fclose(file);
        printf("\x1b[32mTodo-check passed\x1b[0m\n"); // Print in green
    }
    else if (strcmp(extension, "txt") == 0)
    {
        // Open the file
        FILE *file = fopen(filePath, "r");
        if (file == NULL)
        {
            printf("\x1b[31mTodo-check skipped (Failed to open file)\x1b[0m\n"); // Print in red
            return;
        }

        // Check file content for TODO
        char word[256];
        while (fscanf(file, "%s", word) != EOF)
        {
            // Look for TODO in text
            if (strcmp(word, "TODO") == 0)
            {
                printf("\x1b[31mTodo-check failed (TODO found in the file)\x1b[0m\n"); // Print in red
                fclose(file);
                return;
            }
        }

        fclose(file);
        printf("\x1b[32mTodo-check passed\x1b[0m\n"); // Print in green
    }
    else
    {
        printf("\x1b[34mTodo-check skipped (Invalid file format)\x1b[0m\n"); // Print in blue
    }
}
bool checkEofBlankSpace(const char *filePath)
{
    const char *ext = strrchr(filePath, '.');
    if (strcmp(ext, ".cpp") == 0 || strcmp(ext, ".c") == 0 || strcmp(ext, ".txt") == 0)
    {
        FILE *file = fopen(filePath, "r");
        if (file == NULL)
        {
            printf("Failed to open file: %s\n", filePath);
            return false;
        }

        fseek(file, -2, SEEK_END);
        int lastChar = fgetc(file);

        fclose(file);

        if (lastChar == '\n' || lastChar == '\r')
        {
            printf("\033[1;32meof-blank-space: Passed for file\033[0m\n");
            return true;
        }
        else
        {
            printf("\033[1;31meof-blank-space: Failed for file\033[0m\n");
            return false;
        }
    }
    else
    {
        printf("Eof Blank Space Check: Skipped\n");
    }
}

bool checkFormat(const char *filePath)
{
    // Check file extension
    const char *ext = strrchr(filePath, '.');
    if (ext != NULL)
    {
        if (strcmp(ext, ".cpp") == 0 || strcmp(ext, ".c") == 0 || strcmp(ext, ".txt") == 0 || strcmp(ext, ".mp3") == 0 || strcmp(ext, ".mp4") == 0 || strcmp(ext, ".wav") == 0)
        {
            printf("\033[1;32format-check: Passed for file\033[0m\n");
            return true;
        }
        else
        {
            printf("\033[1;31mformat-check: Failed for file\033[0m\n");
            return false;
        }
    }

    // If no extension found, it can be skipped
    printf("Format Check: Skipped for file\n");
    return true;
}

bool checkFileSize(const char *filePath)
{
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileInfo))
    {
        printf("Failed to get file size for: %s\n", filePath);
        return false;
    }

    ULONGLONG fileSize = fileInfo.nFileSizeLow;
    if (fileSize <= 5 * 1024 * 1024)
    {
        printf("\033[1;32mFile Size Check: Passed for file\033[0m\n");
        return true;
    }
    else
    {
        printf("\033[1;31mFile Size Check: Failed for file\033[0m\n");
        return false;
    }
}

bool checkCharacterLimit(const char *filePath)
{
    const char *ext = strrchr(filePath, '.');
    if (strcmp(ext, ".cpp") == 0 || strcmp(ext, ".c") == 0 || strcmp(ext, ".txt") == 0)
    {
        FILE *file = fopen(filePath, "r");
        if (file == NULL)
        {
            printf("Failed to open file: %s\n", filePath);
            return false;
        }

        int characterCount = 0;
        int ch;
        while ((ch = fgetc(file)) != EOF)
        {
            characterCount++;
            if (characterCount > 20000)
            {
                fclose(file);
                printf("\033[1;31mcharacter-limit: Failed for file\033[0m\n");
                return false;
            }
        }
        fclose(file);

        printf("\033[1;32mcharacter-limit: Passed for file\033[0m\n");
        return true;
    }
    else
    {
        printf("Character limit check : Skipped for file\n");
    }
    return true;
}

bool checkStaticError(const char *filePath)
{
    const char *ext = strrchr(filePath, '.');
    if (ext != NULL && (strcmp(ext, ".c") == 0 || strcmp(ext, ".cpp") == 0))
    {
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "gcc -Wall -Wextra -pedantic -fsyntax-only %s", filePath);

        int result = system(cmd);
        if (result != 0)
        {
            printf("\033[1;32mstatic-error-check: Passed for file\033[0m\n");
            return false;
        }
        else
        {
            printf("\033[1;31mstatic-error-check: Failed for file\033[0m\n");
            return true;
        }
    }

    printf("Static Error Check: Skipped for file: %s\n", filePath);
    return true;
}
void checkFilesInFolder(const char *folderPath, int eof, int format, int staticheck, int size, int character, int todocheck)
{
    WIN32_FIND_DATA findData;
    char searchPath[MAX_PATH];
    sprintf(searchPath, "%s\\*", folderPath);

    HANDLE findHandle = FindFirstFile(searchPath, &findData);
    if (findHandle == INVALID_HANDLE_VALUE)
    {
        printf("Invalid folder path: %s\n", folderPath);
        return;
    }

    do
    {
        if (strcmp(findData.cFileName, "staging.txt") != 0 && strcmp(findData.cFileName, "hooks.txt") != 0)
        {
            char filePath[MAX_PATH];
            sprintf(filePath, "%s\\%s", folderPath, findData.cFileName);
            if (checkfileordir(filePath) == 0)
            {
                printf("%s\n", getFileName(filePath));
                if (format == 1)
                {
                    checkFormat(filePath);
                }
                if (eof == 1)
                {
                    checkEofBlankSpace(filePath);
                }
                if (size == 1)
                {
                    checkFileSize(filePath);
                }
                if (character == 1)
                {
                    checkCharacterLimit(filePath);
                }
                // checkTimeLimit(filePath);
                if (staticheck == 1)
                {
                    checkStaticError(filePath);
                }
                if (todocheck == 1)
                {
                    todoCheck(filePath);
                }
            }
        }
    } while (FindNextFile(findHandle, &findData));

    FindClose(findHandle);
}

int precommit(int argc, char *const argv[])
{
    findmah();
    chdir(temppath);
    FILE *branch = fopen("branch.txt", "r");
    char branchpath[100];
    fgets(branchpath, 100, branch);
    char *tempbranch = strcat(branchpath, "\\staging");
    char tempbranchpath[MAX_PATH];
    GetFullPathName(tempbranch, MAX_PATH, tempbranchpath, NULL);
    printf("%s\n", tempbranchpath);
    if (GetFileAttributes(tempbranchpath) == INVALID_FILE_ATTRIBUTES)
    {
        printf("Folder does not exist.\n");
        return 0;
    }
    chdir(tempbranchpath);
    int eof = 0;
    int format = 0;
    int size = 0;
    int staticheck = 0;
    int character = 0;
    int todocheck = 0;
    if (argc == 2)
    {
        FILE *hooks = fopen("hooks.txt", "r");
        char line[100];
        while (fgets(line, 100, hooks) != NULL)
        {
            line[strlen(line) - 1] = '\0';
            if (strcmp(line, "eof-blank-space") == 0)
            {
                eof = 1;
            }
            if (strcmp("format-check", line) == 0)
            {
                format = 1;
            }
            if (strcmp(line, "static-error-check") == 0)
            {
                staticheck = 1;
            }
            if (strcmp(line, "file-size-check") == 0)
            {
                size = 1;
            }
            if (strcmp(line, "character-limit") == 0)
            {
                character = 1;
            }
            if (strcmp(line, "todo-check") == 0)
            {
                todocheck = 1;
            }
        }
        fclose(hooks);
        checkFilesInFolder(tempbranchpath, eof, format, staticheck, size, character, todocheck);
    }
    else if (strcmp(argv[2], "applied") == 0)
    {
        FILE *hooks = fopen("hooks.txt", "r");
        char line[100];
        while (fgets(line, 100, hooks) != NULL)
        {
            printf("%s\n", line);
        }
    }
    else if (strcmp(argv[2], "hooks") == 0)
    {
        printf("todo-check\neof-blank-space\nformat-check\nblance-barces\nindentation-check\nstatic-error-check\nfile-size-check\ncharacter-limit\ntime-limit\n");
    }
    else if (strcmp(argv[2], "add") == 0)
    {
        FILE *hooks = fopen("hooks.txt", "a");
        fprintf(hooks, "%s\n", argv[3]);
        fclose(hooks);
    }
    else if (strcmp(argv[2], "remove") == 0)
    {
        FILE *hooks = fopen("hooks.txt", "r+");
        removeLinehook(hooks, argv[3]);
        fclose(hooks);
    }
    else if (strcmp(argv[2], "-f") == 0)
    {
        FILE *hooks = fopen("hooks.txt", "r");
        char line[100];
        while (fgets(line, 100, hooks) != NULL)
        {
            line[strlen(line) - 1] = '\0';
            if (strcmp(line, "eof-blank-space") == 0)
            {
                eof = 1;
            }
            if (strcmp("format-check", line) == 0)
            {
                format = 1;
            }
            if (strcmp(line, "static-error-check") == 0)
            {
                staticheck = 1;
            }
            if (strcmp(line, "file-size-check") == 0)
            {
                size = 1;
            }
            if (strcmp(line, "character-limit") == 0)
            {
                character = 1;
            }
            if (strcmp(line, "todo-check") == 0)
            {
                todocheck = 1;
            }
        }
        fclose(hooks);
        for (int i = 3; i < argc; i++)
        {
            if (_access(argv[i], 0) != 0)
            {
                printf("File does not exist\n");
            }
            else
            {
                char tempfilepath[100];
                GetFullPathName(argv[i], MAX_PATH, tempfilepath, NULL);
                printf("%s\n", getFileName(tempfilepath));
                if (format == 1)
                {
                    checkFormat(tempfilepath);
                }
                if (eof == 1)
                {
                    checkEofBlankSpace(tempfilepath);
                }
                if (size == 1)
                {
                    checkFileSize(tempfilepath);
                }
                if (character == 1)
                {
                    checkCharacterLimit(tempfilepath);
                }
                // checkTimeLimit(filePath);
                if (staticheck == 1)
                {
                    checkStaticError(tempfilepath);
                }
                if (todocheck == 1)
                {
                    todoCheck(tempfilepath);
                }
            }
        }
    }
}
int main(int argc, char *const argv[])
{

    print_command(argc, argv);
    if (strcmp(argv[1], "init") == 0)
    {
        run_init();
    }
    if (strcmp(argv[1], "config") == 0)
    {
        config(argc, argv);
    }
    if (strcmp(argv[1], "add") == 0)
    {
        add(argc, argv);
    }
    if (strcmp(argv[1], "reset") == 0)
    {
        reset(argc, argv);
    }
    if (strcmp(argv[1], "commit") == 0 || strcmp(argv[1], "set") == 0 || strcmp(argv[1], "replace") == 0 || strcmp(argv[1], "remove") == 0)
    {
        commitkon(argc, argv);
    }
    if (strcmp(argv[1], "status") == 0)
    {
        status(argc, argv);
    }
    if (strcmp(argv[1], "branch") == 0)
    {
        branch(argc, argv);
    }
    if (strcmp(argv[1], "log") == 0)
    {
        logkon(argc, argv);
    }
    if (strcmp(argv[1], "grep") == 0)
    {
        grep(argc, argv);
    }
    if (strcmp(argv[1], "checkout") == 0)
    {
        checkout(argc, argv);
    }
    if (strcmp(argv[1], "pre-commit") == 0)
    {
        precommit(argc, argv);
    }

    return 0;
}