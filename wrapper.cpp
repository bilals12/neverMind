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
std::vector<std::string> wrapper::listdir(const std::string& path)
{
    std::vector<std::string> directory_content;
    for (const auto& val : std::filesystem::directory_iterator(path)) {
        std::string content = val.path().u8string();
        std::size_t last_idx = content.find_last_of("\\");
        directory_content.push_back(content.substr(last_idx + 1));
    }
    return directory_content;
}

// generate random string
std::string wrapper::random_string()
{
    std::string tmp_s;
    static const char alphanum[] = 
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

    srand((unsigned)time(NULL) * 38);
    const int STRING_LENGTH = 20; // define constant
    tmp_s.reserve(STRING_LENGTH);

    for (int i = 0; i < STRING_LENGTH; i++)
        tmp_s += alphanum[rand() & (sizeof(alphanum) - 1)];

    return tmp_s;
}

// current date as string
std::string wrapper::get_date()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    return oss.str();
}

// bitmap header
BITMAPINFOHEADER wrapper::createBitmapHeader(int width, int height)
{
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    return bi;
}

// screen capture
HBITMAP wrapper::GdiPlusScreenCapture(HWND hWnd)
{
    // handles to device context (DC)
    HDC hwindowDC = GetDC(hWnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    // scale, height, width
    int scale = 1;
    int screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    // create bitmap
    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    BITMAPINFOHEADER bi = wrapper::createBitmapHeader(width, height);

    // previously created device context with bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);

    // copy from window DC to bitmap DC
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // avoid memory leak
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hWnd, hwindowDC);

    return hbwindow;
}

// save bitmap to memory
bool wrapper::saveToMemory(HBITMAP* hbitmap, std::vector<BYTE>& data, std::string dataFormat)
{
    Gdiplus::Bitmap bmp(*hbitmap, nullptr);
    // write to istream
    IStream* istream = nullptr;
    CreateStreamOnHGlobal(NULL, TRUE, &istream);

    // encoding
    CLSID clsid;
    if (dataFormat.compare("bmp") == 0) { CLSIDFromString(L"{557cf400-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
    else if (dataFormat.compare("jpg") == 0) { CLSIDFromString(L"{557cf401-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
    else if (dataFormat.compare("gif") == 0) { CLSIDFromString(L"{557cf402-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
    else if (dataFormat.compare("tif") == 0) { CLSIDFromString(L"{557cf405-1a04-11d3-9a73-0000f81ef32e}", &clsid); }
    else if (dataFormat.compare("png") == 0) { CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid); }

    Gdiplus::Status status = bmp.Save(istream, &clsid, NULL);
    if (status != Gdiplus::Status::Ok)
        return false;
    
    // memory handle associated with istream
    HGLOBAL hg = NULL;
    GetHGlobalFromStream(istream, &hg);
    // copy istream to buffer
    int bufsize = GlobalSize(hg);
    data.resize(bufsize);
    // lock/unlock memory
    LPVOID pimage = GlobalLock(hg);
    memcpy(&data[0], pimage, bufsize);
    GlobalUnlock(hg);
    istream->Release();
    return true;
}

// screenshot
std::string wrapper::screenshot(const std::string& path)
{
    std::string full_path = (std::filesystem::path(path) / std::filesystem::path((wrapper::get_date() + ".jpg"))).u8string();

    // init GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // bitmap handle to bitmap screenshot
    HWND hWnd = GetDesktopWindow();
    HBITMAP hBmp = wrapper::GdiPlusScreenCapture(hWnd);
    // save as jpg
    std::vector<BYTE> data;
    std::string dataFormat = "jpg";

    if (wrapper::saveToMemory(&hBmp, data, dataFormat))
    {
        // save to file from memory
        std::ofstream fout(full_path, std::ios::binary);
        fout.write((char*)data.data(), data.size());
    }
    else
        return "";
    GdiplusShutdown(gdiplusToken);

    return full_path;
}

// append content to file
void wrapper::append_file(const std::string& file_path, const std::string& content)
{
    std::ofstream file(file_path, std::ios_base::app);
    if (!file.is_open())
        throw FileError("[!] failed to open file: " + file_path);

    file << content;
}