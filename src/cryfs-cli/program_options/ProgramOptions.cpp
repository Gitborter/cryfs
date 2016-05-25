#include "ProgramOptions.h"
#include <cstring>
#include <cpp-utils/assert/assert.h>

using namespace cryfs::program_options;
using std::string;
using std::vector;
using boost::optional;
namespace bf = boost::filesystem;

ProgramOptions::ProgramOptions(const bf::path &baseDir, const bf::path &mountDir, const optional<bf::path> &configFile,
                               bool foreground, const optional<double> &unmountAfterIdleMinutes,
                               const optional<bf::path> &logFile, const optional<string> &cipher,
                               const optional<uint32_t> &blocksizeBytes,
<<<<<<< HEAD
                               const optional<string> &scryptParameters,
                               const vector<char*> &fuseOptions)
    :_baseDir(baseDir), _mountDir(nullptr), _configFile(configFile), _foreground(foreground),
     _cipher(cipher), _blocksizeBytes(blocksizeBytes), _scryptParameters(scryptParameters), _unmountAfterIdleMinutes(unmountAfterIdleMinutes),
     _logFile(logFile), _fuseOptions(fuseOptions) {

    string mountDirStr = mountDir.native();
    _mountDir = new char[mountDirStr.size()+1];
    std::memcpy(_mountDir, mountDirStr.c_str(), mountDirStr.size()+1);
    // Fuse needs the mountDir passed as first option (first option = position 1, since 0 is the executable name)
    ASSERT(_fuseOptions.size() >= 1, "There has to be one parameter at least for the executable name");
    _fuseOptions.insert(_fuseOptions.begin()+1, _mountDir);
}

ProgramOptions::ProgramOptions(ProgramOptions &&rhs)
    :_baseDir(std::move(rhs._baseDir)), _mountDir(std::move(rhs._mountDir)), _configFile(std::move(rhs._configFile)),
     _foreground(std::move(rhs._foreground)), _cipher(std::move(rhs._cipher)),
     _unmountAfterIdleMinutes(std::move(rhs._unmountAfterIdleMinutes)), _logFile(std::move(rhs._logFile)),
     _scryptParameters(std::move(rhs._scryptParameters)), _fuseOptions(std::move(rhs._fuseOptions)) {
    rhs._mountDir = nullptr;
}

ProgramOptions::~ProgramOptions() {
    if (_mountDir != nullptr) {
        delete[] _mountDir;
    }
=======
                               const vector<string> &fuseOptions)
    :_baseDir(baseDir), _mountDir(mountDir), _configFile(configFile), _foreground(foreground),
     _cipher(cipher), _blocksizeBytes(blocksizeBytes), _unmountAfterIdleMinutes(unmountAfterIdleMinutes),
     _logFile(logFile), _fuseOptions(fuseOptions) {
>>>>>>> cryfs/develop
}

const bf::path &ProgramOptions::baseDir() const {
    return _baseDir;
}

const bf::path &ProgramOptions::mountDir() const {
    return _mountDir;
}

const optional<bf::path> &ProgramOptions::configFile() const {
    return _configFile;
}

bool ProgramOptions::foreground() const {
    return _foreground;
}

const optional<double> &ProgramOptions::unmountAfterIdleMinutes() const {
    return _unmountAfterIdleMinutes;
}

const optional<bf::path> &ProgramOptions::logFile() const {
    return _logFile;
}

const optional<string> &ProgramOptions::cipher() const {
    return _cipher;
}

const optional<uint32_t> &ProgramOptions::blocksizeBytes() const {
    return _blocksizeBytes;
}

<<<<<<< HEAD
const optional<string> &ProgramOptions::scryptParameters() const {
    return _scryptParameters;
}

const vector<char *> &ProgramOptions::fuseOptions() const {
=======
const vector<string> &ProgramOptions::fuseOptions() const {
>>>>>>> cryfs/develop
    return _fuseOptions;
}
