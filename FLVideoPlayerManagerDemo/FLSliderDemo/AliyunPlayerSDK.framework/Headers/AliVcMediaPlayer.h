
/*
 *
 * This framework uses FFmpeg source code.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */


#import <Foundation/Foundation.h> 
#import <UIKit/UIKit.h>

#define ALIVC_EXTERN     extern __attribute__((visibility ("default")))

/**
 *  播放器通知定义
 
 *   AliVcMediaPlayerLoadDidPreparedNotification：播放器初始化视频文件完成通知，调用prepareToPlay函数，会发送该通知，代表视频文件已经准备完成，此时可以在这个通知中获取到视频的相关信息，如视频分辨率，视频时长等。
 
 *   AliVcMediaPlayerPlaybackDidFinishNotification：播放完成通知。当视频播放完成后会收到此通知。播放完成会有几种情况，
        1. 当用户调用stop后视频结束完成。
        2. 视频正常播放结束。
 
 * AliVcMediaPlayerStartCachingNotification:播放器开始缓冲视频时发送该通知，当播放网络文件时，网络状态不佳或者调用seekTo时，此通知告诉用户网络下载数据已经开始缓冲。
 
 * AliVcMediaPlayerEndCachingNotification:播放器结束缓冲视频时发送该通知，当数据已经缓冲完，告诉用户已经缓冲结束，来更新相关UI显示。
 
 * AliVcMediaPlayerPlaybackErrorNotification:播放器播放失败发送该通知，并在该通知中可以获取到错误码。
 
 * AliVcMediaPlayerSeekingDidFinishNotification:播放器位置改变完成后发送该通知。
 
 * AliVcMediaPlayerFirstFrameNotification:播放器状态首帧显示后发送的通知。

 播放器通知发送逻辑：
    1.	调用prepareToPlay成功后发送AliVcMediaPlayerLoadDidPreparedNotification通知，失败则会发送AliVcMediaPlayerPlaybackErrorNotification。
    2.	调用play、pause、stop、prepareToPlay、seekTo失败后发送AliVcMediaPlayerPlaybackErrorNotification通知。
    3.	调用stop/reset成功后播放视频结束发送AliVcMediaPlayerPlaybackDidFinishNotification通知，播放器自动播放结束也会发送AliVcMediaPlayerPlaybackDidFinishNotification通知。
    4.	调用seekTo成功后发送AliVcMediaPlayerSeekingDidFinishNotification通知。
    5.  AliVcMediaPlayerStartCachingNotification和AliVcMediaPlayerEndCachingNotification通知，这个是在网络视频缓冲数据不足以够播放后会发送此通知，一般网络视频在调用seekTo后会发送此通知。
 */
ALIVC_EXTERN NSString * const AliVcMediaPlayerLoadDidPreparedNotification;
ALIVC_EXTERN NSString * const AliVcMediaPlayerPlaybackDidFinishNotification;
ALIVC_EXTERN NSString * const AliVcMediaPlayerStartCachingNotification;
ALIVC_EXTERN NSString * const AliVcMediaPlayerEndCachingNotification;
ALIVC_EXTERN NSString * const AliVcMediaPlayerPlaybackErrorNotification;
ALIVC_EXTERN NSString * const AliVcMediaPlayerSeekingDidFinishNotification;
ALIVC_EXTERN NSString * const AliVcMediaPlayerFirstFrameNotification;

//todo：
//首帧渲染

/**
 * AccessKeyID和AccessKeySecret字段
 */
@interface AliVcAccesskey : NSObject

@property(atomic,strong)     NSString* accessKeyId;
@property(atomic,strong)     NSString* accessKeySecret;

@end

/**
 * 获取AccessKeyID和AccessKeySecret协议，用户必须实现该协议来完成授权验证
 */
@protocol AliVcAccessKeyProtocol <NSObject>
- (AliVcAccesskey*)getAccessKeyIDSecret;
@end

/**
 *  播放器错误代码
 *  可以在errorCode中获取到错误代码
 *  在AliVcMediaPlayerPlaybackErrorNotification获取到视频播放的错误代码
 */
enum{
    ALIVC_SUCCESS = 0,                     // 无错误
    
    ALIVC_ERR_ILLEGALSTATUS = 400,         // 非法的播放器状态
    ALIVC_ERR_NO_NETWORK = 401,            // 网络视频播放错误，没有网络或者网络状态不好的情况下播放网络视频会出现该错误
    ALIVC_ERR_FUNCTION_DENIED = 402,       // 授权功能被拒绝，没有经过授权
    
