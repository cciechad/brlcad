# Tcl package index file, version 1.0

proc LoadBLT { version dir } {

    set prefix "lib"
    set suffix [info sharedlibextension]
    regsub {\.} $version {} version_no_dots

    # Determine whether to load the full BLT library or
    # the "lite" tcl-only version.

    if { [info commands tk] == "tk" } {
        set name ${prefix}BLT${version_no_dots}${suffix}
    } else {
        set name ${prefix}BLTlite${version_no_dots}${suffix}
    }

    global tcl_platform
    if { $tcl_platform(platform) == "unix" } {
	set library [file join $dir $name]
	if { ![file exists $library] } {
	    # Try the parent directory.
	    set library [file join [file dirname $dir] $name]
	}
	if { ![file exists $library] } {
	    # Try a source build directory.
	    set library [file join [file dirname $dir] .libs/$name]
	}
	if { ![file exists $library] } {
	    # Fall back to default blt path.
	    set library [file join "/usr/brlcad/lib/blt2.4" $name]
	}
	if { ![file exists $library] } {
	    # Fall back to default path.
	    set library [file join "/usr/brlcad" lib/$name]
	}
    } else {
	set library $name
    }
    load $library BLT
}

set version "2.4"

package ifneeded BLT $version [list LoadBLT $version $dir]

# End of package index file
