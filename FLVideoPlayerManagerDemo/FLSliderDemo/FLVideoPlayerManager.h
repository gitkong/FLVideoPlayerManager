/*
 * author 孔凡列
 *
 * gitHub https://github.com/gitkong
 * cocoaChina http://code.cocoachina.com/user/
 * 简书 http://www.jianshu.com/users/fe5700cfb223/latest_articles
 * QQ 279761135
 * 微信公众号 原创技术分享
 * 喜欢就给个like 和 star 喔~
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AliyunPlayerSDK/AliyunPlayerSDK.h>
/**
 *  @author 孔凡列
 *
 *  当前屏幕的状态，横屏 | 竖屏
 */
typedef NS_ENUM(NSInteger,FLMoviePlayerScreenStatus){
    FLMoviePlayerScreenStatusProtrait,
    FLMoviePlayerScreenStatusLandscape
};
/**
 *  @author gitKong
 *
 *  当前播放状态
 */
typedef NS_ENUM(NSInteger,FLMoviePlayerStatus){
    FLMoviePlayerStatusNonePrepared,// 没准备
    FLMoviePlayerStatusPrepared,// 准备好
    FLMoviePlayerStatusPlaying,// 正在播放
    FLMoviePlayerStatusPaused,// 暂停
    FLMoviePlayerStatusFinished// 结束
};

@interface FLVideoPlayerManager : NSObject
/**
 *  @author 孔凡列
 *
 *  屏幕的状态
 */
@property (nonatomic,assign)FLMoviePlayerScreenStatus fl_moviePlayerScreenStatus;
/**
 *  @author gitKong
 *
 *  当前播放状态
 */
@property (nonatomic,assign)FLMoviePlayerStatus fl_moviePlayerStatus;

/**
 *  @author gitKong
 *
 *  当前的播放器
 */
@property (nonatomic,strong,readonly)AliVcMediaPlayer *mPlayer;
/**
 *  @author gitKong
 *
 *  当前进度时间，格式：00:00:00
 */
@property (nonatomic,copy,readonly)NSString *currentTime;
/**
 *  @author gitKong
 *
 *  总时间，格式：00:00:00
 */
@property (nonatomic,copy,readonly)NSString *totalTime;
/**
 *  @author gitKong
 *
 *  正在播放的进度比例
 */
@property (nonatomic,assign,readonly)CGFloat value;


/**
 *  @author 孔凡列
 *
 *  单例创建
 */
+ (instancetype)shareManager;

/**
 *  @author 孔凡列
 *
 *  创建播放器，内部已经prepare
 */
- (UIView *)fl_mediaPlayerView:(CGRect)frame URLString:(NSString *)urlString shouldAutoplay:(BOOL)autoPlay;
/**
 *  @author gitKong
 *
 *  暂停
 */
- (BOOL)fl_pause;

/**
 *  @author gitKong
 *
 *  没prepare，先prepare，在play
 */
- (BOOL)fl_play;

/**
 *  @author 孔凡列
 *
 *  真正移除播放器
 */
- (BOOL)fl_removeMediaPlayer;

/**
 *  @author gitKong
 *
 *  跳转到指定位置
 */
- (BOOL)fl_seekTo:(CGFloat) value;

/**
 *  @author gitKong
 *
 *  滑动进度条，获取当前的时间，在正在拖动的方法实现，更新时间
 */
- (NSString *)fl_currentTimeWithSlideValue:(CGFloat)value;

/**
 *  @author gitKong
 *
 *  监听状态
 */
- (void)fl_monitorVideoDidPrepared:(void(^)(FLVideoPlayerManager *manager))preparedOperation playing:(void(^)(FLVideoPlayerManager *manager,CGFloat value))playingOperation didFinish:(void(^)(FLVideoPlayerManager *manager))finishOperation error:(void(^)(NSString *errorMsg))errorOperation;



@end
