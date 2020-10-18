#include "Tab.h"
#include "UI.h"
#include <sstream>

#define INSPECTOR_HEIGHT 300

Tab::Tab(UI* ui, uint32_t width, uint32_t height, int x, int y) 
  : ui_(ui), container_width_(width), container_height_(height) {
  overlay_ = Overlay::Create(ui->window_, width, height, x, y);
  view()->set_view_listener(this);
  view()->set_load_listener(this);
}

Tab::~Tab() {
  view()->set_view_listener(nullptr);
  view()->set_load_listener(nullptr);
}

void Tab::Resize(int width, int height) {
  overlay_->Resize(width, height);
}

// triggered by ViewListener
void Tab::OnChangeURL(View* caller, const String& url) {
  ui_->UpdateTabURL(url);
}

// triggered by ViewListener
void Tab::OnChangeCursor(View* caller, Cursor cursor) {
    ui_->SetCursor(cursor);
}

// triggered by LoadListener
void Tab::OnBeginLoading(View* caller,
                         uint64_t frame_id,
                         bool is_main_frame,
                         const String& url) {
  ui_->UpdateTabNavigation(caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

// triggered by LoadListener
void Tab::OnFinishLoading(View* caller,
                          uint64_t frame_id,
                          bool is_main_frame,
                          const String& url) {
  ui_->UpdateTabNavigation(caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

// triggered by LoadListener
void Tab::OnUpdateHistory(View* caller) {
  ui_->UpdateTabNavigation(caller->is_loading(), caller->CanGoBack(), caller->CanGoForward());
}

// triggered by LoadListener
void Tab::OnDOMReady(View* caller,
                     uint64_t frame_id,
                     bool is_main_frame,
                     const String& url) {
  Ref<JSContext> context = view()->LockJSContext();
  SetJSContext(context.get());

 // SetJSContext(view()->js_context());
  JSObject global = JSGlobalObject(); // equivalent to "window" object
  //Javascript -> C++ (detects scroll of tab with scrollbars)
  global["OnScroll"] = BindJSCallback(&Tab::OnScroll);

  // could we check if title has changed?
  std::string title;
  //JSValueRef result = view()->EvaluateScript("document.title");
  String str = view()->EvaluateScript("document.title");
  //JSStringRef str =JSValueToStringCopy(view()->js_context(), result, nullptr);
  //size_t len = JSStringGetMaximumUTF8CStringSize(str);
  //char* buffer = new char[len];
  //JSStringGetUTF8CString(str, buffer, len);
  //title = std::string(buffer, len);
  title = std::string(str.utf8().data());
  //delete[] buffer;
  //JSStringRelease(str);
  if (title == prev_title) {
    char js[50];
    // ? scrollTo not scrollBy if on reload scroll = 0 then they are equiv
    snprintf(js, sizeof(js), "window.scrollBy(0,%d)", (int)ui_->scroll);
    view()->EvaluateScript(js); 
  } else {
    //view()->EvaluateScript("window.scrollBy(0,0)"); 
    ui_->scroll = 0;
    prev_title = title;
  }
  // without this there is no zoom on saving or loading another note
  char js[50];
  snprintf(js, sizeof(js), "document.body.style.zoom = %f", ui_->zoom);
  view()->EvaluateScript(js);
}

// listener is in tab.js
void Tab::OnScroll(const JSObject& obj, const JSArgs& args) {
  //JSValueRef result = view()->EvaluateScript("window.pageYOffset");
  String result = view()->EvaluateScript("window.pageYOffset");
  std::string scroll(result.utf8().data());
  ui_->scroll = stoi(scroll);  
  //ui_->scroll = JSValueToNumber(view()->js_context(), result, 0); 
  //
  //for debugging
  std::stringstream s;
  s << "Scroll:" << ui_->scroll;
  ui_->updateURL({s.str().c_str()}); //for debugging - using address bar to print results
}
