#include "wrapper.h"

// read content from file
std::string wrapper::read_file(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
        throw FileError("[!] failed to open file: " + file_path);
    std::string text;
    std::string line;
    while (std::getline(file, line)) {
        text += line + "\n";
    }
    return text;
}

// write content to file
void wrapper::write_file(const std::string &file_path, const std::string &content)
{
    std::ofstream file(file_path);
    if (!file.is_open())
        throw FileError("[!] failed to open file: " + file_path);
    file << content;
}

// check if path exists
bool wrapper::path_exists(const std::string& path)
{
    return std::filesystem::exists(path);
}

// get username of current user
std::string wrapper::get_username()
{
    char env[] = "USERNAME";
    DWORD username_len = 257;
    char buffer[4096];
    unsigned int out_size = GetEnvironmentVariableA(env, buffer, username_len);
    return std::string(buffer, out_size);
}

// last error as string
std::string wrapper::last_error_string()
{
    // get error message (if any)
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); // no error recorded
    
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    std::string message(messageBuffer, size);

    LocalFree(messageBuffer); // free up the buffer
    return message;
}

// list directory contents
