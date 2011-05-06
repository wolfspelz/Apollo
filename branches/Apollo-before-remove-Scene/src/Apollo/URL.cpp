// ============================================================================
//
// Apollo
//
// ============================================================================

#include "ApCompiler.h"
#include "URL.h"

#if defined(WIN32)

#include <wininet.h>

AP_NAMESPACE_BEGIN

void URL::Prepare(void)
{
  if (this->chars() > 0 && status_ < OK) {

    //set(SimplifyPath(t_str()));

    //TCHAR pBuf[4096]; pBuf[0] = _T('\0');
    //unsigned long nBufSize = sizeof (pBuf);
    //int canonicalize_ok = InternetCanonicalizeUrl(t_str(), pBuf, &nBufSize, 0); //does not work (see remark section in msdn)
    //TCHAR* pData = pBuf;

    //size_t nLen = SSystem::strlen(pBuf);
    const TCHAR* pData = t_str();
    size_t nLen = chars();

    URL_COMPONENTS uc;
    uc.dwStructSize = sizeof(uc);

    uc.lpszScheme = NULL;
    uc.lpszHostName = NULL;
    uc.lpszUserName = NULL;
    uc.lpszPassword = NULL;
    uc.lpszUrlPath = NULL;
    uc.lpszExtraInfo = NULL;
  
    uc.dwSchemeLength = 1;
    uc.dwHostNameLength = 1;
    uc.dwUserNameLength = 1;
    uc.dwPasswordLength = 1;
    uc.dwUrlPathLength = 1;
    uc.dwExtraInfoLength = 1;

    BOOL crack_ok = InternetCrackUrl(pData, nLen, 0, &uc);
    if (crack_ok) {

      String sScheme;
      String sHostName;
      String sUserName;
      String sPassword;
      String sUrlPath;
      String sExtraInfo;

      if (uc.dwSchemeLength > 0) {
        sScheme.set(uc.lpszScheme, uc.dwSchemeLength);
      }
      if (uc.dwHostNameLength > 0) {
        sHostName.set(uc.lpszHostName, uc.dwHostNameLength);
      }
      if (uc.dwUserNameLength > 0) {
        sUserName.set(uc.lpszUserName, uc.dwUserNameLength);
      }
      if (uc.dwPasswordLength > 0) {
        sPassword.set(uc.lpszPassword, uc.dwPasswordLength);
      }
      if (uc.dwUrlPathLength > 0) {
        sUrlPath.set(uc.lpszUrlPath, uc.dwUrlPathLength);
      }
      if (uc.dwExtraInfoLength > 0) {
        sExtraInfo.set(uc.lpszExtraInfo, uc.dwExtraInfoLength);
      }

      if (0) {
      } else if (uc.nScheme == INTERNET_SCHEME_HTTP) {
        proto_ = "http";
      } else if (uc.nScheme == INTERNET_SCHEME_HTTPS) {
        proto_ = "https";
      } else if (uc.nScheme == INTERNET_SCHEME_FTP) {
        proto_ = "ftp";
      } else {
        proto_ = sScheme;
      }
      
      cred_ = sUserName;
      if (! sPassword.empty()) {
        cred_.appendf(":%s", (TCHAR*) sPassword);
      }

      host_ = sHostName;

      if (0) {
      } else if (uc.nScheme == INTERNET_SCHEME_HTTP && uc.nPort == INTERNET_DEFAULT_HTTP_PORT) {
        port_ = "";
        portnum_ = INTERNET_DEFAULT_HTTP_PORT;
      } else if (uc.nScheme == INTERNET_SCHEME_HTTPS && uc.nPort == INTERNET_DEFAULT_HTTPS_PORT) {
        port_ = "";
        portnum_ = INTERNET_DEFAULT_HTTPS_PORT;
      } else if (uc.nScheme == INTERNET_SCHEME_FTP && uc.nPort == INTERNET_DEFAULT_FTP_PORT) {
        port_ = "";
        portnum_ = INTERNET_DEFAULT_FTP_PORT;
      } else {
        port_.appendf(":%d", uc.nPort);
        portnum_ = uc.nPort;
      }
      
      if (sUrlPath.empty() || sUrlPath == "/") {
        path_ = "/";
      } else {
        path_ = String::filenameBasePath(sUrlPath);
      }
      
      sUrlPath.reverseToken("/", file_);
      
      arg_ = sExtraInfo;

      status_ = OK;
    } else {
      status_ = NOTOK;
    }

  }
}

