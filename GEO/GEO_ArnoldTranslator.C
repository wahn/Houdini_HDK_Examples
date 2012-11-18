#include <GU/GU_Detail.h>
// header file
#include "GEO_ArnoldTranslator.h"

// class GEO_ArnoldIOTranslator

GEO_IOTranslator*
GEO_ArnoldIOTranslator::duplicate() const {
    return new GEO_ArnoldIOTranslator(*this);
}

const char*
GEO_ArnoldIOTranslator::formatName() const {
    return "Arnold's .ass scene description files";
}

int
GEO_ArnoldIOTranslator::checkExtension(const char* name) {
    UT_String sname(name);

    if (sname.fileExtension() && !strcmp(sname.fileExtension(), ".ass")) {
        return true;
    }

    return false;
}

int
GEO_ArnoldIOTranslator::checkMagicNumber(unsigned magic) {
    return 0;
}

// entry point

SYS_VISIBILITY_EXPORT void
newGeometryIO(void *) {
    GU_Detail::registerIOTranslator(new GEO_ArnoldIOTranslator());
    // TODO: see GEO_VoxelTranslator.C
}
