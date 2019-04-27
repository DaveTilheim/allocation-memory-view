#ifndef __AMVC_H__
#define __AMVC_H__


#ifdef __APPLE__
#define _VERSION "\033[97mAMVC Apple version (1.0.0)\033[0m"
#endif
#ifdef _WIN32
#define _VERSION "\033[97muntested AMVC Win32 version (0.0.1)\033[0m"
#endif
#ifdef __linux__
#define _VERSION "\033[97muntested AMVC linux version (0.0.1)\033[0m"
#endif

#define _AUTOR "\033[97mAuthor: Detrembleur Arthur\033[0m \033[91m(Copyright ©)\033[0m"
#define _GITHUB "\033[97mGitHub:\033[0m \033[33mhttps://github.com/DaveTilheim\033[0m"

#define amvcError(msg) fprintf(stderr, "\033[91mamvc error:\033[0m %s\n", msg);
#define amvcWarning(msg) fprintf(stderr, "\033[95mamvc warning:\033[0m %s\n", msg);
#define amvcNote(msg) fprintf(stderr, "\033[97mamvc note:\033[0m %s\n", msg);

void generateAMV_H(char *);
int existsFile(const char *);
int duplicateReservedLabel(const char *);
int goodExtentionFile(const char *);
void setAMVCFiles(char *,pList, const char*);
void compileAMVCFiles(const char *,const pList, const char*, const char*, const char*);
void ls_str(char *);

#endif
