//
//  AppDelegate.m
//  devilution-ios
//
//  Created by cobiro on 29/06/2019.
//

#import "AppDelegate.h"

#include "diablo.h"
#include <cstring>

@interface AppDelegate ()

@end

@implementation SDLUIKitDelegate (extra)

+ (NSString *)getAppDelegateClassName {
    return @"AppDelegate";
}
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [super application:application didFinishLaunchingWithOptions:launchOptions];

    // Override point for customization after application launch.

    NSString *path = [NSBundle.mainBundle URLForResource:@"diabdat" withExtension:@"mpq"].URLByDeletingLastPathComponent.path;
    strncpy(dvl::diabdat_mpq_path, path.UTF8String, path.length);

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