#else

// -------------------------------------------------------------------

AP_NAMESPACE_BEGIN

void URL::Prepare(void)
{
  if (this->chars() > 0 && status_ < OK) {
    char protostr[MAX_URL_PART_PROTO_SIZE];
    char credstr[MAX_URL_PART_CRED_SIZE];
    char hoststr[MAX_URL_PART_HOST_SIZE];
    char portstr[MAX_URL_PART_PORT_SIZE];
    char pathstr[MAX_URL_PART_PATH_SIZE];
    char filestr[MAX_URL_PART_FILE_SIZE];
    char argstr[MAX_URL_PART_ARG_SIZE];
    int implicitport = 0;
    
    int ok= SplitURL((char* ) this->c_str(), force_relative_,  &implicitport, protostr, credstr, hoststr, portstr, pathstr, filestr, argstr);
    if (*protostr != '\0') proto_= protostr;
    if (*credstr != '\0') cred_= credstr;
    if (*hoststr != '\0') host_= hoststr;
    if (*portstr != '\0') {
      if (!implicitport) { port_= portstr; }
      String sPort(portstr);
      portnum_ = String::atoi(sPort.subString(1, sPort.chars() - 1));
    }
    if (*pathstr != '\0') path_= pathstr;
    if (*filestr != '\0') file_= filestr;
    if (*argstr != '\0') arg_= argstr;

    if (ok) {
      status_= OK;
    } else {
      status_= NOTOK;
    }
  }
}

int URL::ScanForProtocol(char* url, char* *partstr, int *partstrlen, char* *nextstr)
{
  int urllen = 0;
  int i = 0, start = 0, phase = 0;
  int success = false, done = false;

  *partstrlen = 0;
  *partstr = 0;
  *nextstr = url;
  urllen = strlen(url);

  while (! done && i < urllen) {
    switch (url[i]) {
      case ' ':
        i++; start++;
        break;
      case ':':
        i++;
        success = true;
        done = true;
        break;
      default:
        if (isalpha(url[i])) {
          i++;
          phase = 1;
        } else done = true;
    }
  }
  if ( success && (( i - start) > 0)  && phase == 1 ) {
    *partstr = &url[start];
    *partstrlen = i - start - 1;
    *nextstr = &url[i];
    success = true;
  } else success = false;
  return success;
}

int URL::ScanForCredentials(char* url, char* *partstr, int *partstrlen, char* *nextstr)
{ 
  int urllen = 0;
  int i = 0, start = 0, phase = 0;
  int success = false, done = false;

  *partstrlen = 0;
  *partstr = 0;
  *nextstr = url;
  urllen = strlen(url);

  while (! done && i < urllen) {
    switch (url[i]) {
      case '/':
        if (phase == 0) {
          i++; start++;
        } else {
          done = true;
        }
        break;
      case '@':
        done = true;
        success = true;
        break;
      default:
        phase = 1;
        i++;
        break;
    }
  }

  if (success) {
    *partstr = &url[start];
    *partstrlen = i - start;
    i++;
    *nextstr = &url[i];
  }

  return success;
}

