#include <GU/GU_Detail.h>
#include <UT/UT_IOTable.h> // UTgetGeoExtensions
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

GA_Detail::IOStatus
GEO_ArnoldIOTranslator::fileLoad(GEO_Detail* gdp,
                                 UT_IStream& is,
                                 int ate_magic) {
    return GA_Detail::IOStatus(true);
}

GA_Detail::IOStatus
GEO_ArnoldIOTranslator::fileSave(const GEO_Detail* gdp,
                                 ostream& os) {
    return GA_Detail::IOStatus(true);
}


// entry point

SYS_VISIBILITY_EXPORT void
newGeometryIO(void *) {
    GU_Detail::registerIOTranslator(new GEO_ArnoldIOTranslator());
    UT_ExtensionList* geoextension;
    geoextension = UTgetGeoExtensions();
    if (!geoextension->findExtension("ass")) {
        geoextension->addExtension("ass");
    }
}
