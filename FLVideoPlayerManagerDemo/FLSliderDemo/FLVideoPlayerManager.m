//
//  VZMoviePlayerManager.m
//  FLSliderDemo
//
//  Created by clarence on 16/12/9.
//  Copyright © 2016年 gitKong. All rights reserved.
//

#import "FLVideoPlayerManager.h"

@interface FLVideoPlayerManager ()<AliVcAccessKeyProtocol>
@property (nonatomic,strong)AliVcMediaPlayer *mPlayer;

@property (nonatomic,strong)NSURL *mUrl;

@property (nonatomic,weak)NSTimer *timer;

@property (nonatomic,weak)UIView *mPlayerView;

@property (nonatomic,strong)void(^preparedOperation)(FLVideoPlayerManager *manager) ;
@property (nonatomic,strong)void(^playingOperation)(FLVideoPlayerManager *manager,CGFloat value) ;
@property (nonatomic,strong)void(^finishOperation)(FLVideoPlayerManager *manager) ;
@property (nonatomic,strong)void(^errorOperation)(NSString *errorMsg) ;

@property (nonatomic,assign)BOOL isPrepared;

@property (nonatomic,assign)BOOL isFinished;

@property (nonatomic,copy)NSString *currentTime;

@property (nonatomic,copy)NSString *totalTime;

@property (nonatomic,assign)CGFloat value;
@end

@implementation FLVideoPlayerManager

static NSString* accessKeyID = @"LTAIGtEHyyZNQ1Df";
static NSString* accessKeySecret = @"tHVqdpHG2f894FpMHVkZ2SAazQspnr";


-(AliVcAccesskey*)getAccessKeyIDSecret
{
    AliVcAccesskey* accessKey = [[AliVcAccesskey alloc] init];
    accessKey.accessKeyId = accessKeyID;
    accessKey.accessKeySecret = accessKeySecret;
    return accessKey;
}

