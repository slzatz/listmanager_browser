#pragma once
#include <AppCore/AppCore.h>
#include "Tab.h"
#include <map>
#include <memory>

using ultralight::JSObject;
using ultralight::JSArgs;
using ultralight::JSFunction;
using namespace ultralight;

/**
* Browser UI implementation. Renders the toolbar/addressbar/tabs in top pane.
*/
class UI : public WindowListener,
           public LoadListener { //,
 public:
  UI(Ref<Window> window, std::string& ff);
  ~UI();
               
  // Inherited from WindowListener
  virtual void OnClose() override;
  virtual void OnResize(uint32_t width, uint32_t height) override;

  // Inherited from LoadListener
  virtual void OnDOMReady(View* caller,
                               uint64_t frame_id,
                               bool is_main_frame,
                               const String& url) override;

  // Called by UI JavaScript
  void OnBack(const JSObject& obj, const JSArgs& args);
  void OnForward(const JSObject& obj, const JSArgs& args);
  void OnRefresh(const JSObject& obj, const JSArgs& args);
  void OnStop(const JSObject& obj, const JSArgs& args);
  void OnToggleTools(const JSObject& obj, const JSArgs& args);
  void OnRequestChangeURL(const JSObject& obj, const JSArgs& args);
  void OnClickURL(const JSObject& obj, const JSArgs& args);
  void OnNumberLinks(const JSObject& obj, const JSArgs& args);
  void OnQuit(const JSObject& obj, const JSArgs& args);
  void OnScrollUp(const JSObject& obj, const JSArgs& args);
  void OnScrollDown(const JSObject& obj, const JSArgs& args);
  void OnScrollRight(const JSObject& obj, const JSArgs& args);
  void OnScrollLeft(const JSObject& obj, const JSArgs& args);
  void OnIncreaseZoom(const JSObject& obj, const JSArgs& args);
  void OnDecreaseZoom(const JSObject& obj, const JSArgs& args);
  //void OnNoteScroll(const JSObject& obj, const JSArgs& args);
  void OnNoteScroll(void);

  JSFunction updateURL;

  Ref<View> view() { return overlay_->view(); } //slz moved this into public so Browser could access
  std::unique_ptr<Tab> tab;
  RefPtr<Overlay> overlay_;
  double scroll = 0;
  double zoom = 1.0;

  void quit(void);

protected:
  void CreateNewTab();
  void UpdateTabURL(const String& url);
  void UpdateTabNavigation(bool is_loading, bool can_go_back, bool can_go_forward);
  void SetLoading(bool is_loading);
  void SetCanGoBack(bool can_go_back);
  void SetCanGoForward(bool can_go_forward);
  void SetURL(const String& url);
  void SetCursor(Cursor cursor);
  Ref<Window> window_;
  std::string file_;
  //RefPtr<Overlay> overlay_;
  int ui_height_;
  int tab_height_;
  float scale_;

  Cursor cur_cursor_;

  JSFunction updateBack;
  JSFunction updateForward;
  JSFunction updateLoading;
  //JSFunction updateURL;
  bool tab_created = false;

  friend class Tab;
};
