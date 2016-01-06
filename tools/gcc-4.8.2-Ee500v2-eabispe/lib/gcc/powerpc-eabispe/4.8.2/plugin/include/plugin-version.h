#include "configargs.h"

#define GCCPLUGIN_VERSION_MAJOR   4
#define GCCPLUGIN_VERSION_MINOR   8
#define GCCPLUGIN_VERSION_PATCHLEVEL   2
#define GCCPLUGIN_VERSION  (GCCPLUGIN_VERSION_MAJOR*1000 + GCCPLUGIN_VERSION_MINOR)

static char basever[] = "4.8.2";
static char datestamp[] = "20131016";
static char devphase[] = "Thu Mar 13 11:33:16 CDT 2014         build.sh rev=963 s=F482 -i /pkg/fs-DTgnu- ELe500v2 -V release_r963_build_CDE_ELe500v2";
static char revision[] = "";

/* FIXME plugins: We should make the version information more precise.
   One way to do is to add a checksum. */

static struct plugin_gcc_version gcc_version = {basever, datestamp,
						devphase, revision,
						configuration_arguments};
