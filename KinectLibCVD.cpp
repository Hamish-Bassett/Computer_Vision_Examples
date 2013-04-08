#include "KinectHDBAS1CVD.h"
#include "LibCVDIncludeWrapper.h"
#include <iostream>
using namespace CVD;
using namespace std;

int main(void)
{
	//define the size of the window to be used
	ImageRef Size; Size.x=640; Size.y=480;
	
	//create an instance of the class Freenect from the namespace Freenect called KinectConstructor and use the default constructor.
	//Freenect::Freenect is a factory class that contains all the methods for making the Kinect device classes
	Freenect::Freenect KinectConstructor;

	//create an instance of the class KinectDeviceSingleThread from the KinectHDBAS1CVD.h file which is a called device1, use a reference to prevent bugs
	//the device is created using the factory class KinectConstructor and the function create device using the templated type of the class used.
	//the device ID is set to 0, mulitple Kinects can be connected at once so must use device IDs to prevent confusion.
	KinectDeviceSingleThread &device1 = KinectConstructor.createDevice<KinectDeviceSingleThread>(0);

	//The way the header works is it returns a pointer to the data when there's a new frame, at other times it returns false. So we need a storage for the pointer
	void* RGBPointer=0;
	void* DepthPointer=0;

	//There must be some way to hold the data. This can be done in a number of ways, the way presented here
	Image<Rgb<byte> > RGBFrame(Size);
	Image<unsigned short int> DepthFrame(Size);

	//no real reason to go video then depth except there appears to be a runtime error if depth then video.
	device1.startVideo();
	device1.startDepth();

	//CVD classes for displaying GL outputs with OpenGL event functionality
	GLWindow RGBRaw(Size,24,"RGB Window");
	GLWindow DepthRaw(Size,24, "Depth Window");

	//create a base type vector of the size of the Event member from GLWindow class called events.
	vector<GLWindow::Event> events;

	//flag to indicate when the program has finished or is done.
	bool done=false;

	while (!done)
	{
		RGBPointer=device1.getVideo();
	    RGBRaw.make_current();
	    //blocking access to get a new RGB frame in context of main
	    if (RGBPointer) //block if RGBPointer is zero
	    {
	    	BasicImage<Rgb<byte> > Temporary((Rgb<byte>*) RGBPointer, Size);
	    	RGBFrame.copy_from(Temporary);
	    }
	    glDrawPixels(RGBFrame);

	    DepthRaw.make_current();
	    DepthPointer=device1.getDepth();
	    //blocking access to get a new Depth frame in context of main
	    if (DepthPointer) //block if DepthPointer is zero
	    {
	    	BasicImage<short unsigned int> Temporary((short unsigned int *) DepthPointer, Size);
	    	DepthFrame.copy_from(Temporary);
	    }
	    glDrawPixels(DepthFrame);
	    //must swap buffers because GLWindow is a dual buffer implimentation. The program writes to the buffer not currently being displayed
	    RGBRaw.swap_buffers();
	    DepthRaw.swap_buffers();
	    //get X server events from the GL windows and store both in the events vector
    	RGBRaw.get_events(events);
	    DepthRaw.get_events(events);
	    //loop over all events
    	for(int i=0;i<events.size();i++){
	    	if(events[i].type == GLWindow::Event::KEY_DOWN)
	    	{
	    		switch (events[i].which){
	    		case 'q':
	    			done=true;
	    			break;
	    		default:
	    			break;
	    		}//end of switch events which
	    	}//end of if events type
	    }//end of loop over buffer
	    events.clear(); //empty the event buffer to prevent multiple processes of the same user input

	}//loop until q is pressed in the context of either window

	//release the resource to allow the OS to allow other programs to use the resource
	device1.stopVideo();
	device1.stopDepth();

	return 0;
}
