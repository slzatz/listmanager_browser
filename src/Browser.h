#include <AppCore/AppCore.h>
#include <string>
#include <filesystem>
#include <chrono>
#include "UI.h"

using namespace ultralight;

class Browser : public AppListener {
public:
  Browser(char *f_);
  virtual ~Browser();
    
  virtual void Run();

  //adding here slz
  virtual void OnUpdate(); 

protected:
  RefPtr<App> app_;
  RefPtr<Window> window_;
  std::unique_ptr<UI> ui_;
  std::string ff, fff;
  std::chrono::time_point<std::chrono::file_clock> lwt, prev_lwt;
};
