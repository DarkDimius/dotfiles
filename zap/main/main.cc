#include "ProgressIndicator.h"
#include "absl/strings/string_view.h"
#include "rang.hpp"
#include "re2/re2.h"
#include "spdlog/spdlog.h"
#include <cxxopts.hpp>
#include <fstream>
#include <git2.h>
#include <iostream>
#include <memory>
#include <string>
using namespace std;

string readFile(const absl::string_view filename) {
    string fileNameStr(filename.data(), filename.size());
    ifstream fin(fileNameStr);
    if (!fin.good()) {
        throw std::invalid_argument("file does not exist");
    }
    string src;
    fin.seekg(0, ios::end);
    src.reserve(fin.tellg());
    fin.seekg(0, ios::beg);

    src.assign((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    return src;
}

void write(const absl::string_view filename, const absl::string_view text) {
    string fileNameStr(filename.data(), filename.size());
    ofstream fout(fileNameStr);
    if (!fout.good()) {
        throw std::invalid_argument("Can't create file");
    }
    fout << text;
}

string strprintf(const char *format, va_list vlist) {
    char *buf = nullptr;
    int ret = vasprintf(&buf, format, vlist);
    if (ret < 0) {
        throw std::invalid_argument("wtf");
    }
    //    ENFORCE(ret >= 0, "vasprintf failed");
    string str = buf;
    free(buf);
    return str;
}

string strprintf(const char *format, ...) {
    va_list vlist;
    va_start(vlist, format);
    auto str = strprintf(format, vlist);
    va_end(vlist);
    return str;
}

int main(int argc, const char *argv[]) {
    cxxopts::Options options("zap", "fast tools by dd");
    options.add_options()("v,verbose", "Verbosity level [0-3]");
    options.add_options()("h,help", "Show long help");
    options.add_options()("pattern", "pattern", cxxopts::value<string>()->default_value(""));
    options.add_options()("replacement", "replacement", cxxopts::value<string>()->default_value(""));
    options.add_options()("filePattern", "filePattern", cxxopts::value<vector<string>>()->default_value(""));
    options.parse_positional({"pattern", "replacement", "filePattern"});
    options.positional_help("pattern replacement <file_pattern1> <file_pattern2> ...");
    cxxopts::ParseResult raw = options.parse(argc, argv);
    bool verbose = raw.count("v");

    if (raw.count("h") != 0) {
        printf("%s\n", options.help({"", "advanced", "dev"}).c_str());
        return 0;
    }
    if (raw.count("pattern") == 0 || raw.count("replacement") == 0) {
        printf("%s\n", options.help({"", "advanced", "dev"}).c_str());
        return 1;
    }
    auto patternStr = raw["pattern"].as<string>();
    auto replacement = raw["replacement"].as<string>();
    re2::RE2 pattern(patternStr);
    auto filePatternStrings = raw["filePattern"].as<vector<string>>();
    std::vector<unique_ptr<re2::RE2>> filePatterns;
    for (auto &str : filePatternStrings) {
        filePatterns.emplace_back(make_unique<re2::RE2>(str));
    }
    git_libgit2_init();
    git_index *index;
    git_buf buf = GIT_BUF_INIT_CONST(NULL, 0);
    git_repository *repo;
    git_repository_discover(&buf, ".", 0, NULL);
    git_repository_open(&repo, buf.ptr);
    string repoPath(buf.ptr);
    repoPath.resize(repoPath.size() - strlen(".git/"));
    printf("Found repo @ %s\n", repoPath.c_str());
    git_repository_index(&index, repo);
    git_index_read(index, false);
    //    git_repository_free(repo);
    auto ecount = git_index_entrycount(index);

    vector<string> pathsToBeAdded;
    int totalPatches = 0;
    {
        ProgressIndicator replacementProcess(1, "Replacing", ecount);
        for (auto i = 0; i < ecount; ++i) {
            char out[GIT_OID_HEXSZ + 1];
            out[GIT_OID_HEXSZ] = '\0';
            const git_index_entry *e = git_index_get_byindex(index, i);
            auto fileName = strprintf("%s%s", repoPath.c_str(), e->path);
            bool matches = filePatterns.size() == 0;
            for (auto &pat : filePatterns) {
                matches = re2::RE2::PartialMatch(fileName, *pat);
                if (matches) {
                    break;
                }
            }
            if (!matches) {
                continue;
            }
            try {
                auto fileConents = readFile(fileName);
                if (verbose) {
                    git_oid_fmt(out, &e->id);
                    printf("File Path: %s\n", fileName.c_str());
                    printf("    Stage: %d\n", git_index_entry_stage(e));
                    printf(" Blob SHA: %s\n", out);
                    printf("File Mode: %07o\n", e->mode);
                    printf("Dev/Inode: %d/%d\n", (int)e->dev, (int)e->ino);
                    printf("File Size: %d bytes\n", (int)e->file_size);
                    printf("File contents size: %lu bytes\n", fileConents.size());
                    printf("  UID/GID: %d/%d\n", (int)e->uid, (int)e->gid);
                    printf("    ctime: %d\n", (int)e->ctime.seconds);
                    printf("    mtime: %d\n", (int)e->mtime.seconds);
                    printf("\n");
                }
                int replacementsCount = re2::RE2::GlobalReplace(&fileConents, pattern, replacement);
                if (replacementsCount > 0) {
                    pathsToBeAdded.emplace_back(e->path);
                    totalPatches += replacementsCount;
                    write(fileName, fileConents);
                }
            } catch (...) {
                printf("Failed to subtitute on %s\n", fileName.c_str());
            }
            replacementProcess.reportProgress(i);
        }
    }
    printf("Replaced %i times in %lu files\n", totalPatches, pathsToBeAdded.size());
    vector<char *> filesToBeAdded(pathsToBeAdded.size());
    for (int i = 0; i < pathsToBeAdded.size(); i++) {
        filesToBeAdded[i] = (char *)pathsToBeAdded[i].c_str();
    }
    git_strarray arg{filesToBeAdded.data(), filesToBeAdded.size()};
    git_index_add_all(index, &arg, GIT_INDEX_ADD_DEFAULT, nullptr, nullptr);
    git_index_free(index);
    git_libgit2_shutdown();
    return 0;
}
