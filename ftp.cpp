#include "ftp.h"

// constructor that inits ftp connection with provided host, username, pw
ftpC::ftpC(const std::string& host, const std::string& username, const std::string& password)
    : m_host(host), m_username(username), m_password(password), ftpIO(NULL), ftpS(NULL)
{
}

// destructor that cleans up ftp connection
ftpC::~ftpC()
{
    // close ftp connection
    if (ftpS)
    {
        InternetCloseHandle(ftpS);
        ftpS = NULL;
    }
    // close internet connection
    if (ftpIO)
    {
        InternetCloseHandle(ftpIO);
        ftpIO = NULL;
    }
}

// connect to ftp server
void ftpC::connect()
{
    // open connection (internet)
    ftpIO = InternetOpenA("SystemConnection", INTERNET_OPEN_TYPE_DIRECT, m_host.c_str(), 0, INTERNET_FLAG_CACHE_IF_NET_FAIL);
    // connect to ftp server
    ftpS = InternetConnectA(ftpIO, m_host.c_str(), INTERNET_DEFAULT_FTP_PORT, m_username.c_str(), m_password.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
}

// upload file from local -> ftp server
bool ftpC::upload_file(const std::string& local_path, const std::string& server_name)
{
    // upload
    BOOL result = FtpPutFileA(ftpS, local_path.c_str(), server_name.c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
    return result;
}

// download file from ftp server -> local
bool ftpC::retr_file(const std::string& local_path, const std::string& server_name)
{
    // download
    BOOL result = FtpGetFileA(ftpS, server_name.c_str(), local_path.c_str(), 0, 0, FTP_TRANSFER_TYPE_BINARY, INTERNET_FLAG_HYPERLINK);
    return result;
}

// create new directory
bool ftpC::mkdir(const std::string& folder_name)
{
    BOOL result = FtpCreateDirectoryA(ftpS, folder_name.c_str());
    return result;
}

// switch directory
bool ftpC::cd(const std::string& server_path)
{
    BOOL result = FtpSetCurrentDirectoryA(ftpS, server_path.c_str());
    return result;
}