    ALIVC_ERR_UNKOWN = 501,                // 未知错误
    ALIVC_ERR_NO_INPUTFILE = 502,          // 无输入文件，没有设置dataSource
    ALIVC_ERR_NO_VIEW = 503,               // 没有设置显示窗口
    ALIVC_ERR_INVALID_INPUTFILE = 504,     // 无效的输入文件，不是一个有效的视频文件
    ALIVC_ERR_NO_SUPPORT_CODEC = 505,      // 视频编码格式不支持，不支持这种编码格式的播放
    ALIVC_ERR_NO_MEMORY = 506,             // 没有足够的内存
    ALIVC_ERR_DOWNLOAD_TIMEOUT=507,        // 下载超时
};
typedef NSInteger AliVcMovieErrorCode;


enum {
    MediaType_AUTO = -1,
    MediaType_VOD = 0,
    MediaType_LIVE = 1
};
typedef NSInteger MediaType;

enum {
    scalingModeAspectFit = 0,
    scalingModeAspectFitWithCropping = 1,
};
typedef NSInteger ScalingMode;


/**
 * iOS媒体播放器SDK是在iOS平台上使用的软件开发工具包（Software Developement Kit），为iOS开发者提供简单易用的接口，帮助开发者实现iOS平台上的媒体播放应用开发。该SDK对目前主流的视频格式都提供了良好的支持，支持本地和网络媒体的播放，弥补了系统播放器在媒体格式上的不足。
 
 * 该SDK有一下功能特点：
        方便快捷、低门槛实现媒体播放功能。用户无须关心内部实现细节，只需要自定义界面既可以实现专业级的播放应用。
        流媒体格式的完美支持，支持HLS、RTMP、RTSP、HTTP STREAMING等多种流媒体格式。
        全媒体格式的支持。支持目前所有主流的媒体格式（MP4、AVI、WMV、FLV、MKV、MOV、RMVB、WEBM、MPG、M4V、AVS）等。
        全编码格式的支持，支持最新的HEVC、VP9等编码格式。
 */
@interface AliVcMediaPlayer : NSObject

/**
 * 功能：创建播放器，并设置播放器显示窗口。播放器内部会新建各个播放器变量并初始化，并启动播放器内部流水线线程等。
 * 参数：UIView* view，播放器显示窗口
 * 备注：如果创建播放器的时候view没有，则可以传递nil，可以在后续需要设置view。
 */
- (AliVcMovieErrorCode) create: (UIView*)view;

/**
 * 功能：播放器初始化视频文件，主要目的是分析视频文件内容，读取视频头信息，解析视频流中的视频和音频信息，并根据视频和音频信息去寻找解码器，创建播放线程等。
 * 返回值：当初始化视频文件成功，返回ALIVC_SUCCESS，否则返回失败，具体的逻辑如下：
    1.	验证是否有权限调用该函数，无权限则返回ALIVC_ERR_FUNCTION_DENIED。
    2.	验证视频源dataSource是否为空，为空则返回ALIVC_ERR_NO_INPUTFILE。
    3.	如果播放器是正在播放或者正在暂停状态，则不能够进行prepare，此时返回非法的播放流程错误ALIVC_ERR_ILLEGALSTATUS，如果播放器状态是已经是准备完成状态，则返回ALIVC_SUCCESS。
    4.	对视频进行初始化，如果成功，则会发送LoadDidPreparedNotification通知，表示视频初始化完成。如果失败则会发送PlaybackErrorNotification通知，在错误通知中可以获取到错误代码。
 * 备注：调用该函数完成后立即返回，需要等待准备完成通知AliVcMediaPlayerLoadDidPreparedNotification，收到该通知后代表视频初始化完成，视频准备完成后可以获取到视频的相关信息如：duration、videoWidth、videoHeight。
 */
- (AliVcMovieErrorCode) prepareToPlay: (NSURL*)dataSource;

/**
 * 功能：播放当前视频
 * 返回值：当播放视频成功，返回ALIVC_SUCCESS，否则返回失败，具体见下述逻辑：
    1.	验证是否有权限调用该函数
    2.	如果播放器是停止的状态，则直接返回ALIVC_ERR_ILLEGALSTATUS错误，如果此时播放器为播放的状态，直接返回ALIVC_SUCCESS。
    3.	如果播放器在暂停或者准备完成的状态，则直接启动视频播放。
 * 备注：播放器调用play进行播放，必须在播放器状态为准备完成的状态或者暂停的状态才能进行播放，其他情况都不能够将视频播放起来。
 */
- (AliVcMovieErrorCode) play;

