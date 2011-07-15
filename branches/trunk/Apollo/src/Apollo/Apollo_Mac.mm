// ============================================================================
//
// Apollo
//
// ============================================================================

#import <Foundation/Foundation.h>
#include "Apollo.h"

String Apollo::getAppBasePath()
{
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  String sPath = String::FilenameBasePath([[[NSBundle mainBundle] executablePath] fileSystemRepresentation]);
  [pPool release];
  return sPath;
}

String Apollo::getAppResourcePath()
{
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  
  String sPath = [[[NSBundle mainBundle] resourcePath] fileSystemRepresentation];
  if (!sPath.endsWith(String::PathSeparator())) { sPath.append(String::PathSeparator()); }
  
  [pPool release];
  
  return sPath;
}

String Apollo::getAppLibraryPath()
{
  
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  
  String sPath = [[[NSBundle mainBundle] privateFrameworksPath] fileSystemRepresentation];
  if (!sPath.endsWith(String::PathSeparator())) { sPath.append(String::PathSeparator()); }
  
  [pPool release];
  
  return sPath;
}

String Apollo::getAppModulePath()
{
  
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  
  String sPath = [[[NSBundle mainBundle] builtInPlugInsPath] fileSystemRepresentation];
  if (!sPath.endsWith(String::PathSeparator())) { sPath.append(String::PathSeparator()); }
  
  [pPool release];
  
  return sPath;
}

String Apollo::getModuleLibraryPath(const char* szModuleBaseName)
{
  String sResult;
  String sModuleBundlePath = Apollo::getAppModulePath() + Apollo::getModulePackageName(szModuleBaseName) + ".bundle";
  NSBundle* pModuleBundle = nil;
  
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
  
  NSString* nsModuleBundlePath = [NSString stringWithUTF8String: (const char*) sModuleBundlePath];
  if (nsModuleBundlePath != nil) {
    pModuleBundle = [NSBundle bundleWithPath:nsModuleBundlePath];
    if (pModuleBundle == nil) {
      sModuleBundlePath = Apollo::getAppModulePath() + Apollo::getModulePackageName(String::ToLower(szModuleBaseName)) + ".bundle";
      //[nsModuleBundlePath release];
      nsModuleBundlePath = [NSString stringWithUTF8String: (const char*) sModuleBundlePath];
      if (nsModuleBundlePath != nil) {
        pModuleBundle = [NSBundle bundleWithPath:nsModuleBundlePath];
      }
    }
    //[nsModuleBundlePath release];
  }
  if (pModuleBundle != nil) {
    sResult = [[pModuleBundle privateFrameworksPath] fileSystemRepresentation];
    //[pModuleBundle release];
    if (!sResult.endsWith(String::PathSeparator())) { sResult.append(String::PathSeparator()); }
  }
  
  [pPool release];
  
  return sResult;
}

String Apollo::getModuleResourcePath(const char* szModuleBaseName)
{
  String sResult;
  String sModuleBundlePath = Apollo::getAppModulePath() + Apollo::getModulePackageName(szModuleBaseName) + ".bundle";
  NSBundle* pModuleBundle = nil;
  
  NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];

  NSString* nsModuleBundlePath = [NSString stringWithUTF8String: (const char*) sModuleBundlePath];
  if (nsModuleBundlePath != nil) {
    pModuleBundle = [NSBundle bundleWithPath:nsModuleBundlePath];
    if (pModuleBundle == nil) {
      sModuleBundlePath = Apollo::getAppModulePath() + Apollo::getModulePackageName(String::ToLower(szModuleBaseName)) + ".bundle";
      //[nsModuleBundlePath release];
      nsModuleBundlePath = [NSString stringWithUTF8String: (const char*) sModuleBundlePath];
      if (nsModuleBundlePath != nil) {
        pModuleBundle = [NSBundle bundleWithPath:nsModuleBundlePath];
      }
    }
    //[nsModuleBundlePath release];
  }
  if (pModuleBundle != nil) {
    sResult = [[pModuleBundle resourcePath] fileSystemRepresentation];
    //[pModuleBundle release];
    if (!sResult.endsWith(String::PathSeparator())) { sResult.append(String::PathSeparator()); }
  }
  
  [pPool release];
  
  return sResult;
}