int URL::ScanForHost(char* url, char* *partstr, int *partstrlen, char* *nextstr, int preferhost)
{ 
  int urllen = 0;
  int i = 0, start = 0, phase = 0, nDots = 0;
  int success = false, done = false, isFilename = false;

  *partstrlen = 0;
  *partstr = 0;
  *nextstr = url;
  urllen = strlen(url);

  while (! done && i < urllen) {
    switch (url[i]) {
      case '/':
        if (phase == 0) {
          i++; start++;
        } else {
          done = true;
          success = true;
        }
        break;
      case ' ':
        if (phase == 0) {
          i++; start++;
        } else {
          done = true;
        }
        break;
      case ':':
        if (phase == 0) {
          i++; start++;
        } else {
          done = true;
          success = true;
        }
        break;
      case '.':
        if (phase == 1) nDots += 1;
        i++;
        phase = 1;
        break;
      case '?':
      case '#':
        done = true;
        success = false;
        break;
      default:
        if (isalnum(url[i]) || url[i] == '-' || url[i] == '_') {
          i++;
          phase = 1;
        } else if (phase == 1) {
          done = true;
        } else {
          i++; start++;
        }
        break;
    }
  }

/* This version recognizes spock/welcome.html as <hostname>/<filename>
  if ( (success || (! done && ( i - start) > 0) ) && nDots != 1 ) {
    if (i - start > 2) {
      isFilename = (  (! strncasecmp( &url[i-5], ".html", 5) )    \
            ||  (! strncasecmp( &url[i-4], ".gif", 4) )   \
            ||  (! strncasecmp( &url[i-4], ".jpg", 4) )   \
            ||  (! strncasecmp( &url[i-4], ".jpeg", 5) )    \
            ||  (! strncasecmp( &url[i-4], ".htm", 4) ) );
    } 
    
    if (! isFilename) {
      *partstr = &url[start];
      *partstrlen = i - start;
      *nextstr = &url[i];
      success = true;
    }
  }
*/
///* This version recognizes spock/welcome.html as <diectory>/<filename>
  if ( (success || (! done && ( i - start) > 0) ) ) {
    if (nDots == 0) {
      isFilename = ! preferhost;
    } else if (nDots == 1) {
      isFilename = ! preferhost; // true; // HW 28.6.98
    } else if (nDots >= 2 && ! preferhost) {
      if (i - start > 2) {
        isFilename = (  (! String::strncasecmp( &url[i-5], ".html", 5) )   
              ||  (! String::strncasecmp( &url[i-4], ".gif", 4) )    
              ||  (! String::strncasecmp( &url[i-4], ".jpg", 4) )    
              ||  (! String::strncasecmp( &url[i-4], ".jpeg", 5) )   
              ||  (! String::strncasecmp( &url[i-4], ".htm", 4) )  );
      }
    }
    
    if (! isFilename) {
      *partstr = &url[start];
      *partstrlen = i - start;
      *nextstr = &url[i];
      success = true;
    }
  }
//*/
  return success;
}

int URL::ScanForPort(char* url, char* *partstr, int *partstrlen, char* *nextstr)
{
  int urllen = 0;
  int i = 0, phase = 0;
  int success = false, done = false;

  *partstrlen = 0;
  *partstr = 0;
  *nextstr = url;
  urllen = strlen(url);

  while (! done && i < urllen) {
    switch (url[i]) {
      case ':':
        if (phase == 0) {
          i++;
          phase = 1;
        } else {
          done = true;
        }
        break;
      case '/':
        done = true;
        if (phase == 1) {
          success = true;
        }
        break;
      default:
        if (isdigit(url[i]) ) {
          if (phase > 0) {
            i++;
          } else {
            done = true;
          }
        } else {
          if (phase > 0) {
            success = true;
          }
          done = true;
        }
        break;
    }
  }

  if (success || (! done && i > 0)) {
    *partstr = &url[0];
    *partstrlen = i;
    *nextstr = &url[i];
    success = true;
  }
  return success;
}