/**
 * 功能：停止当前视频播放，调用此函数则是结束视频播放，视频显示为黑屏，并回到视频播放起始点。
 * 返回值：停止视频播放成功ALIVC_SUCCESS。否则返回失败。具体逻辑如下：
 *   1.	验证是否有权限调用该函数,无权限则返回ALIVC_ERR_FUNCTION_DENIED。
     2.	如果此时播放器的状态为停止状态，直接返回ALIVC_SUCCESS。
     3.	其他情况则停止视频播放，并将播放器状态设置为Stopped，视频停止后会发送视频结束通知AliVcMediaPlayerPlaybackDidFinishNotification。
 * 备注：该函数会停止掉内部音视频解码线程，如果需要重新进行播放，则需要再调用prepareToPlay进行重新对视频进行初始化。
 */
- (AliVcMovieErrorCode) stop;

/**
 * 功能：暂停当前视频播放
 * 返回值：暂停视频播放成功ALIVC_SUCCESS。否则返回失败。具体逻辑如下：
    1.	验证是否有权限调用该函数,无权限则返回ALIVC_ERR_FUNCTION_DENIED。
    2.	如果此时播放器的状态为暂停状态，直接返回ALIVC_SUCCESS。
    3.	如果播放器状态为停止或者准备完成状态则返回错误的播放器状态ALIVC_ERR_ILLEGALSTATUS。
    4.	其他情况则暂停视频播放，并将播放器状态设置为暂停状态。
 
 * 备注：播放器调用pause暂停视频播放，一般是在视频播放的情况下调用此函数。
 */
- (AliVcMovieErrorCode) pause;


/**
 * 功能：跳转视频到新的播放位置
 * 参数：newPos，单位为毫秒
 * 返回值：跳转成功ALIVC_SUCCESS。否则返回失败。具体逻辑如下：
    1.	验证是否有权限调用该函数,无权限则返回ALIVC_ERR_FUNCTION_DENIED。
    2.	如果此时播放器的状态为停止或准备完成状态，返回错误的播放器状态ALIVC_ERR_ILLEGALSTATUS。
    3.	其他情况则进行视频跳转，返回等待视频跳转结束通知AliVcMediaPlayerSeekingDidFinishNotification。
 * 备注：该函数为异步函数，调用此函数后并不会立即进行视频跳转，需要等待视频跳转结束通知。参数的范围为[0,duration]，如果传入的参数小于0，则播放器内部会自动跳转到0，如果传入参数大于duration，则会跳转到duration最后位置。
 */
- (AliVcMovieErrorCode) seekTo: (NSTimeInterval) newPos;


/**
 * 功能：重置播放器，其目的是将播放器所有状态恢复到最初位置，当播放器内部出现错误或其它问题时，调用此函数来进行恢复。
 * 返回值：如果权限验证通过，则会返回ALIVC_SUCCESS。
 * 备注：该函数和stop函数的区别是，reset也能够实现stop的功能，但是reset会去销毁播放器内部的各种变量，然后重新进行变量初始化，view变量等会被设置成空，如果需要播放，则需要重新设置view，并且调用create重新初始化播放器内部各个变量。
 */
- (AliVcMovieErrorCode) reset;

/**
 * 功能：销毁播放器，该函数用来释放播放器内部的所有变量，退出所有线程。
 * 备注：在退出播放器的时候必须调用该函数，用来进行内存释放，否则会存在内存泄露。
 */
-(AliVcMovieErrorCode) destroy;


/**
 * 功能：获取SDK版本号。
 * 返回值：返回NSString类型的版本号。
 * 备注：当开发者反馈问题时，请使用该方法获得SDK版本号并随同问题一起反馈。
 */
- (NSString *) getSDKVersion;


/**
 * 功能：获取SDK中日志的一些属性，double值。
 * 返回值：返回double型日志属性。
 */
-(double) getPropertyDouble:(int)property defaultValue:(double)defaultValue;

/**
 * 功能：获取SDK中日志的一些属性，int64值。
 * 返回值：返回int64型日志属性。
 */
-(int64_t) getPropertyLong:(int)property defaultValue:(int64_t)defaultValue;

/**
 * 功能：获取SDK中日志的一些属性，字符值。
 * 返回值：返回字符型日志属性。
 */
-(NSString*) getPropertyString:(int)property defaultValue:(NSString*)defaultValue;


/**
 * 功能：设置获取AccessKeyID和AccessKeySecret的代理。
 * 返回值：是否设置成功。
 * 备注：必须设置该delegate，否则无法获取AccessKeyId和AccessKeySecret，导致播放器授权失败。
 */
+ (AliVcMovieErrorCode) setAccessKeyDelegate:(id<AliVcAccessKeyProtocol>) delegate;

