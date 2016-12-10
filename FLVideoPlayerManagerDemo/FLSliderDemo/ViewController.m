//
//  ViewController.m
//  FLSliderDemo
//
//  Created by clarence on 16/12/8.
//  Copyright © 2016年 gitKong. All rights reserved.
//

#import "ViewController.h"
#import "FLSlider.h"
#import <AliyunPlayerSDK/AliyunPlayerSDK.h>
#import "FLVideoPlayerManager.h"
@interface ViewController ()<FLSliderDelegate>
@property (nonatomic,strong)AliVcMediaPlayer *mPlayer;

@property (nonatomic,weak) IBOutlet FLSlider *slider;
@property (nonatomic,weak)UILabel *label;
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
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
    
    UILabel *label = [[UILabel alloc] initWithFrame:CGRectMake(100, 400, 300, 30)];
    label.textColor = [UIColor blueColor];
    self.label = label;
    self.label.text = [NSString stringWithFormat:@"%@/%@",@"00:00:00",@"00:00:00"];
    label.textAlignment = NSTextAlignmentCenter;
    [self.view addSubview:label];

    self.slider.backgroundColor = [UIColor grayColor];
    self.slider.delegate = self;
    self.slider.cacheValue = 0.5;
}


/**
 *  @author gitKong
 *
 *  开始拖动
 */
- (void)beginSlide:(UIButton *)sliderBtn slider:(FLSlider *)slider{
    [[FLVideoPlayerManager shareManager] fl_pause];
}
/**
 *  @author gitKong
 *
 *  正在拖动
 */
- (void)sliding:(UIButton *)sliderBtn slider:(FLSlider *)slider{
    NSString *current = [[FLVideoPlayerManager shareManager] fl_currentTimeWithSlideValue:slider.value];
    NSString *total = [FLVideoPlayerManager shareManager].totalTime;
    
    self.label.text = [NSString stringWithFormat:@"%@/%@",current,total];
    NSLog(@"text = %@",self.label.text);
}

/**
 *  @author gitKong
 *
 *  结束拖动
 */
- (void)endSlide:(UIButton *)sliderBtn slider:(FLSlider *)slider{
    [[FLVideoPlayerManager shareManager] fl_seekTo:slider.value];
}

@end
