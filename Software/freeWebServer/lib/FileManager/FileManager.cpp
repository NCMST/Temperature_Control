/**
 * @file FileManager.cpp
 * @author Creciune Catalin creciunelcatalin@gmail.com
 * @brief  This file contains the implementation of the FileManager class
 * @version 0.1
 * @date 2025-02-22
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "FileManager.hpp"

/**
 * @brief listSPIFFSFiles - This function lists all the files from the SPIFFS filesystem
 * 
 */
bool  FileManager::listSPIFFSFiles()
{
    // SPIFFS.format();

    Serial.println("Listing files...");
    File root = SPIFFS.open("/");

    if(!root)
    {
        Serial.println("Failed to open directory");
        return false;
    }

    File file = root.openNextFile();
    while (file)
    {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }

    return true;
}

/**
 * @brief readFile - This function reads the content of a file from the SPIFFS filesystem
 * 
 * returns the content of the file if the file was read successfully
 * returns an empty string if the file was not read successfully 
 * 
 * @param path - the path of the file to be read
 * @return String - the content of the file
 */
String FileManager::readFile(const char *path)
{
    String content;

    File file = SPIFFS.open(path, "r");

    if (!file || file.isDirectory())
    {
        Serial.println("Failed to open file for reading");
        return String();
    }

    while (file.available())
    {
        content += String((char)file.read());
    }

    file.close();

    return content;
}

/**
 * @brief updateFile - This function updates the content of a file from the SPIFFS filesystem
 * 
 * returns true if the file was updated successfully
 * returns false if the file was not updated successfully
 * 
 * @param path 
 * @param content 
 * @return true 
 * @return false 
 */
bool FileManager::updateFile(const char *path, const char *content)
{
    File file = SPIFFS.open(path, "a");

    if (!file)
    {
        Serial.println("Failed to open file for writing");
        return false;
    }

    file.print(content);
    file.println();

    file.close();

    return true;
}

/**
 * @brief FileManager::FileManager - Constructor of the FileManager class
 * 
 */
FileManager::FileManager(/* args */)
{
    if (!SPIFFS.begin(true)) {
        Serial.println("An error has occurred while mounting SPIFFS");
    } else {
        Serial.println("SPIFFS mounted successfully");
    }
}

/**
 * @brief FileManager::~FileManager - Destructor of the FileManager class
 * 
 */
FileManager::~FileManager()
{
    SPIFFS.end();
}