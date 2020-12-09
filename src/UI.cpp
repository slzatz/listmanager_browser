#include <sstream>
#include "UI.h"

#define UI_HEIGHT 40 //was 80 but removed the tabs

// seems like UI should have file name so its UI(Ref<Window> window, std::string &ff) window_(window) file_(ff) {
//UI::UI(Ref<Window> window) : window_(window) {
UI::UI(Ref<Window> window, std::string &ff) : window_(window), file_(ff) {
  uint32_t window_width = App::instance()->window()->width();
  overlay_ = Overlay::Create(window_, window_width, UI_HEIGHT, 0, 0);
  overlay_->Focus();

  view()->set_load_listener(this);
  view()->LoadURL("file:///ui.html");
}

UI::~UI() {
  view()->set_load_listener(nullptr);
}

void UI::OnClose() {
}

void UI::OnResize(uint32_t width, uint32_t height) {
  RefPtr<Window> window = App::instance()->window();

  int tab_height = window->height() - UI_HEIGHT;

  if (tab_height < 1)
    tab_height = 1;

  overlay_->Resize(window->width(), UI_HEIGHT);

  if (tab_created) {
  tab->Resize(window->width(), tab_height);
  }
}

void UI::OnDOMReady(ultralight::View* caller,
                    uint64_t frame_id,
                    bool is_main_frame,
                    const String& url) {

  //CreateNewTab();
  //tab_created = true;
  // Set the context for all subsequent JS* calls
  //SetJSContext(view()->js_context());
  Ref<JSContext> context = view()->LockJSContext();
  SetJSContext(context.get());

  JSObject global = JSGlobalObject(); // equivalent to "window" object

  // C++ to javascript (call javascript to affect appearance of UI overlay)
  // triggered by tab view listener
  //javascript functions below in ui.js 
  updateBack = global["updateBack"];
  updateForward = global["updateForward"];
  updateLoading = global["updateLoading"];
  updateURL = global["updateURL"];

  //Javascript -> C++ (mainly to manage tab overlay)
  global["OnBack"] = BindJSCallback(&UI::OnBack);
  global["OnForward"] = BindJSCallback(&UI::OnForward);
  global["OnRefresh"] = BindJSCallback(&UI::OnRefresh);
  global["OnStop"] = BindJSCallback(&UI::OnStop);
  global["OnRequestChangeURL"] = BindJSCallback(&UI::OnRequestChangeURL);
  global["OnClickURL"] = BindJSCallback(&UI::OnClickURL);
  global["OnNumberLinks"] = BindJSCallback(&UI::OnNumberLinks);
  global["OnScrollUp"] = BindJSCallback(&UI::OnScrollUp);
  global["OnScrollDown"] = BindJSCallback(&UI::OnScrollDown);
  global["OnScrollRight"] = BindJSCallback(&UI::OnScrollRight);
  global["OnScrollLeft"] = BindJSCallback(&UI::OnScrollLeft);
  global["OnIncreaseZoom"] = BindJSCallback(&UI::OnIncreaseZoom);
  global["OnDecreaseZoom"] = BindJSCallback(&UI::OnDecreaseZoom);
  //global["OnNoteScroll"] = BindJSCallback(&UI::OnNoteScroll);
  global["OnQuit"] = BindJSCallback(&UI::OnQuit);

  //might want to put this into OnWindowObjectReady (fires before OnDOMReady
  // see https://docs.ultralig.ht/docs/using-the-ondomready-event
  CreateNewTab(); //might want to put this into OnWindowObjectReady (fires before OnDOMReady
  tab_created = true;
}

void UI::OnBack(const JSObject& obj, const JSArgs& args) {
  tab->view()->GoBack();
}

void UI::OnForward(const JSObject& obj, const JSArgs& args) {
  tab->view()->GoForward();
}

void UI::OnRefresh(const JSObject& obj, const JSArgs& args) {
  tab->view()->Reload();
}

void UI::OnStop(const JSObject& obj, const JSArgs& args) {
  tab->view()->Stop();
}

void UI::OnScrollUp(const JSObject& obj, const JSArgs& args) {
  tab->view()->EvaluateScript("window.scrollBy(0,-50)");
  //JSValueRef result = tab->view()->EvaluateScript("window.pageYOffset");
  String result = tab->view()->EvaluateScript("window.pageYOffset");
  //scroll = JSValueToNumber(tab->view()->js_context(), result, 0); 
  scroll = std::stoi(result.utf8().data());

  //for debugging
  std::stringstream s;
  s << "Scroll:" << scroll;
  updateURL({s.str().c_str()}); //for debugging - using address bar to print results
}

//void UI::OnNoteScroll(const JSObject& obj, const JSArgs& args) {
void UI::OnNoteScroll(void) {
  char js[50];
  snprintf(js, sizeof(js), "window.scrollTo(0,%d)", (int)scroll);
  tab->view()->EvaluateScript(js); 
}

/* for many of the functions below that just call a javascript function
 * it would seem like you could just stay in javascript
 * but the problem is we're detecting in UI overlay and then
 * causing an action in the Tab overal.
 * Maybe all the keystroke detection should take place in Tab
 * overlay but the below has the virtue of actually working
 */