+ (instancetype)shareManager{
    static FLVideoPlayerManager *instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (void)setFl_moviePlayerScreenStatus:(FLMoviePlayerScreenStatus)fl_moviePlayerScreenStatus{
    _fl_moviePlayerScreenStatus = fl_moviePlayerScreenStatus;
    if (fl_moviePlayerScreenStatus == FLMoviePlayerScreenStatusProtrait) {
        NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationPortrait];
        [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
    }
    else if(fl_moviePlayerScreenStatus == FLMoviePlayerScreenStatusLandscape){
        NSNumber *value = [NSNumber numberWithInt:UIInterfaceOrientationLandscapeRight];
        [[UIDevice currentDevice] setValue:value forKey:@"orientation"];
    }
}

- (id)fl_player{
    if (_mPlayer) {
        return _mPlayer;
    }
    return nil;
}

- (UIView *)fl_mediaPlayerView:(CGRect)frame URLString:(NSString *)urlString shouldAutoplay:(BOOL)autoPlay{
    // 1、创建承接容器
    UIView *mPlayerView = [[UIView alloc] initWithFrame:frame];
    self.mPlayerView = mPlayerView;
    mPlayerView.backgroundColor = [UIColor clearColor];
    
    // 2、创建播放器
    [self fl_createVideo:mPlayerView url:[NSURL URLWithString:urlString] shouldAutoplay:autoPlay];
    
    return mPlayerView;
}

- (BOOL)fl_removeMediaPlayer{
    // 销毁定时器
    [self fl_invalidateTimer];
    [self fl_removePlayerObserver];
    if (_mPlayer) {
        AliVcMovieErrorCode err = [_mPlayer destroy];
        if(err != ALIVC_SUCCESS){
            return NO;
        }
        else{
            return YES;
        }
    }
    else{
        return NO;
    }
}

-(void) fl_createVideo:(UIView *)mShowView url:(NSURL *)mUrl shouldAutoplay:(BOOL)autoPlay{
    [AliVcMediaPlayer setAccessKeyDelegate:self];
    self.mUrl = mUrl;
    // 新建播放器
    if (!_mPlayer) {
        _mPlayer = [[AliVcMediaPlayer alloc] init];
    }
    // 创建播放器，传入显示窗口
    [_mPlayer create:mShowView];
    // 开启监听
    [self fl_addPlayerObserver];
    // 初始化播放器配置
    [self fl_initVideoBaseMessage];
    
    // 准备播放
    if(![self fl_prepareToPlay]) {
        
        return;
    }
    
    
    if (autoPlay) {
        if(![self fl_playByPrepared]){
            
            return;
        }
    }
}

- (void)fl_initVideoBaseMessage{
    _mPlayer.mediaType = MediaType_VOD;
    _mPlayer.scalingMode = scalingModeAspectFitWithCropping;
    _mPlayer.timeout = 25000;
    _mPlayer.dropBufferDuration = 8000;
    _mPlayer.muteMode = NO;
}


/**
 *  @author gitKong
 *
 *  没prepare，先prepare，在play
 */
- (BOOL)fl_play{
    if (self.isPrepared) {
        return [self fl_playByPrepared];
    }
    else{
        if ([self fl_prepareToPlay]) {
            return [self fl_playByPrepared];
        }
        else{
            return NO;
        }
    }
}

/**
 *  @author gitKong
 *
 *  已经prepare，直接play
 */
- (BOOL)fl_playByPrepared{
    AliVcMovieErrorCode err = [_mPlayer play];
    if(err != ALIVC_SUCCESS) {
        NSLog(@"play failed,error code is %d",(int)err);
        // 移除定时器
        [self fl_invalidateTimer];
        // 准备好，没播放成功
        self.fl_moviePlayerStatus = FLMoviePlayerStatusPrepared;
        return NO;
    }
    else{
        // 开启定时器
        [self fl_fireTimer];
        // 正在播放
        self.fl_moviePlayerStatus = FLMoviePlayerStatusPlaying;
        return YES;
    }
}


/**
 *  @author gitKong
 *
 *  prepare，如果播放器是正在播放或者正在暂停状态，则不能够进行prepare
 */
- (BOOL)fl_prepareToPlay{
    if (self.mUrl) {
        
        AliVcMovieErrorCode err = [_mPlayer prepareToPlay:self.mUrl];
        if(err != ALIVC_SUCCESS) {
            NSLog(@"preprare failed,error code is %d",(int)err);
            self.isPrepared = NO;
            // 没准备
            self.fl_moviePlayerStatus = FLMoviePlayerStatusNonePrepared;
            return NO;
        }
        else{
            // 准备好
            self.fl_moviePlayerStatus = FLMoviePlayerStatusPrepared;
            return YES;
        }
    }
    else{
        self.isPrepared = NO;
        // 没准备
        self.fl_moviePlayerStatus = FLMoviePlayerStatusNonePrepared;
        return NO;
    }
}

- (BOOL)fl_pause{
    if (_mPlayer) {
        AliVcMovieErrorCode err = [_mPlayer pause];
        if(err != ALIVC_SUCCESS){
            // 准备
            self.fl_moviePlayerStatus = FLMoviePlayerStatusPrepared;
            return NO;
        }
        else{
            // 暂停
            self.fl_moviePlayerStatus = FLMoviePlayerStatusPaused;
            // 销毁定时器
            [self fl_invalidateTimer];
            return YES;
        }
    }
    else{
        return NO;
    }
}

- (BOOL)fl_stop{
    if (_mPlayer) {
        AliVcMovieErrorCode err = [_mPlayer stop];
        if(err != ALIVC_SUCCESS){
            self.fl_moviePlayerStatus = FLMoviePlayerStatusPrepared;
            return NO;
        }
        else{
            self.fl_moviePlayerStatus = FLMoviePlayerStatusFinished;
            // 销毁定时器
            [self fl_invalidateTimer];
            return YES;
        }
    }
    else{
        return NO;
    }
}

/**
 *  @author gitKong
 *
 *  跳转到指定位置
 */
- (BOOL)fl_seekTo:(CGFloat) value{
    if (_mPlayer) {
        // 先暂停
        [self fl_pause];
        NSTimeInterval newPos = value * self.mPlayer.duration;
        AliVcMovieErrorCode err = [_mPlayer seekTo:newPos];
        if(err != ALIVC_SUCCESS){
            return NO;
        }
        else{
            // 播放
            [self fl_play];
            return YES;
        }
    }
    return NO;
}

- (void)fl_fireTimer{
    [self fl_invalidateTimer];
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(playing) userInfo:nil repeats:YES];
    self.timer = timer;
    [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];
}

