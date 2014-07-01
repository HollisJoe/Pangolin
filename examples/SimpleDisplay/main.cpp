#include <iostream>

#include <pangolin/pangolin.h>

using namespace pangolin;
using namespace std;

struct CustomType
{
  CustomType()
    : x(0), y(0.0f) {}

  CustomType(int x, float y, string z)
    : x(x), y(y), z(z) {}

  int x;
  float y;
  string z;
};

std::ostream& operator<< (std::ostream& os, const CustomType& o){
  os << o.x << " " << o.y << " " << o.z;
  return os;
}

std::istream& operator>> (std::istream& is, CustomType& o){
  is >> o.x;
  is >> o.y;
  is >> o.z;
  return is;
}

void GlobalKeyHook()
{
    cout << "You pushed ctrl-r" << endl;
}

int main( int /*argc*/, char* argv[] )
{  
  // Load configuration data
  pangolin::ParseVarsFile("app.cfg");

  // Create OpenGL window in single line thanks to GLUT
  pangolin::CreateWindowAndBind("Main",640,480);
  
  // 3D Mouse handler requires depth testing to be enabled
  glEnable(GL_DEPTH_TEST);
  
  // Issue specific OpenGl we might need
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  // Define Camera Render Object (for view / scene browsing)
  pangolin::OpenGlRenderState s_cam(
    ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
    ModelViewLookAt(-0,0.5,-3, 0,0,0, AxisY)
  );

  const int UI_WIDTH = 180;

  // Add named OpenGL viewport to window and provide 3D Handler
  View& d_cam = pangolin::CreateDisplay()
    .SetBounds(0.0, 1.0, Attach::Pix(UI_WIDTH), 1.0, -640.0f/480.0f)
    .SetHandler(new Handler3D(s_cam));

  // Add named Panel and bind to variables beginning 'ui'
  // A Panel is just a View with a default layout and input handling
  pangolin::CreatePanel("ui")
      .SetBounds(0.0, 1.0, 0.0, Attach::Pix(UI_WIDTH));

  // Demonstration of how we can register a keyboard hook to alter a Var
  pangolin::RegisterKeyPressCallback( PANGO_CTRL + 'b', SetVarFunctor<double>("ui.A Double", 3.5) );

  // Demonstration of how we can register a keyboard hook to trigger a method
  pangolin::RegisterKeyPressCallback( PANGO_CTRL + 'r', GlobalKeyHook );

  // Safe and efficient binding of named variables.
  // Specialisations mean no conversions take place for exact types
  // and conversions between scalar types are cheap.
  Var<bool> a_button("ui.A Button",false,false);
  Var<double> a_double("ui.A Double",3,0,5);
  Var<int> an_int("ui.An Int",2,0,5);
  Var<double> a_double_log("ui.Log scale var",3,1,1E4, true);
  Var<bool> a_checkbox("ui.A Checkbox",false,true);
  Var<int> an_int_no_input("ui.An Int No Input",2);
  Var<CustomType> any_type("ui.Some Type", CustomType(0,1.2,"Hello") );

  Var<bool> save_window("ui.Save Window",false,false);
  Var<bool> save_teapot("ui.Save Teapot",false,false);

  Var<bool> record_teapot("ui.Record Teapot",false,false);

  // Default hooks for exiting (Esc) and fullscreen (tab).
  while( !pangolin::ShouldQuit() )
  {
    // Clear entire screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

    if( Pushed(a_button) )
      cout << "You Pushed a button!" << endl;

    // Overloading of Var<T> operators allows us to treat them like
    // their wrapped types, eg:
    if( a_checkbox )
      an_int = a_double;

    if( !any_type->z.compare("robot"))
        any_type = CustomType(1,2.3,"Boogie");

    an_int_no_input = an_int;

    if( Pushed(save_window) )
        pangolin::SaveWindowOnRender("window");

    if( Pushed(save_teapot) )
        d_cam.SaveOnRender("teapot");
    
    if(Pushed(record_teapot))
        DisplayBase().RecordOnRender("ffmpeg:[fps=50,bps=8388608,unique_filename]//screencap.avi");

    // Activate efficiently by object
    d_cam.Activate(s_cam);

    // Render some stuff
    glColor3f(1.0,1.0,1.0);
    glutWireTeapot(1.0);

    // Swap frames and Process Events
    pangolin::FinishFrame();
  }

  return 0;
}