int URL::ScanForPath(char* url, char* *partstr, int *partstrlen, char* *nextstr)
{ 
  int urllen = 0;
  int i = 0, num = 0;
  char c1, c2;
  char str[3];
  int success = false, done = false;
  int slash_cnt = 0;
  char old_section[MAX_URL_PART_PATH_SIZE];
  char new_section[MAX_URL_PART_PATH_SIZE];

  *partstrlen = 0;
  *partstr = 0;
  *nextstr = url;
  urllen = strlen(url);
  *old_section = '\0';
  *new_section = '\0';

  while (! done && i < urllen) {
    switch (url[i]) {
      case '%':
        if (urllen > i + 2) {
          c1 = url[i + 1];
          c2 = url[i + 2];
          if ( isxdigit(c1) && isxdigit(c2)) {
            sprintf( str, "%c%c", c1, c2);
            sscanf( str, "%02x", &num);
            url[i] =(char)  num;
            memmove(&url[i+1], &url[i+3], urllen - i - 2);
          } else {
            done = true;
          }
        } else {
          done = true;
        }
        break;
      case '?':
        done = true;
        success = true;
        break;
      case '#':
        done = true;
        success = true;
        break;
      case '/':
        {
          char* new_section_end;
          int new_section_len;
          char* arg_begin = (char*) strchr(&(url[i+1]), '?');

          new_section_end = (char*) strchr(&(url[i+1]), '/');
          if (new_section_end != 0 && arg_begin != 0) {
            if (arg_begin < new_section_end) {
              new_section_end= 0;
            }
          }

          if ( new_section_end != 0) {
            new_section_len = new_section_end - &(url[i+1]);
            if (new_section_len > 0 && new_section_len < MAX_URL_PART_PATH_SIZE) {

              strncpy(new_section, &(url[i+1]), new_section_len);
              new_section[new_section_len] = '\0';
              if ( ! strncmp(new_section, old_section, new_section_len) ) {
                slash_cnt++;
                if (slash_cnt > MAX_REQUEST_PATH_SECTIONS) {
                  //strncpy(&(url[i]), new_section_end, strlen(new_section_end)+1); 
                  memmove(&(url[i]), new_section_end, strlen(new_section_end)+1);  // copying overlaping blocks
                  i--;
                }
              } else {
                //strncpy(old_section, new_section, new_section_len + 1);
                memmove(old_section, new_section, new_section_len + 1);
                slash_cnt = 0;
              }
            } else {
              //strncpy(old_section, new_section, new_section_len + 1);
              slash_cnt = 0;
            }
          }
          i++;
          break;
        }
/*
      case '/':
        slash_cnt++;
        if (slash_cnt > MAX_URL_PART_PATH_SECTIONS) {
          TCHAR* slash = &(url[i]);
          TCHAR* last_slash = 0; 
          while ((slash = strchr(&(slash[1]), '/')) != 0) {
            last_slash = slash;
          }
          strncpy(&(url[i]), last_slash, strlen(last_slash)+1);
        }
        i++;
        break;
*/
      default:
        i++;
        break;
    }
  }
  if (success || ((! done && i > 0) ) ){
    *partstr = &url[0];
    *partstrlen = i;
    *nextstr = &url[i];
    success = true;
  }

  return success;
}

int URL::ScanForArguments(char* url, char* *partstr, int *partstrlen, char* *nextstr)
{ 
  int urllen = 0;
  int i = 0;
  int success = false, done = false;

  *partstrlen = 0;
  *partstr = 0;
  *nextstr = url;
  urllen = strlen(url);

  while (! done && i < urllen) {
    switch (url[i]) {
      case '%':
/*
        if (urllen > i + 2) {
          char c1, c2;
          char str[3];
          int num = 0;
          c1 = url[i + 1];
          c2 = url[i + 2];
          if ( isxdigit(c1) && isxdigit(c2)) {
            sprintf( str, "%c%c", c1, c2);
            sscanf( str, "%02x", &num);
            url[i] =(char)  num;
            memmove(&url[i+1], &url[i+3], urllen - i - 2);
          } else {
            done = true;
          }
        } else {
          done = true;
        }
        break;
*/
      default:
        i++;
        break;
    }
  }
  if (success || ((! done && i > 0) ) ){
    *partstr = &url[0];
    *partstrlen = i;
    *nextstr = &url[i];
    success = true;
  }

  return success;
}

int URL::IsDirectoryIndex(const char* filename)
{
  AP_UNUSED_ARG(filename);
  int found = false;
//  Elem *tmpElem;

//  if (gDirectoryIndex != 0) {
//    found = FindNamedElemInList(filename, &tmpElem, gDirectoryIndex);
//  }

//  found= (SSystem::stricmp(filename, "index.html") == 0);

  return found;
}

