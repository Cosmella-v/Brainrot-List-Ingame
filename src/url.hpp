#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <thread>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
using namespace geode::prelude;
#include <iostream>
#include <string>

#if defined(webview)
#include <webview/webview.h>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <CoreGraphics/CoreGraphics.h>
#include <objc/objc-runtime.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
class WebviewUrl {
    public:
    inline static std::string ConvertToEmbed(std::string url) {
        if (url.find("https://youtu.be/") == 0) {
            std::string videoId = url.substr(17);  
            return "https://www.youtube.com/embed/" + videoId+"?rel=0&autoplay=1";
        }
         if (url.find("youtu.be/") == 0) {
            std::string videoId = url.substr(9);  
            return "https://www.youtube.com/embed/" + videoId+"?rel=0&autoplay=1";
        }
        if (url.find("www.youtube.com/watch?v=") == 0) {
            std::string videoId = url.substr(24); 
            return "https://www.youtube.com/embed/" + videoId;
        }
        if (url.find("https://www.youtube.com/watch?v=") == 0) {
            std::string videoId = url.substr(32);  
            return "https://www.youtube.com/embed/" + videoId;
        }
        return url;
    }
    inline static void Open(std::string url, std::string Name) {
        try {
            auto winSize = WebviewUrl::getWindowSizeRealSize();
            webview::webview w(true, nullptr); 
            w.set_title(Name); 
            w.set_size(winSize.first / 1.1, winSize.second / 1.1, WEBVIEW_HINT_NONE );
            w.navigate(url); 
            auto wid = w.window();
            if (wid.has_value()) {
                set_window_props(wid.value());
            }
            w.run();
        } catch (const std::exception& ex) {/*gotta break me sometimes*/}
    };
    inline static std::pair<float, float> getWindowSizeRealSize() {
        float width = 0.0f;
        float height = 0.0f;
        
        #if defined(_WIN32)
            RECT rect;
            HWND hwnd = GetActiveWindow();
            if (GetClientRect(hwnd, &rect)) {
                width = static_cast<float>(rect.right - rect.left);
                height = static_cast<float>(rect.bottom - rect.top);
            }
        #elif defined(__APPLE__) && !defined(TARGET_OS_IPHONE)
            CGRect screenRect = CGDisplayBounds(CGMainDisplayID());
            width = screenRect.size.width;
            height = screenRect.size.height;
        #elif defined(TARGET_OS_IPHONE)
            CGRect screenRect = [[UIScreen mainScreen] bounds];
            width = screenRect.size.width;
            height = screenRect.size.height;
        #elif defined(__linux__) && !defined(__ANDROID__)
            Display* dpy = XOpenDisplay(nullptr);
            if (dpy) {
                Window root = DefaultRootWindow(dpy);
                XWindowAttributes windowAttributes;
                XGetWindowAttributes(dpy, root, &windowAttributes);
                width = static_cast<float>(windowAttributes.width);
                height = static_cast<float>(windowAttributes.height);
                XCloseDisplay(dpy);
            }
        #elif defined(__ANDROID__)
            JNIEnv* env = nullptr; // Obtain the JNI environment
            jobject activity = nullptr; // Reference to the Android activity
            jclass activityClass = env->GetObjectClass(activity);
            jmethodID getWindowManager = env->GetMethodID(activityClass, "getWindowManager", "()Landroid/view/WindowManager;");
            jobject windowManager = env->CallObjectMethod(activity, getWindowManager);
            jclass windowManagerClass = env->GetObjectClass(windowManager);
            jmethodID getDefaultDisplay = env->GetMethodID(windowManagerClass, "getDefaultDisplay", "()Landroid/view/Display;");
            jobject display = env->CallObjectMethod(windowManager, getDefaultDisplay);
            jclass displayClass = env->GetObjectClass(display);
            jmethodID getSize = env->GetMethodID(displayClass, "getSize", "(Landroid/graphics/Point;)V");
            jobject size = env->AllocObject(env->FindClass("android/graphics/Point"));
            env->CallVoidMethod(display, getSize, size);
            jclass pointClass = env->GetObjectClass(size);
            jfieldID xField = env->GetFieldID(pointClass, "x", "I");
            jfieldID yField = env->GetFieldID(pointClass, "y", "I");
            width = static_cast<float>(env->GetIntField(size, xField));
            height = static_cast<float>(env->GetIntField(size, yField));
            env->DeleteLocalRef(size);
        #endif

        return {width, height};
    }
    private:
static void set_window_props(void* window) {
    #if defined(_WIN32)
        HWND hwnd = static_cast<HWND>(window);
        if (hwnd != NULL) {
            LONG style = GetWindowLong(hwnd, GWL_STYLE);
            style |= WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION; // Include system menu and close button
            style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX); // Remove resizable border and maximize button
            SetWindowLong(hwnd, GWL_STYLE, style);
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED);
        }
    #endif
}

};
#else
class WebviewUrl {
    public:
    inline static std::string ConvertToEmbed(std::string url) {
        return url;
    }
    inline static void Open(std::string url, std::string Name) {
        geode::utils::web::openLinkInBrowser(url);
    };
};
#endif;

static std::map<int,matjson::Value> level_map;

inline void RequestApi(bool retry,std::string Url, std::function<void(matjson::Value)> fun,std::function<void()> onfail) {
      web::WebRequest().get(Url).listen(
        [=](auto getval) {
            auto json = getval->json().unwrapOr("failed");
            if (json == "failed") {
                //log::error("Failed at {}",Url);
                if (retry) {
                     RequestApi(retry,Url,fun,onfail);
                } else {
                    if (!onfail) {
                        return;
                    } else {
                        onfail();
                    }
                }
               return;
            }
            fun(json); 
        },
        [](auto prog) {
            
        },
        [=]() {
             if (retry) {
                     RequestApi(retry,Url,fun,onfail);
                } else {
                    if (!onfail) {
                        return;
                    } else {
                        onfail();
                    }
                }
            //log::warn("Request was cancelled.");
        }
    );
}

inline static void getlistjson(std::function<void(matjson::Value)> fun,std::function<void()> failed) {
   //log::debug("should send https://br-list.pages.dev/data/_list.json");
    RequestApi(false,"https://br-list.pages.dev/data/_list.json", fun,failed);
}

inline static void getpackjson(std::function<void(matjson::Value)> fun,std::function<void()> failed) {
    RequestApi(false,"https://br-list.pages.dev/data/_packlist.json", fun,failed);
}

inline static void getleveljson(const std::string& name, std::function<void(matjson::Value)> fun) {
    std::string finalurl;
    for (char c : name) {
        if (c == ' ') {
            finalurl += "%20"; 
        } else {
            finalurl += c; 
        }
    }
    //log::debug("getting info for {}",name);
    RequestApi(true,"https://br-list.pages.dev/data/" + finalurl + ".json", fun,nullptr);
}
