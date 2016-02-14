#include "AppDelegate.h"
#include "StartupScene.h"
#include "SimpleAudioEngine.h"
#include "Config.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("Pan-club SlotGame");
        director->setOpenGLView(glview);
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	glview->setFrameSize(540, 960);
#endif

	// デザイン解像度を720x1280に設定
	// 720x1280より小さい解像度はCocos2dxが自動で対応。これより大きい解像度については、必要があれば対応する
	glview->setDesignResolutionSize(config::resolution::DESIGN_RESOLUTION_SHORT_SPAN, config::resolution::DESIGN_RESOLUTION_LONG_SPAN, config::resolution::DESIGN_POLICY);

	// オーディオファイルディレクトを追加
	FileUtils::getInstance()->addSearchPath("audio");

#if defined(COCOS2D_DEBUG)
    // turn on display FPS
    director->setDisplayStats(true);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    auto scene = StartupScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationScreenSizeChanged(int w, int h)
{
	// スクリーンの大きさと共に、カスタムイベントを通知
	auto data = ValueMap();
	data["w"] = w;
	data["h"] = h;
	auto evtDispatcher = Director::getInstance()->getEventDispatcher();
	evtDispatcher->dispatchCustomEvent(config::event::OrientationChangedNotification, &data);
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
}
