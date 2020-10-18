#pragma once
#include <AppCore/AppCore.h>
#include <Ultralight/Listener.h>
#include <string>

class UI; //forward declaration
using namespace ultralight;

class Tab : public ViewListener,
            public LoadListener {
public:
  Tab(UI* ui, uint32_t width, uint32_t height, int x, int y);
  ~Tab();

  void set_ready_to_close(bool ready) { ready_to_close_ = ready; }
  bool ready_to_close() { return ready_to_close_; }
                
  Ref<View> view() { return overlay_->view(); }

  void Resize(int width, int height);

  // Inherited from Listener::View
  virtual void OnChangeURL(View* caller, const String& url) override;
  virtual void OnChangeCursor(View* caller, Cursor cursor) override;

  // Inherited from Listener::Load
  virtual void OnBeginLoading(View* caller,
                               uint64_t frame_id,
                               bool is_main_frame,
                               const String& url) override;

  virtual void OnFinishLoading(View* caller,
                               uint64_t frame_id,
                               bool is_main_frame,
                               const String& url) override;

  virtual void OnUpdateHistory(View* caller) override;

  virtual void OnDOMReady(View* caller,
                               uint64_t frame_id,
                               bool is_main_frame,
                               const String& url) override;

  void OnScroll(const JSObject& obj, const JSArgs& args);

protected:
  UI* ui_;
  RefPtr<Overlay> overlay_;
  bool ready_to_close_ = false;
  uint32_t container_width_, container_height_;
  std::string title = "";
  std::string prev_title = "";
};
