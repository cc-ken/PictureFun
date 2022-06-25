#include "utils/system.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <thread>
#include <chrono>

#import "cocoa/cocoa.h"

namespace pf
{
    std::string getDefaultFolder()
    {
        NSError *error;
        NSFileManager *manager = [NSFileManager defaultManager];
        NSURL *userLibrary = [manager URLForDirectory:NSLibraryDirectory inDomain:NSUserDomainMask appropriateForURL:nil create:false error:&error];
        
        NSString *myString = [userLibrary path];
        
        return [myString UTF8String];
    }
    
    std::string getCurrFilePath()
    {
        @autoreleasepool {
            NSString *currentpath = [[NSBundle mainBundle] bundlePath];
            
            return std::string([currentpath UTF8String]) + "/Contents";
        }
    }
    
    void getScreenResolution(int &width, int &height) {
        NSRect screenSize = [[NSScreen mainScreen] frame];
        
        width = screenSize.size.width;
        height = screenSize.size.height;
    }

    void shellExec(const std::string& docPath)
    {
        NSString *docPathStr = [NSString stringWithCString: docPath.c_str() encoding:[NSString defaultCStringEncoding]];
        NSURL *docUrl = [NSURL fileURLWithPath: docPathStr];
        [[NSWorkspace sharedWorkspace] openURL: docUrl];
    }
}