- (NSString *)fl_currentTimeWithSlideValue:(CGFloat)value{
    return  [self fl_stringWithTime:value *self.mPlayer.duration / 1000];
}

- (void)fl_monitorVideoDidPrepared:(void(^)(FLVideoPlayerManager *manager))preparedOperation playing:(void(^)(FLVideoPlayerManager *manager,CGFloat value))playingOperation didFinish:(void(^)(FLVideoPlayerManager *manager))finishOperation error:(void(^)(NSString *errorMsg))errorOperation{
    self.preparedOperation = preparedOperation;
    self.playingOperation = playingOperation;
    self.finishOperation = finishOperation;
    self.errorOperation = errorOperation;
}

- (void)playing{
    
    self.currentTime = [self fl_stringWithTime:_mPlayer.currentPosition / 1000];
    self.totalTime = [self fl_stringWithTime:_mPlayer.duration / 1000];
    
    NSLog(@"self.currentTime = %@,self.totalTime = %@",self.currentTime,self.totalTime);
    //    NSLog(@"self.currentTime = %@======%@",[self fl_stringWithTime:_mPlayer.currentPosition / 1000],[self fl_stringWithTime:[self fl_timeWithString:[self fl_stringWithTime:_mPlayer.currentPosition / 1000]]]);
    self.value = [self fl_timeWithString:self.currentTime] / [self fl_timeWithString:self.totalTime];
    if (self.playingOperation) {
        self.playingOperation(self,self.value);
    }
}

- (void)fl_invalidateTimer{
    if (self.timer) {
        [self.timer invalidate];
        self.timer = nil;
    }
}

- (void)dealloc{
    [self fl_removePlayerObserver];
    self.mUrl = nil;
    self.preparedOperation = nil;
    self.playingOperation = nil;
    self.finishOperation = nil;
    self.errorOperation = nil;
    self.isPrepared = NO;
    self.isFinished = NO;
    [self fl_invalidateTimer];
}

/**
 *  @author gitKong
 *
 *  时间显示转换
 */
- (NSString *)fl_stringWithTime:(NSTimeInterval)time{
    // Get the system calendar
    NSCalendar *sysCalendar = [NSCalendar currentCalendar];
    
    // Create the NSDates
    NSDate *date1 = [[NSDate alloc] init];
    NSDate *date2 = [[NSDate alloc] initWithTimeInterval:time sinceDate:date1];
    
    // Get conversion to months, days, hours, minutes
    unsigned int unitFlags = NSCalendarUnitHour | NSCalendarUnitMinute | NSCalendarUnitDay | NSCalendarUnitMonth | NSCalendarUnitSecond;
    
    NSDateComponents *breakdownInfo = [sysCalendar components:unitFlags fromDate:date1  toDate:date2  options:0];
    
    
    NSString *stringtime = [NSString stringWithFormat:@"%02ld:%02ld:%02ld", (long)[breakdownInfo hour], (long)[breakdownInfo minute], (long)[breakdownInfo second]];
    
    return stringtime;
}

- (NSTimeInterval)fl_timeWithString:(NSString *)timeString{
    NSArray *timeArr = [timeString componentsSeparatedByString:@":"];
    NSString *hourStr = timeArr.firstObject;
    NSString *minuteStr = timeArr[1];
    NSString *secondStr = timeArr.lastObject;
    
    return hourStr.integerValue * 3600 + minuteStr.integerValue * 60 + secondStr.integerValue;
    
}

/**
 *  @author gitKong
 *
 *  视频准备完成
 */
- (void)OnVideoPrepared:(NSNotification *)notification{
    NSAssert(_mPlayer, @"请先创建播放器");
    self.isPrepared = YES;
    self.fl_moviePlayerStatus = FLMoviePlayerStatusPrepared;
    self.currentTime = [self fl_stringWithTime:_mPlayer.currentPosition / 1000];
    self.totalTime = [self fl_stringWithTime:_mPlayer.duration / 1000];
    if (self.preparedOperation) {
        self.preparedOperation(self);
    }
}
/**
 *  @author gitKong
 *
 *  出现错误
 */