int URL::SplitURL(char* url, int forceRelative, int* implicitport, char* protostr, char* credstr, char* hoststr, char* portstr, char* pathstr, char* filestr, char* argstr)
{
  char workurl[MAX_URL_SIZE], *myurl;
  char* partstart, *nextpart;
  int partlen;
  int protofound = false;
  int credfound = false;
  int hostfound = false;
  int portfound = false;
  int pathfound = false;
  int argsfound = false;
  int success = false;
  char* filebegin;
  int skip_hostport= 0;

//  strncpy(gWorkURL, url, MAXGWORKURL);

  protostr[0] = '\0';
  credstr[0] = '\0';
  hoststr[0] = '\0';
  portstr[0] = '\0';
  filestr[0] = '\0';
  pathstr[0] = '\0';
  argstr[0] = '\0';
  
  if (url == 0) return (false);

  if (strlen(url) < MAX_URL_SIZE - 1) {
    String::strncpy(&workurl[0], url, MAX_URL_SIZE); 
    success = true; myurl = workurl;

    while (*myurl == ' ') myurl++;
    if (myurl[0] == '<') success = false;

    if (success) {
      protofound = ScanForProtocol(myurl, &partstart, &partlen, &nextpart);
      if (protofound && partlen > 0 && partlen < MAX_URL_PART_PROTO_SIZE) {
        memcpy(protostr, partstart, partlen);
        protostr[partlen] = '\0';
        protofound = true;

        if (strlen(protostr) == 2 && protostr[1] == ':' && isalpha(protostr[0]) && isupper(protostr[0])) {
          String::strncpy(protostr, "file", MAX_URL_PART_PROTO_SIZE);
          nextpart= partstart;
        }
      }
      if (protofound) {
        if (String::stricmp(protostr, "file") == 0) {
          skip_hostport= 1;
        }
      }

      if (! skip_hostport) {
        credfound = ScanForCredentials(nextpart, &partstart, &partlen, &nextpart);
        if (credfound && partlen > 0 && partlen < MAX_URL_PART_CRED_SIZE) {
          memcpy(credstr, partstart, partlen);
          credstr[partlen] = '\0';
        }

        hostfound = ScanForHost(nextpart, &partstart, &partlen, &nextpart, protofound || ! forceRelative);
        if (hostfound && partlen > 0 && partlen < MAX_URL_PART_HOST_SIZE) {
          memcpy(hoststr, partstart, partlen);
          hoststr[partlen] = '\0';
          if (! protofound) {
            //strcpy(protostr, "http");
            //protofound = true;
          }
        }
        
        if (hostfound) {
          portfound = ScanForPort(nextpart, &partstart, &partlen, &nextpart);
          if (portfound && partlen > 0 && partlen < MAX_URL_PART_PORT_SIZE) {
            memcpy(portstr, partstart, partlen);
            portstr[partlen] = '\0';
          } else {
            if (protofound) {
              *implicitport = 1;
              if (String::stricmp(protostr, "http") == 0) {
                memcpy(portstr, ":80", strlen(":80")+1);
              } else if (String::stricmp(protostr, "https") == 0) {
                memcpy(portstr, ":443", strlen(":443")+1);
              } else if (String::stricmp(protostr, "jabber") == 0) {
                memcpy(portstr, ":5222", strlen(":5222")+1);
              } else if (String::stricmp(protostr, "xmpp") == 0) {
                memcpy(portstr, ":5222", strlen(":5222")+1);
              }
            }
          }
        }

      } // if (! skip_hostport)

      pathfound = ScanForPath(nextpart, &partstart, &partlen, &nextpart);
      if (pathfound && partlen > 0 && partlen < MAX_URL_PART_PATH_SIZE) {
        memcpy(pathstr, partstart, partlen);
        pathstr[partlen] = '\0';

        filebegin = (char*) strrchr(pathstr, '/');
        if (filebegin != 0) {
          strcpy(filestr, &filebegin[1]); 
          filebegin[1] = '\0';
          if ( IsDirectoryIndex(filestr) )  {
            *filestr = '\0';
          }
        }
      } else if (hostfound) {
        strcpy(pathstr, "/"); 
      }
      
      if (pathfound) {
        argsfound = ScanForArguments(nextpart, &partstart, &partlen, &nextpart);
        if (argsfound && partlen > 0 && partlen < MAX_URL_PART_ARG_SIZE) {
          memcpy(argstr, partstart, partlen);
          argstr[partlen] = '\0';
        }
      }
      
      success = (hostfound || pathfound);
      String sHostStr(hoststr);
      String::strncpy(hoststr, (const char*) String::toLower((const char*) sHostStr), strlen(hoststr)+1);   // lower chars of hostname
    } //success
  }
/*
  printf("\nURL= %s\n", myurl);
  printf("protostr= %s\n", protostr);
  printf("hoststr= %s\n", hoststr);
  printf("portstr= %s\n", portstr);
  printf("pathstr= %s\n", pathstr);
  printf("filestr= %s\n", filestr);
  printf("arguments= %s\n", argstr);
  printf("%s\n", success ? "parse OK" : "parse ERROR" );
*/
  return success;
}