/**
 * 功能：设置业务ID。
 * 返回值：无。
 * 备注：设置用户使用该SDK的业务ID，用于日志汇报和统计。
 */
- (void) setBussinessID:(NSString*)bussinessID;

/**
 * 功能：设置用户ID。
 * 返回值：无。
 * 备注：设置用户使用该SDK的ID，用于日志汇报和统计。
 */
- (void) setUserID:(NSString*)userID;

/**
 * 功能：包含视频内容的view。
 * 备注：需要在调用prepareToPlay之前设置view，当前view只有视频帧图像，没有相关控制组件，相当于iOS系统播放器MPMoviePlayerController的控制方式controlStyle为MPMovieControlStyleNone的效果。
 */
@property(nonatomic, retain)    UIView *view;

/**
 * 功能：获取当前视频播放位置，只读属性，获取单位为毫秒。
 * 备注：当播放器状态为正在播放或暂停的状态，能够获取到有效值，否则获取值为无效0。
 */
@property(nonatomic, readonly)   NSTimeInterval currentPosition;

/**
 * 功能：获取视频高度，只读属性。
 * 备注：当调用了prepareToPlay后，并不能立即获得videoHeight的值，只有当播放器发送了prepared通知后，videoHeight的值才有效，否则为默认值0。
 */
@property(nonatomic, readonly)  int videoHeight;

/**
 * 功能：获取视频宽度，只读属性。
 * 备注：当调用了prepareToPlay后，并不能立即获得videoWidth的值，只有当播放器发送了prepared通知后，videoWidth的值才有效，否则为默认值0。
 */
@property(nonatomic, readonly)  int videoWidth;

/**
 * 功能：设置是直播还是点播，1为直播，0为点播，默认为-1不进行设置
 * 备注：设置了是直播还是点播，不由播放器自动判定
 */
@property(nonatomic, readwrite)  MediaType mediaType;

/**
 * 功能：设置网络超时时间，单位毫秒
 * 备注：当播放网络流时，设置网络超时时间，默认15000毫秒
 */
@property(nonatomic, readwrite)  int timeout;

/**
 * 功能：当直播时，缓冲区数据大于dropBufferDuration时开始丢数据，单位毫秒，默认8000毫秒
 * 备注：必须大于一个GOP的长度。
 */
@property(nonatomic, readwrite)  int dropBufferDuration;

/**
 * 功能：获取视频时长，只读属性，单位为毫秒。
 * 备注：当调用了prepareToPlay后，并不能立即获得duration的值，只有当播放器发送了prepared通知后，duration的值才有效，否则为默认值0。
 */
@property(nonatomic, readonly)  NSTimeInterval duration;

/**
 * 功能：获取缓冲的时长，只读属性，单位为毫秒。
 * 备注：当调用了prepareToPlay后，并且开始播放后，才能获取该值。
 */
@property(nonatomic, readonly)  NSTimeInterval bufferingPostion;

/**
 * 功能：设置渲染的模式，适应全屏还是充满全屏
 * 备注：scalingModeApsectFill可能会存在crop的情况
 */
@property(nonatomic, readwrite)  ScalingMode scalingMode;

/**
 * 功能：设置是否静音，YES为静音
 */
@property(nonatomic, readwrite)  BOOL muteMode;

/**
 *  获取播放器错误代码（只读），错误代码的类型为：
    enum{
        ALIVC_SUCCESS = 0,                     // 无错误
 
        ALIVC_ERR_ILLEGALSTATUS = 400,         // 非法的播放器状态
        ALIVC_ERR_NO_NETWORK = 401,            // 网络视频播放错误，没有网络或者网络状态不好的情况下播放网络视频会出现该错误
        ALIVC_ERR_FUNCTION_DENIED = 402,       // 授权功能被拒绝，没有经过授权
 
        ALIVC_ERR_UNKOWN = 501,                // 未知错误
        ALIVC_ERR_NO_INPUTFILE = 502,          // 无输入文件，没有设置dataSource
        ALIVC_ERR_NO_VIEW = 503,               // 没有设置显示窗口
        ALIVC_ERR_INVALID_INPUTFILE = 504,     // 无效的输入文件，不是一个有效的视频文件
        ALIVC_ERR_NO_SUPPORT_CODEC = 505,      // 视频编码格式不支持，不支持这种编码格式的播放
        ALIVC_ERR_NO_MEMORY = 506,             // 没有足够的内存
    };
    typedef NSInteger AliVcMovieErrorCode;
 */
@property(nonatomic, readonly)  AliVcMovieErrorCode errorCode;

@end
