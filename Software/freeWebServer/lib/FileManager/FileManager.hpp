/**
 * @file FileManager.hpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief A class that manages the files in the application
 * @version 0.1
 * @date 2025-02-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <SPIFFS.h>
#include <FS.h>
#include <vector>


/**
 * 
 * @class FileManager
 * @brief FlieManager class that manages the files in the application
 * 
 * @version 0.1
 * 
 * @function listSPIFFSFiles() - List all the files in the SPIFFS
 * @function readFile() - Read the content of a file
 * @function updateFile() - Update the content of a file
 * 
 * 
 */
class FileManager
{
private:
    /* data */
public:

    FileManager(/* args */);
    /**
     * @brief Initialize the SPIFFS
     * 
     */
    bool listSPIFFSFiles();

    /**
     * @brief Create a file in the SPIFFS
     * 
     * @param path The path to the file
     * @param content The content of the file
     */
    String readFile(const char *path);

    /**
     * @brief update the content of a file
     * 
     */
    bool updateFile(const char *path, const char *content);


    ~FileManager();
};


#endif // FILEMANAGER_HPP