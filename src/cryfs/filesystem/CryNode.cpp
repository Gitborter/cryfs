#include "CryNode.h"

#include <sys/time.h>

#include "CryDevice.h"
#include "CryDir.h"
#include "CryFile.h"
#include <fspp/fuse/FuseErrnoException.h>
#include <cpp-utils/pointer/cast.h>
#include <cpp-utils/system/clock_gettime.h>

namespace bf = boost::filesystem;

using blockstore::Key;
using blobstore::Blob;
using cpputils::dynamic_pointer_move;
using cpputils::unique_ref;
using boost::optional;
using boost::none;
using std::shared_ptr;
using cryfs::parallelaccessfsblobstore::FsBlobRef;
using cryfs::parallelaccessfsblobstore::DirBlobRef;

//TODO Get rid of this in favor of an exception hierarchy
using fspp::fuse::CHECK_RETVAL;
using fspp::fuse::FuseErrnoException;

namespace cryfs {

CryNode::CryNode(CryDevice *device, optional<unique_ref<DirBlobRef>> parent, const Key &key)
: _device(device),
  _parent(none),
  _key(key) {
  if (parent != none) {
    _parent = cpputils::to_unique_ptr(std::move(*parent));
  }
}

CryNode::~CryNode() {
}

void CryNode::access(int mask) const {
  device()->callFsActionCallbacks();
  //TODO
  return;
  throw FuseErrnoException(ENOTSUP);
}

shared_ptr<const DirBlobRef> CryNode::parent() const {
  ASSERT(_parent != none, "We are the root directory and can't get the parent of the root directory");
  return *_parent;
}

void CryNode::rename(const bf::path &to) {
  device()->callFsActionCallbacks();
  if (_parent == none) {
    //We are the root direcory.
    throw FuseErrnoException(EBUSY);
  }
  auto targetDir = _device->LoadDirBlob(to.parent_path());
  auto old = (*_parent)->GetChild(_key);
  if (old == boost::none) {
    throw FuseErrnoException(EIO);
  }
  fsblobstore::DirEntry oldEntry = *old; // Copying this and not only keeping the reference is necessary, because the operations below (i.e. RenameChild()) might make a reference invalid.
  auto onOverwritten = [this] (const blockstore::Key &key) {
      device()->RemoveBlob(key);
  };
  if (targetDir->key() == (*_parent)->key()) {
    targetDir->RenameChild(oldEntry.key(), to.filename().native(), onOverwritten);
  } else {
    targetDir->AddOrOverwriteChild(to.filename().native(), oldEntry.key(), oldEntry.type(), oldEntry.mode(), oldEntry.uid(), oldEntry.gid(),
                                   oldEntry.lastAccessTime(), oldEntry.lastModificationTime(), onOverwritten);
    (*_parent)->RemoveChild(oldEntry.name());
  }
}

void CryNode::utimens(timespec lastAccessTime, timespec lastModificationTime) {
  device()->callFsActionCallbacks();
  if (_parent == none) {
    //We are the root direcory.
    //TODO What should we do?
    throw FuseErrnoException(EIO);
  }
  (*_parent)->utimensChild(_key, lastAccessTime, lastModificationTime);
}

void CryNode::removeNode() {
  //TODO Instead of all these if-else and having _parent being an optional, we could also introduce a CryRootDir which inherits from fspp::Dir.
  if (_parent == none) {
    //We are the root direcory.
    //TODO What should we do?
    throw FuseErrnoException(EIO);
  }
  (*_parent)->RemoveChild(_key);
  _device->RemoveBlob(_key);
}

CryDevice *CryNode::device() {
  return _device;
}

const CryDevice *CryNode::device() const {
  return _device;
}

unique_ref<FsBlobRef> CryNode::LoadBlob() const {
  return _device->LoadBlob(_key);
}

void CryNode::stat(struct ::stat *result) const {
  device()->callFsActionCallbacks();
  if(_parent == none) {
    //We are the root directory.
	//TODO What should we do?
    result->st_uid = getuid();
    result->st_gid = getgid();
	result->st_mode = S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR;
    result->st_size = fsblobstore::DirBlob::DIR_LSTAT_SIZE;
    //TODO If possible without performance loss, then for a directory, st_nlink should return number of dir entries (including "." and "..")
    result->st_nlink = 1;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
#ifdef __APPLE__
    result->st_atimespec = now;
    result->st_mtimespec = now;
    result->st_ctimespec = now;
#else
    result->st_atim = now;
    result->st_mtim = now;
    result->st_ctim = now;
#endif
  } else {
    (*_parent)->statChild(_key, result);
  }
}

void CryNode::chmod(mode_t mode) {
  device()->callFsActionCallbacks();
  if (_parent == none) {
    //We are the root direcory.
	//TODO What should we do?
	throw FuseErrnoException(EIO);
  }
  (*_parent)->chmodChild(_key, mode);
}

void CryNode::chown(uid_t uid, gid_t gid) {
  device()->callFsActionCallbacks();
  if (_parent == none) {
	//We are the root direcory.
	//TODO What should we do?
	throw FuseErrnoException(EIO);
  }
  (*_parent)->chownChild(_key, uid, gid);
}

}