- (void)OnVideoError:(NSNotification *)notification{
    NSString* error_msg = @"未知错误";
    NSAssert(_mPlayer, @"请先创建播放器");
    
    // 销毁定时器
    [self fl_invalidateTimer];
    
    AliVcMovieErrorCode error_code = _mPlayer.errorCode;
    
    switch (error_code) {
        case ALIVC_ERR_FUNCTION_DENIED:
            error_msg = @"未授权";
            break;
        case ALIVC_ERR_ILLEGALSTATUS:
            error_msg = @"非法的播放流程";
            break;
        case ALIVC_ERR_INVALID_INPUTFILE:
            error_msg = @"无法打开";
            
            break;
        case ALIVC_ERR_NO_INPUTFILE:
            error_msg = @"无输入文件";
            
            break;
        case ALIVC_ERR_NO_NETWORK:
            error_msg = @"网络连接失败";
            break;
        case ALIVC_ERR_NO_SUPPORT_CODEC:
            error_msg = @"不支持的视频编码格式";
            
            break;
        case ALIVC_ERR_NO_VIEW:
            error_msg = @"无显示窗口";
            
            break;
        case ALIVC_ERR_NO_MEMORY:
            error_msg = @"内存不足";
            break;
        case ALIVC_ERR_DOWNLOAD_TIMEOUT:
            error_msg = @"网络超时";
            break;
        case ALIVC_ERR_UNKOWN:
            error_msg = @"未知错误";
            break;
        default:
            break;
    }
    
    NSLog(@"%@", error_msg);
    if (self.errorOperation) {
        self.errorOperation(_mPlayer ? error_msg : @"还没创建播放器");
    }
}

/**
 *  @author gitKong
 *
 *  调用stop/reset成功后播放视频结束,自动视频播放完毕也会调用
 */
- (void)OnVideoFinish:(NSNotification *)notification {
    
    NSAssert(_mPlayer, @"请先创建播放器");
    
    //    [self fl_stop];
    self.fl_moviePlayerStatus = FLMoviePlayerStatusFinished;
    
    self.isFinished = YES;
    
    [self fl_createVideo:self.mPlayerView url:self.mUrl shouldAutoplay:YES];
    
    //    self.currentTime = [self fl_stringWithTime:0];
    //    self.totalTime = [self fl_stringWithTime:_mPlayer.duration / 1000];
    
    
    if (self.finishOperation) {
        self.finishOperation(self);
    }
}
/**
 *  @author gitKong
 *
 *  跳转结束
 */
- (void)OnSeekDone:(NSNotification *)notification {
    
}

/**
 *  @author gitKong
 *
 *  开始缓冲
 */
- (void)OnStartCache:(NSNotification *)notification {
    
}

/**
 *  @author gitKong
 *
 *  结束缓冲
 */
- (void)OnEndCache:(NSNotification *)notification {
    
}

#pragma mark -- private method about notification

-(void)fl_addPlayerObserver{
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(OnVideoPrepared:)
                                                 name:AliVcMediaPlayerLoadDidPreparedNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(OnVideoError:)
                                                 name:AliVcMediaPlayerPlaybackErrorNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(OnVideoFinish:)
                                                 name:AliVcMediaPlayerPlaybackDidFinishNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(OnSeekDone:)
                                                 name:AliVcMediaPlayerSeekingDidFinishNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(OnStartCache:)
                                                 name:AliVcMediaPlayerStartCachingNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(OnEndCache:)
                                                 name:AliVcMediaPlayerEndCachingNotification object:_mPlayer];
}

-(void)fl_removePlayerObserver{
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:AliVcMediaPlayerLoadDidPreparedNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:AliVcMediaPlayerPlaybackErrorNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:AliVcMediaPlayerPlaybackDidFinishNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:AliVcMediaPlayerSeekingDidFinishNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:AliVcMediaPlayerStartCachingNotification object:_mPlayer];
    
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:AliVcMediaPlayerEndCachingNotification object:_mPlayer];
}

@end
