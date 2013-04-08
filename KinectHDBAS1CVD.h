/*********************************************************************************************/
/* this header is written as a part of Hamish Bassetts FYP at Monash University in 2012      */
/* according to statutes of the university all the intellectual property is owned by said    */
/* University.                                                                               */
/*********************************************************************************************/
/* The header is designed to create and instantiate all the necessary definitions to use     */
/* libfreenect version 1:0.0.1+20101211+2-2ubuntu1. To compile it was designed with CMake 2.8*/
/* in mind. Cmake can be sourced from www.cmake.org and is an open source system.            */
/* The program requires the above libfreenect and LibCVD                                     */
/*********************************************************************************************/
/* PLEASE NOTE THAT NO NAMESPACES HAVE BEEN USED TO SHOW WHERE ALL FUNCTIONS AND TYPES ARE   */
/* SOURCED FROM                                                                              */
/*********************************************************************************************/

/********       			  getBool() and getVideo() Functions      				  ********/
/* 		These functions take no input and output a pointer to the location of the            */
/*    new frame from the Kinect. This is method is not ideal as it does mean that any        */
/*    non blocking attempts to access these frames will cause a segfault on the first case   */
/*    when no frame is available however this method does allow the function that called     */
/*    to deal with the data in what ever manner it sees fit.                                 */
/*********************************************************************************************/

//protect against multiple declaration
#pragma once
//required to create a useable headder
#ifndef KINECTHDBAS1CVD_H
#define KINECTHDBAS1CVD_H

//libfreenect library
#include <libfreenect.hpp>
//LibCVD includes
#include <LibCVDIncludeWrapper.h>
//c++ core library
#include <vector>

//useful definitions that are not included in the libfreenect headers
#define         FREENECT_FRAME_W   640
#define         FREENECT_FRAME_H   480
#define         FREENECT_FRAME_PIX   (FREENECT_FRAME_H*FREENECT_FRAME_W) // width*height pixels in the image
#define         FREENECT_VIDEO_RGB_SIZE   (FREENECT_FRAME_PIX*3) //3 bytes per pixel

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   when using libfreenect you have to create your own class,
   inheriting from the FreenectDevice in the libfreenect.hpp
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

class KinectDeviceSingleThread : public Freenect::FreenectDevice //define a new class that is our kinect Device class
{
public:
	//constructor
	KinectDeviceSingleThread (freenect_context *_ctx, int _index) //*_ctx is a pointer to the device in the device map that is created by libfreenect to keep track of connected kinects, index is the identifier in this map
		: Freenect::FreenectDevice(_ctx, _index), //use default constructor for the inherited members from the FreenectDevice class that corresponds to this device
		  m_buffer_depth(FREENECT_DEPTH_11BIT), //vector type. Initialise the size of the depth buffer to 0, if want a non trivial value use FREENECT_FRAME_PIX
		  m_buffer_rgb(FREENECT_VIDEO_RGB_SIZE), //Vector type. Initialise the size of the RGB buffer to be 3 times the size of the frame pixels to allow R,G and B channels to be stored
		  m_gamma(2048), //m_gamma is a vector of gamma values
		  m_new_rgb_frame(false), //initialise the boolean flag that states if a new frame is recieved or not to false
		  m_new_depth_frame(false) //initialise the boolean flag that states if a new frame is recieved or not to false

		  {
		  //the for loop populates the m_gamma vector with a linear series of values from 0 to 9216
		  	for (unsigned int i=0 ; i<2048 ; i++)
		  		{
		  			float v = i/2048.0;
		  			v=(v*v*v)*6;
		  			m_gamma[i]=v*6*256;
		  		}//end for to create an array of different gamma values
		  	//populate the x,y values of the ImageRef FrameSize
			FrameSize.x=FREENECT_FRAME_W;
			FrameSize.y=FREENECT_FRAME_H;
		  } //end of constructor
		  
	//there needs to be a routine to handle Callbacks from the Kinect, this is how new frame flags are set. These functions should never be called by the programmer as the drivers will handle all of this.
	//necessary as the parent class does not have the functionality. Allows for customisable control. Obsfuscates the C++ standard from the C standard.
	
	//DO NOT CALL EVER
	/* Makes a copy of the pointer passed to the computer by the kinect and stores it in a class member then changes the new video frame flag high */
	void VideoCallback(void* _rgb, uint32_t timestamp)
	{
#ifdef DEBUG
	std::cout << "RGB callback called" <<endl;
#endif
	__rgb=_rgb;
	m_new_rgb_frame = true;
	};
	
	//DO NOT CALL
	/* Makes a copy of the pointer to the depth image passed to the computer by the kinect and stores it in a class member then changes the new depth frame flag high */
	void DepthCallback(void* _depth, uint32_t timestamp)
	{
		#ifdef DEBUG
			std::cout << "Depth callback called" <<endl;
		#endif
		__depth= _depth;
		m_new_depth_frame = true;
	};
	
	/**
	 * The user must create a blocking routine to prevent access with a zero vector. Failure to do so will result in a segfault when ever a new frame is not available.
	 **/	
	
	void* getVideo( void )
		{
			if (m_new_rgb_frame)
			{
				m_new_rgb_frame=false; //reset new frame flag to show frame has been taken
				return __rgb;
			}
			else
			return false; //if no new frame return a zero pointer
		}

	void* getDepth( void )
		{
			if (m_new_depth_frame)
			{
				m_new_depth_frame=false; //reset new frame flag to show frame has been taken
				return __depth;
			}
			else
			{
			return false; //if no new frame return a zero pointer
			}
	}

private:
	//definitions of variables used in function.
	std::vector<uint16_t> m_buffer_depth;
	std::vector<uint8_t> m_buffer_rgb;
	std::vector<uint16_t> m_gamma;
	
	//flags to control access to frame buffer to prevent asynchronous access.
	bool m_new_rgb_frame;
	bool m_new_depth_frame;
	
	//a class scope declaration of the frame size being used. Effective Constant declaration of 640*480
	CVD::ImageRef FrameSize;
	
	//a pair of void* pointers which are of the same type as what is given to the system by the kinect. Used to extract the pointers from the callbacks
	void* __rgb;
	void* __depth;
}; //end of class definition


#endif //end of #ifndef KINECTHDBAS1
