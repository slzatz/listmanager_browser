#include "Browser.h"
#include <Ultralight/platform/Platform.h>
#include <Ultralight/platform/Config.h>
#include <Ultralight/Renderer.h>
#include <AppCore/Platform.h> /// for Config which doesn't seem to do anything
#include <Ultralight/Ultralight.h> ///ditto
#include <zmq.hpp>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>

zmq::context_t context(1);
zmq::socket_t subscriber(context, ZMQ_SUB);

struct flock lock;

Browser::Browser(char *f_) {
  app_ = App::Create();
  window_ = Window::Create(app_->main_monitor(), 500, 500, false, 
    kWindowFlags_Resizable | kWindowFlags_Titled | kWindowFlags_Maximizable);

  /*
  Config config;
  config.enable_javascript = true;
  config.device_scale = window_->scale();
 // config.scroll_timer_delay = 1.0 / (ultralight::mode->refreshRate);
 // config.animation_timer_delay = 1.0 / (ultralight::mode->refreshRate);
  //config.use_gpu_renderer = true;
  //config.resource_path = "./resources/";
  config.cache_path = "/home/slzatz/.cache/listmanager";
  Platform::instance().set_config(config);
  */

  subscriber.connect("tcp://localhost:5556");
  subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0);

  /* variables to monitor changes in current.html */ 
  ff = "file:///";
  ff = ff + f_;
  fff = "assets/";
  fff = fff + f_;
  lwt = std::filesystem::last_write_time(fff);
  prev_lwt = lwt;
  /* variables to monitor changes in current.html */ 

  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
  lock.l_pid = getpid();

  //app_ = App::Create();
    
  //window_ = Window::Create(app_->main_monitor(), 1024, 768, false, 
  //window_ = Window::Create(app_->main_monitor(), 500, 500, false, 
  //  kWindowFlags_Resizable | kWindowFlags_Titled | kWindowFlags_Maximizable);

  //window_->SetTitle("Ultralight Sample - Browser");

  app_->set_window(*window_.get());
    
  // Create the UI
  //ui_.reset(new UI(*window_.get()));
  ui_ = std::unique_ptr<UI>(new UI(*window_.get(), ff));
  window_->set_listener(ui_.get());
  app_->set_listener(this); 
}

Browser::~Browser() {
  window_->set_listener(nullptr);

  ui_.reset();

  window_ = nullptr;
  app_ = nullptr;
}

/* Below is what detects changes to current.html */ 
void Browser::OnUpdate() {
  lwt = std::filesystem::last_write_time(fff);
  if (lwt != prev_lwt) {
    //prev_lwt = lwt;
    int fd;
    if ((fd = open(fff.c_str(), O_RDONLY)) != -1) {
      lock.l_type = F_RDLCK;
      if (fcntl(fd, F_SETLK, &lock) != -1) {
        ui_->tab->view()->LoadURL(ff.c_str());
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        prev_lwt = lwt;
      }
    }
  }
  ui_->overlay_->Focus();
  //zeromq stuff
  zmq::message_t update;
  auto result = subscriber.recv(update, zmq::recv_flags::dontwait);
  if (result)  {
    char *scroll = static_cast<char*>(update.data());
    char s[50];
    snprintf (s, 50, "scroll: %s", scroll);
    ui_->updateURL({s}); //for debugging - using address bar to print results
    ui_->scroll = atoi(scroll);
    ui_->OnNoteScroll();
  }
}

void Browser::Run() {
  app_->Run();
}