void UI::OnScrollDown(const JSObject& obj, const JSArgs& args) {
  tab->view()->EvaluateScript("window.scrollBy(0,50)");
  //JSValueRef result = tab->view()->EvaluateScript("window.pageYOffset");
  String result = tab->view()->EvaluateScript("window.pageYOffset");
  //scroll = JSValueToNumber(tab->view()->js_context(), result, 0); 
  scroll = std::stoi(result.utf8().data());

  //for debugging
  std::stringstream s;
  s << "Scroll:" << scroll;
  updateURL({s.str().c_str()}); //for debugging - using address bar to print results
}

void UI::OnScrollLeft(const JSObject& obj, const JSArgs& args) {
  tab->view()->EvaluateScript("window.scrollBy(-25,0)");
}

void UI::OnScrollRight(const JSObject& obj, const JSArgs& args) {
  tab->view()->EvaluateScript("window.scrollBy(25,0)");
}

void UI::OnIncreaseZoom(const JSObject& obj, const JSArgs& args) {
  zoom += 0.25;
  char js[50];
  snprintf(js, sizeof(js), "document.body.style.zoom = %f", zoom);
  tab->view()->EvaluateScript(js);
  //JSValueRef result = tab->view()->EvaluateScript("document.body.style.zoom");
  String result = tab->view()->EvaluateScript("document.body.style.zoom");
  //double zoom = JSValueToNumber(tab->view()->js_context(), result, 0); 
  double zoom = 1.0 * std::stoi(result.utf8().data());

  //for debugging
  std::stringstream s;
  s << "zoom:" << zoom;
  updateURL({s.str().c_str()}); //for debugging - using address bar to print results
}

void UI::OnDecreaseZoom(const JSObject& obj, const JSArgs& args) {
  zoom -= 0.25;
  char js[50];
  snprintf(js, sizeof(js), "document.body.style.zoom = %f", zoom);
  tab->view()->EvaluateScript(js);
  //JSValueRef result = tab->view()->EvaluateScript("document.body.style.zoom");
  String result = tab->view()->EvaluateScript("document.body.style.zoom");
  //double zoom = JSValueToNumber(tab->view()->js_context(), result, 0); 
  double zoom = 1.0 * std::stoi(result.utf8().data());

  //for debugging
  std::stringstream s;
  s << "zoom:" << zoom;
  updateURL({s.str().c_str()}); //for debugging - using address bar to print results
}

void UI::OnRequestChangeURL(const JSObject& obj, const JSArgs& args) {
  if (args.size() == 1) {
    ultralight::String url = args[0];

    tab->view()->LoadURL(url);
  }
}

void UI::OnClickURL(const JSObject& obj, const JSArgs& args) {
  /*
  ultralight::String url;
  if (args.size() == 1) 
    url = args[0];
  */

  int x = args[0].ToInteger(); //works - args[0] JSValue
  x -= 48;
  std::stringstream s;
  s << "document.getElementById('" << x << "').click()";
  updateURL({s.str().c_str()}); //for debugging - using address bar to print results
  tab->view()->EvaluateScript(s.str().c_str());
}      

void::UI::OnNumberLinks(const JSObject& obj, const JSArgs& args) {
  /* note we don't use i but use link[i].id because
   * markdown mkd_e_flags treats images as links to
   * so numbering could be off using i.
   */

  std::stringstream s;
  s << "var links = document.getElementsByTagName('a');"
    << "for (var i=0; i< links.length; i++) {"
    << "var link_text = links[i].innerHTML;"
    << "var id_ = links[i].id;"
    << "links[i].innerHTML = id_ + '. ' + link_text;}";

  tab->view()->EvaluateScript(s.str().c_str());
}

void UI::OnQuit(const JSObject& obj, const JSArgs& args) {
  App::instance()->window()->Close();
  App::instance()->Quit();
}      

void UI::quit(void) {
  App::instance()->window()->Close();
  App::instance()->Quit();
}      
void UI::CreateNewTab() {
  RefPtr<Window> window = App::instance()->window();
  int tab_height = window->height() - UI_HEIGHT;
  if (tab_height < 1)
    tab_height = 1;

  tab = std::unique_ptr<Tab>(new Tab(this, window->width(), (uint32_t)tab_height, 0, UI_HEIGHT));
  //tab->view()->LoadURL("file:///current.html");
  tab->view()->LoadURL(file_.c_str());
}

void UI::UpdateTabURL(const ultralight::String& url) {
    SetURL(url);
}

void UI::UpdateTabNavigation(bool is_loading, bool can_go_back, bool can_go_forward) {
  //updateTab({tab->view()->title(), "", tab->view()->is_loading() });
  SetLoading(is_loading);
  SetCanGoBack(can_go_back);
  SetCanGoForward(can_go_forward);
}

void UI::SetLoading(bool is_loading) {
  updateLoading({ is_loading });
}

void UI::SetCanGoBack(bool can_go_back) {
  updateBack({ can_go_back });
}

void UI::SetCanGoForward(bool can_go_forward) {
  updateForward({ can_go_forward });
}

void UI::SetURL(const ultralight::String& url) {
  updateURL({ url });
}

void UI::SetCursor(ultralight::Cursor cursor) {
  if (App::instance())
    App::instance()->window()->SetCursor(cursor);
}
