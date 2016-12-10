# 基于阿里云播放SDK封装的工具类
- 内含[自定义滑条](https://github.com/gitkong/FLSlider)，可监听开始滑动、正在滑动、结束滑动三个状态；工具类中的播放器SDK可随时替换

# 先看看效果图
![效果图](http://upload-images.jianshu.io/upload_images/1085031-1672aa5a25f5d43b.gif?imageMogr2/auto-orient/strip)

# 为什么要再封装？
- 1、降低项目对第三方SDK耦合，修改维护方便，作者项目中播放SDK本来是用IJKPlayer（哔哩哔哩开源的），后来换成阿里云SDK，只需要修改本工具类就可以
- 2、第三方SDK功能比较多，只需要将需要的功能性接口开放出来就行，阅读清晰

# API分析
- 1、旋转屏幕，视频播放中一般都需要进行屏幕旋转，此时应该属于播放器管理

```
/**
 *  @author 孔凡列
 *
 *  屏幕的状态
 */
@property (nonatomic,assign)FLMoviePlayerScreenStatus fl_moviePlayerScreenStatus;
```

- 2、当前播放状态，不管是哪个播放SDK，都需要随时拿到当前的播放状态，然后处理不同的情况
```
/**
 *  @author gitKong
 *
 *  当前播放状态
 */
@property (nonatomic,assign)FLMoviePlayerStatus fl_moviePlayerStatus;
```

- 3、当前的播放器，当然，考虑到一定的拓展性以及灵活性，当前创建的播放器可以获取，但不能修改，因此是readonly 修饰
```
/**
 *  @author gitKong
 *
 *  当前的播放器
 */
@property (nonatomic,strong,readonly)AliVcMediaPlayer *mPlayer;
```

- 4、进度（包括当前的进度时间、总进度时间以及进度比例）-注意，默认的时间进度格式是 00:00:00
```
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
```

- 5、单例模式创建，项目中的播放器唯一

```
/**
 *  @author 孔凡列
 *
 *  单例创建
 */
+ (instancetype)shareManager;
```

- 6、创建播放器，只需要传入播放器显示的frame、播放地址 就能返回一个播放器的view，添加到视图容器中就行，当然，只要调用这个方法创建，不管是不是自动开启，都会 进去准备播放状态

```
/**
 *  @author 孔凡列
 *
 *  创建播放器，内部已经prepare
 */
- (UIView *)fl_mediaPlayerView:(CGRect)frame URLString:(NSString *)urlString shouldAutoplay:(BOOL)autoPlay;
```

- 7、播放视频，此时内部已经做判断，如果没有准备好，先prepare，然后play，如果已经准备好，就直接play；可以通过返回值来判断是否播放成功

```
/**
 *  @author gitKong
 *
 *  没prepare，先prepare，在play
 */
- (BOOL)fl_play;
```

- 8、暂停播放，可以通过返回值来判断暂停是否成功

```
/**
 *  @author gitKong
 *
 *  暂停
 */
- (BOOL)fl_pause;
```

- 9、销毁播放器，销毁内部定时器、移除通知
```
/**
 *  @author 孔凡列
 *
 *  真正移除播放器
 */
- (BOOL)fl_removeMediaPlayer;
```

- 10、跳转到指定的播放位置，只需要传入比例就可以，一般传入slider的value值

```
/**
 *  @author gitKong
 *
 *  跳转到指定位置
 */
- (BOOL)fl_seekTo:(CGFloat) value;
```

- 11、滑动进度条，获取当前的时间，一般在正在拖动的方法实现，更新时间，格式是00:00:00

```
/**
 *  @author gitKong
 *
 *  滑动进度条，获取当前的时间，在正在拖动的方法实现，更新时间
 */
- (NSString *)fl_currentTimeWithSlideValue:(CGFloat)value;
```

- 12、监听播放状态，处理不同情况，一般SDK都是通过通知来监听，考虑到此时是通过单例管理，因此封装提供了四个block回调，已经准备好播放回调、正在播放回调、完成播放回调、出现错误回调

```
/**
 *  @author gitKong
 *
 *  监听状态
 */
- (void)fl_monitorVideoDidPrepared:(void(^)(FLVideoPlayerManager *manager))preparedOperation playing:(void(^)(FLVideoPlayerManager *manager,CGFloat value))playingOperation didFinish:(void(^)(FLVideoPlayerManager *manager))finishOperation error:(void(^)(NSString *errorMsg))errorOperation;
```

# 使用,一般只需要两步，创建-监听

```
/**
     *  @author gitKong
     *
     *  创建播放器
     */
    UIView *view = [[FLVideoPlayerManager shareManager] fl_mediaPlayerView:CGRectMake(0, 0, self.view.bounds.size.width, self.view.bounds.size.width * 9 / 16) URLString:@"http://vzan-input.oss-cn-hangzhou.aliyuncs.com/live/131254085448340233.1480942028.m3u8" shouldAutoplay:YES];
    [self.view addSubview:view];
    
    /**
     *  @author gitKong
     *
     *  监听播放状态
     */
    __weak typeof(self) weakSelf = self;
    [[FLVideoPlayerManager shareManager] fl_monitorVideoDidPrepared:^(FLVideoPlayerManager *manager) {
        weakSelf.label.text = [NSString stringWithFormat:@"%@/%@",manager.currentTime,manager.totalTime];
    } playing:^(FLVideoPlayerManager *manager,CGFloat value) {
        weakSelf.label.text = [NSString stringWithFormat:@"%@/%@",manager.currentTime,manager.totalTime];
        
        NSLog(@"value = %.2lf",value);
        [weakSelf.slider setValue:value];
    } didFinish:^(FLVideoPlayerManager *manager) {
        weakSelf.label.text = [NSString stringWithFormat:@"%@/%@",manager.currentTime,manager.totalTime];
        weakSelf.slider.value = 0.0;
    } error:^(NSString *errorMsg) {
        
    }];
```

# 内置自定义slider
- 为什么要自定义slider？[点我前往](https://github.com/gitkong/FLSlider)

# 喜欢给我个Star，欢迎关注[我的简书](http://www.jianshu.com/users/fe5700cfb223/latest_articles)，更多原创干货等着你
