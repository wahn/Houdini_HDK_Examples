# Default script run when a lamp object is created.

if ( "$arg1" != "" ) then
    set saved_path = `execute("oppwf")`
    opcf /obj/$arg1

    opadd file file1
    opparm file1 file 'deflamp.bgeo'
    oplocate -x 0.18 -y 3.0 file1

    opcf $saved_path
endif
