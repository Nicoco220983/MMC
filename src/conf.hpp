#pragma once

#include <set>
#include "utils.hpp"

namespace mmc {

const std::set<const char*, cstrless> IMAGE_EXTS = {
    "jpg", "jpeg",
    "png",
    "gif",
    "bmp"
};

const std::set<const char*, cstrless> VIDEO_EXTS = {
    "mp4",
    "avi",
    "mpeg"
};

}