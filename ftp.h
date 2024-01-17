#pragma once
#include <iostream>
#include <Windows.h>
#include <WinInet.h>
#include <string>
#include "wrapper.h"
#pragma comment(lib, "WinInet.lib")

// manage the ftp connection + ops
class ftpC
{
public:
    // constructor that inits the ftp connection with provided host, username, pw
    ftpC(const std::string& host, const std::string& username, const std::string& password);

    // destructor that cleans up ftp connection
    ~ftpC();

    // connects to ftp server using the provided host, username, pw
    void connect();

    // upload file from local -> ftp server; return true if download is successful
    bool upload_file(const std::string& local_path, const std::string& server_name);

    // download file from ftp server -> local; return true if download is successful
    bool retr_file(const std::string& local_path, const std::string& server_name);

    // create new directory on ftp server
    bool mkdir(const std::string& folder_name);

    // switch directory
    bool cd(const std::string& server_path);

private:
    // hostname, username, pw
    std::string m_host;
    std::string m_username;
    std::string m_password;
    // handles (internet, ftp)
    HINTERNET ftpIO, ftpS; 
};