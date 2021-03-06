/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#import <objc/runtime.h>

#import <JavaScriptCore/JavaScriptCore.h>

#import "RCTInvalidating.h"

typedef void (^RCTJavaScriptCompleteBlock)(NSError *error);
typedef void (^RCTJavaScriptCallback)(id json, NSError *error);

/**
 * Abstracts away a JavaScript execution context - we may be running code in a
 * web view (for debugging purposes), or may be running code in a `JSContext`.
 */
@protocol RCTJavaScriptExecutor <RCTInvalidating>


/**
 * Executes given string on JS thread and calls the given callback
 * with JSValue and JSContext as a result of the JS module call.
 */
- (void)executeJSString:(NSString*)execString
                context:(NSNumber *)executorID
               callback:(RCTJavaScriptCallback)onComplete;

/**
 * Executes given method with arguments on JS thread and calls the given callback
 * with JSValue and JSContext as a result of the JS module call.
 */
- (void)executeJSCall:(NSString *)name
               method:(NSString *)method
            arguments:(NSArray *)arguments
              context:(NSNumber *)executorID
             callback:(RCTJavaScriptCallback)onComplete;

/**
 * Runs an application script, and notifies of the script load being complete via `onComplete`.
 */
- (void)executeApplicationScript:(NSString *)script
                       sourceURL:(NSURL *)url
                      onComplete:(RCTJavaScriptCompleteBlock)onComplete;

- (void)injectJSONText:(NSString *)script
   asGlobalObjectNamed:(NSString *)objectName
              callback:(RCTJavaScriptCompleteBlock)onComplete;

/**
 * Enqueue a block to run in the executors JS thread. Fallback to `dispatch_async`
 * on the main queue if the executor doesn't own a thread.
 */
- (void)executeBlockOnJavaScriptQueue:(dispatch_block_t)block;

@optional

/**
 * Special case for Timers + ContextExecutor - instead of the default
 *   if jsthread then call else dispatch call on jsthread
 * ensure the call is made async on the jsthread
 */
- (void)executeAsyncBlockOnJavaScriptQueue:(dispatch_block_t)block;

@end

static const char *RCTJavaScriptExecutorID = "RCTJavaScriptExecutorID";
__used static id<RCTJavaScriptExecutor> RCTCreateExecutor(Class executorClass)
{
  static NSUInteger executorID = 0;
  id<RCTJavaScriptExecutor> executor = [[executorClass alloc] init];
  if (executor) {
    objc_setAssociatedObject(executor, RCTJavaScriptExecutorID, @(++executorID), OBJC_ASSOCIATION_RETAIN);
  }
  return executor;
}

__used static NSNumber *RCTGetExecutorID(id<RCTJavaScriptExecutor> executor)
{
  return executor ? objc_getAssociatedObject(executor, RCTJavaScriptExecutorID) : @0;
}
