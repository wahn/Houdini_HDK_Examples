#ifndef __GEO_ArnoldTranslator_h__
#define __GEO_ArnoldTranslator_h__

#include <GEO/GEO_IOTranslator.h>

class GEO_ArnoldIOTranslator : public GEO_IOTranslator {
public:
    GEO_ArnoldIOTranslator() {}
    GEO_ArnoldIOTranslator(const GEO_ArnoldIOTranslator& src)
    : GEO_IOTranslator() {}
    virtual ~GEO_ArnoldIOTranslator() {}
    // pure virtual functions (need implementation)
    virtual GEO_IOTranslator* duplicate() const;
    virtual const char* formatName(void) const;
    virtual int checkExtension(const char* name);
    virtual int checkMagicNumber(unsigned magic);

protected:
private:
};

#endif // __GEO_ArnoldTranslator_h__
