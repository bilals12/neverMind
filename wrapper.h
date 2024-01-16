#pragma once
#include <iostream>
#include <stream>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <gdiplus.h>
#include <gdiplusbitmap.h>
#include <gdiplusbase.h>

using namespace Gdiplus;
#pragma comment (lib,"gdiplus.lib")

// enum class for error codes
enum class ErrorCodes
{
    MKDIR_ERROR = 0,
    FILE_ERROR = 1,
    INTERNAL_ERROR = 2,
    FTP_CONNECT_ERROR = 3
};

// custom exception class for file errors
class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& message)
        : std::runtime_error(message) {} // pass message to base class
};

class wrapper
{
public:
    wrapper() = default;
    ~wrapper() = default;

    static std::string read_file(const std::string& file_path); // read content from file
    static void write_file(const std::string& file_path, const std::string& content); // write content to file

    static bool path_exists(const std::string& path); // check if path exists
    static std::string get_username(); // get username of current user

    static std::string last_error_string(); // get last error as string
    static std::vector<std::string> listdir(const std::string& path); // list directory contents

    static std::string screenshot(const std::string& path); // take screenshot and save to path
    static std::string random_string(); // generate random string

    static std::string get_date(); // get current date as string
    static BITMAPINFOHEADER createBitmapHeader(int width, int height); // create bitmap header

    static HBITMAP GdiPlusScreenCapture(HWND hWnd); // screen capture
    static bool saveToMemory(HBITMAP* hbitmap, std::vector<BYTE>& data, std::string dataFormat = "png"); // save bitmap to memory

    static void append_file(const std::string& file_path, const std::string& content); // append content to file
}; 