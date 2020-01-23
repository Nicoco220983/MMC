#include <fstream>
#include <exception>
#include <libgen.h>
#include <memory>
#include <sstream>
#include <array>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <cassert>
#include <utime.h>

#include "utils.hpp"

namespace mmc {


void log(LogLevel logLevel, const Context& ctx, const std::string& text){
    if(logLevel == LogLevel::DEBUG){
        if(ctx.logLevel != LogLevel::DEBUG) return;
        std::cout << "[DEBUG]";
    } else if(logLevel == LogLevel::INFO){
        if(ctx.logLevel == LogLevel::ERROR) return;
        std::cout << "[INFO]";
    } else if(logLevel == LogLevel::WARNING){
        if(ctx.logLevel == LogLevel::ERROR) return;
        std::cout << "[WARNING]";
    } else if(logLevel == LogLevel::ERROR){
        std::cout << "[ERROR]";
    }
    std::cout << " ";
    if(not ctx.currentPath.empty())
        std::cout << ctx.currentPath << ": ";
    std::cout << text << std::endl;
}


#define READ_FD       0
#define WRITE_FD      1

std::string exec(const Context& ctx, const std::vector<std::string>& cmd){
    std::stringstream msg;
    msg << "Command:";
    for(auto& c : cmd) msg << " " << c;
    log(LogLevel::DEBUG, ctx, msg.str());

    assert(not cmd.empty());
    assert(cmd[0].size() > 0);

    int pipefd[2];
    assert(pipe(pipefd) >= 0);

    pid_t pid = fork();
    assert(pid >= 0);
    if(pid == 0) {

        dup2(pipefd[WRITE_FD], STDOUT_FILENO);
        dup2(pipefd[WRITE_FD], STDERR_FILENO);
 
        // Close fds not required by child
        close(pipefd[READ_FD]);
        close(pipefd[WRITE_FD]);

        std::vector<char*> argv;
        argv.reserve(cmd.size()+1);
        for(auto& c : cmd) argv.push_back(const_cast<char*>(c.c_str()));
        argv.push_back(0);
        if(execvp(argv[0], &argv[0]) < 0){
            std::cerr << "execv error" << std::endl;
            exit(1);
        }
        exit(0);

    } else {
    
        // close fds not required by parent
        close(pipefd[WRITE_FD]);

        int status;
        if (waitpid(pid, &status, 0) > 0) {

            char buffer[1000];
            std::stringstream res;
            while(true){
                int nb = read(pipefd[READ_FD], buffer, sizeof(buffer)-1);
                if(nb == 0) break;
                buffer[nb] = '\0';
                res << buffer;
            }

            if (not WIFEXITED(status) || WEXITSTATUS(status)){
                std::stringstream msg;
                msg << "Command \"" << cmd[0] << "\" failed with rc " << status;
                msg << std::endl;
                msg << res.str();
                throw std::runtime_error(msg.str().c_str());
            }

            return res.str();

        } else
            throw std::runtime_error("waitpid failed");
    }
}

std::string concat(const std::vector<std::string>& strs){
    size_t len = 0;
    for(auto& s : strs) len += s.size();
    std::string res;
    res.reserve(len);
    for(auto& s : strs) res += s;
    return res;
}

std::vector<std::string> split(const std::string& str, char del){
   std::vector<std::string> res;
   res.reserve(str.size());
   std::string token;
   std::istringstream tokenStream(str);
   auto pos = tokenStream.tellg();
   while(true){
      std::getline(tokenStream, token, del);
      if(pos == tokenStream.tellg()) break;
      pos = tokenStream.tellg();
      if(not token.empty()) res.push_back(token);
   }
   return res;
}

std::string toLower(const std::string& str){
    std::string res = str;
    std::for_each(res.begin(), res.end(), [](char & c){
        c = ::tolower(c);
    });
    return res;
}

bool iequals(const std::string& str1, const std::string& str2){
    std::string lstr1 = toLower(str1);
    std::string lstr2 = toLower(str2);
    return str1 == str2;
}


namespace fs {

const char* getExt(const std::string& name){
	const char* last_dot = strrchr(name.c_str(), '.');
	if(last_dot == nullptr) return nullptr;
	return last_dot + 1;
}

FileType getFileType(const std::string& path){
    struct stat info;
    if(stat( path.c_str(), &info ) == 0){
        if(info.st_mode & S_IFDIR)
            return FileType::DIR;
        if(info.st_mode & S_IFREG)
            return FileType::REG;
    }
    return FileType::NONE;
}

std::string join(const std::string& a, const std::string& b){
    std::string res;
    if(b.size() == 0) {
        res += a;
    } else if(a.size() == 0) {
        res += b;
    } else {
        res.reserve(a.size()+b.size()+1);
        res += a;
        res += DIR_SEP;
        res += b;
    }
    return res;
}

bool exists(const std::string& path) {
  struct stat buffer;   
  return (stat(path.c_str(), &buffer) == 0); 
}

void copy(const std::string& ipath, const std::string& opath){
    std::ifstream src(ipath, std::ios::binary);
    if(src.fail())
        throw std::logic_error(concat({"Cannot read file: ", ipath}).c_str());
    std::ofstream dst(opath, std::ios::binary);
    if(dst.fail())
        throw std::logic_error(concat({"Cannot create file: ", opath}).c_str());
    dst << src.rdbuf();
}

std::string dirname(const std::string& path){
    char* path2 = strdup(path.c_str());
    const char* path3 = ::dirname(path2);
    std::string res = path3;
    delete(path2);
    return res;
}

void mkdirp(const std::string& path, mode_t mode){
    auto t = getFileType(path);
    if(t == FileType::DIR) return;
    mkdirp(fs::dirname(path).c_str(), mode);
    auto res = mkdir(path.c_str(), mode);
    if(res != 0) throw std::logic_error(concat({"Cannot to make dir: ", path}));
}

void dupTime(const std::string& iPath, const std::string& oPath){
    struct stat st;
    time_t mtime;
    struct utimbuf new_times;

    stat(iPath.c_str(), &st);
    mtime = st.st_mtime; // seconds since the epoch

    new_times.actime = st.st_atime; // keep atime unchanged
    new_times.modtime = st.st_mtime;    // keep mtime unchanged
    utime(oPath.c_str(), &new_times);
}

}

}