#endif

/*
//          Simplify a URI
//    --------------
// A URI is allowed to contain the seqeunce xxx/../ which may be
// replaced by "" , and the seqeunce "/./" which may be replaced by "/".
// Simplification helps us recognize duplicate URIs. 
//
//  Thus,   /etc/junk/../fred   becomes /etc/fred
//    /etc/junk/./fred  becomes /etc/junk/fred
//
//      but we should NOT change
//    http://fred.xxx.edu/../..
//
//  or  ../../albert.html
//
// In order to avoid empty URLs the following URLs become:
//
//    /fred/..    becomes /fred/..
//    /fred/././..    becomes /fred/..
//    /fred/.././junk/.././ becomes /fred/..
//
// If more than one set of `://' is found (several proxies in cascade) then
// only the part after the last `://' is simplified.
//
// Returns: A string which might be the old one or a new one.

TCHAR* Apollo::URL::SimplifyPath(TCHAR* url)
{
  TCHAR* path;
  TCHAR* p;

  if (url == 0) return url;

  path = url;
  p = path;

  if ((p = path)) {
  TCHAR* end;
  if (!((end = (TCHAR*) SSystem::strchr(path, ';')) || (end = (TCHAR*) SSystem::strchr(path, '?')) ||
        (end = (TCHAR*)  SSystem::strchr(path, '#'))))
    end = path+SSystem::strlen(path);

  // Parse string second time to simplify
  p = path;
  while(p<end) {
      if (*p=='/') {
     if (*(p+1)=='/') {
        TCHAR* orig = p+1;
        TCHAR* dest = p;
        while ((*dest++ = *orig++)); // Remove a slash
    } else if (*(p+1)=='.' && (*(p+2)=='/' || !*(p+2))) {
        TCHAR* orig = p+1;
        TCHAR* dest = (*(p+2)!='/') ? p+2 : p+3;
        while ((*orig++ = *dest++)); // Remove a slash and a dot
        end = orig-1;
    } else if (*(p+1)=='.' && *(p+2)=='.' && (*(p+3)=='/' || !*(p+3))) {
        TCHAR* q = p;
        while (q>path && *--q!='/');         // prev slash
//        if (strncmp(q, "/../", 4)) {
        {
      TCHAR* orig, *dest;
      if (*q == '/') {
        orig = q+1;
        dest = (*(p+3)!='/') ? p+3 : p+4;
      } else {
        orig = q;
        dest = (*(p+3)!='/') ? p+2 : p+3;
      }
      while ((*orig++ = *dest++));     // Remove /xxx/.. 
      end = orig-1;
      p = q;          // Start again with prev slash 
        }
//        } else
//      p++;
    } else if (*(p+1)=='/') {
        while (*(p+1)=='/') {
      TCHAR* orig=p, *dest=p+1;
      while ((*orig++ = *dest++));  // Remove multiple /'s 
      end = orig-1;
        }
    } else
        p++;
      } else
    p++;
  }
  }
  return url;
}
*/

/*
//  Thus,   /etc/junk/../fred   becomes /etc/fred
//    /etc/junk/./fred  becomes /etc/junk/fred
//
//      but we should NOT change
//    http://fred.xxx.edu/../..
//
//  or  ../../albert.html
//
// In order to avoid empty URLs the following URLs become:
//
//    /fred/..    becomes /fred/..
//    /fred/././..    becomes /fred/..
//    /fred/.././junk/.././ becomes /fred/..
void doit(TCHAR* url)
{
  char save[512];
  strcpy(save, url);
  SimplifyPath(url);
  printf("%s --> %s\n", save, url);
}

void testmain()
{
  doit("/../../");
  doit("../../fred");
  doit("../../fred/");
  doit("../../");
  doit("/../../");
  doit("//etc/junk/../fred/");
  doit("/etc/junk/../fred/");
  doit("/etc//junk/../fred/");
  doit("/etc/junk/./fred/");
  doit("../");
  doit("..");
  doit("/fred/..");
  doit("/fred/../");
  doit("/fred/././..");
  doit("/fred/././../");
  doit("/fred/.././junk/.././");
  doit("/fred/.././junk/../.");
  doit("/./");
}
*/

// -------------------------------------------------------------------
// C++ wrapper part

URL::URL(void)
:status_(UNKNOWN)
,force_relative_(0)
,portnum_(0)
{
}

URL::URL(const char* _url)
:String(_url)
,status_(RAW)
,force_relative_(0)
,portnum_(0)
{
}

URL::~URL()
{
}

String &URL::protocol(void)
{
  Prepare();
  return proto_;
}

String &URL::credentials(void)
{
  Prepare();
  return cred_;
}

String &URL::host(void)
{
  Prepare();
  return host_;
}


String &URL::port(void)
{
  Prepare();
  return port_;
}

int URL::portnum(void)
{
  Prepare();
  return portnum_;
}

String &URL::path(void)
{
  Prepare();
  return path_;
}

String &URL::file(void)
{
  Prepare();
  return file_;
}

String &URL::args(void)
{
  Prepare();
  return arg_;
}

void URL::PrepareBase(void)
{
  if (base_.empty()) {
    base_.appendf("%s://%s%s/", StringType(protocol()), StringType(host()), StringType(port()));
  }
}
String &URL::base(void)
{
  PrepareBase();
  return base_;
}

void URL::PrepareFileBase(void)
{
  if (filebase_.empty()) {
//    String cred;
//    if (! credentials().empty()) {
//      cred = credentials();
//      cred += "@";
//    }
//    filebase_.appendf("%s://%s%s%s%s", StringType(protocol()), StringType(cred), StringType(host()), StringType(port()), StringType(path()));
    filebase_.appendf("%s://%s%s%s", StringType(protocol()), StringType(host()), StringType(port()), StringType(path()));
  }
}
String &URL::filebase(void)
{
  PrepareFileBase();
  return filebase_;
}

void URL::PrepareURI(void)
{
  if (uri_.empty()) {
    uri_.appendf("%s%s%s", StringType(path()), StringType(file()), StringType(args()));
  }
}
String &URL::uri(void)
{
  PrepareURI();
  return uri_;
}

void URL::Normalize(void)
{
  String normalized;
  Prepare();
  if (
    proto_ == "http" || 
    proto_ == "https" || 
    proto_ == "ftp"
    ) {
    normalized.appendf("%s://%s%s%s%s%s", StringType(protocol()), StringType(host()), StringType(port()), StringType(path()), StringType(file()), StringType(args()));
    set(normalized);
  }
}

//----------------------------------------------------------

void UriBuilder::setQueryParam(const String& sKey, const String& sValue)
{
  Elem* e = lQuery_.FindByName(sKey);
  if (e) {
    e->setString(sValue);
  } else {
    lQuery_.AddLast(sKey, sValue);
  }
}

void UriBuilder::setQuery(const String& sQuery)
{
  sArgs_ = sQuery;
  sArgs_.escape(String::EscapeURL);
}

String UriBuilder::getQuery()
{
  String sQuery;

  String sArgs = sArgs_;

  for (Elem* e = 0; (e = lQuery_.Next(e)) != 0; ) {
    if (!sArgs.empty()) { sArgs += "&"; }

    String sKey = e->getName();
    sKey.escape(String::EscapeURL);
    String sValue = e->getString();
    sValue.escape(String::EscapeURL);

    sArgs += sKey + "=" + sValue;
  }

  if (!sArgs.empty()) {
    sQuery = "?" + sArgs;
  }

  return sQuery;
}

String UriBuilder::getUri()
{
  if (!sPath_.endsWith("/")) {
    sPath_.append("/");
  }

  String sUri = sPath_ + sFile_ + getQuery();

  return sUri;
}

//----------------------------------------------------------

void UrlBuilder::setPort(int nPort)
{
  if (nPort == 0 || nPort == 80) {
    sPort_ = "";
  } else {
    sPort_.clear();
    sPort_.appendf("%d", nPort);
  }
}

String UrlBuilder::getUrl()
{
  String sUrl;

  if (!sBase_.empty()) {
    sUrl = sBase_ + getQuery();

  } else {
    if (sProtocol_.empty()) { sProtocol_ = "http"; }

    sUrl = sProtocol_ + "://";
    sUrl += sHost_;

    if (!sPort_.empty()) {
      sUrl += ":" + sPort_;
    }

    sUrl += getUri();
  }

  return sUrl;
}

AP_NAMESPACE_END
