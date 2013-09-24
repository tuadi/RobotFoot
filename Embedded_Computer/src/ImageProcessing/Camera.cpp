#include "Camera.h"

using boost::filesystem::path;
using cv::VideoCapture;
using cv::Mat;
using cv::Point;

/** \brief Retrieve the instance of the Camera
 *
 * \return Camera&: Instance of the Camera object
 *
 */
Camera& Camera::getInstance()
{
	static Camera instance;
	return instance;
}

/** \brief Initialize the capture device from the XML configuration
 *
 * \param config XmlParser&: XML configuration containing the initialization informations
 * \return bool: Success of the initialization
 *
 */
bool Camera::initialize(XmlParser& config)
{
	return initialize(config.getIntValue(XmlPath::Root / XmlPath::ImageProcessing
		/ XmlPath::Camera / "DeviceIndex"));
}

/** \brief Initialize the capture device
 *
 * \param deviceIndex int: Id of the camera to retrieve the frame from
 * \return bool: Success of the initialization
 *
 */
bool Camera::initialize(int deviceIndex)
{
	_capture = VideoCapture(deviceIndex);
	return _capture.isOpened();
}

/** \brief Retrieve a frame from the camera
 *
 */
void Camera::captureFrame()
{
	if (!_capture.isOpened()) { return; }

	_capture >> _bgrFrame;

	processFrame();
}

/** \brief Retrieve the captured frame in a specified color space
 *
 * \param colorSpace ColorSpace: Color space of the frame to get
 * \return Mat&: Retrieved frame
 *
 */
Mat& Camera::getFrame(Camera::ColorSpace colorSpace)
{
	switch (colorSpace)
	{
	case ColorSpace::BGR:
		return _bgrFrame;
	case ColorSpace::HSV:
	default:
		return _hsvFrame;
	}
}

/** \brief Retrieve the coordinates of the center of the frame captured by the camera
 *
 * \return Point: Coordinates of the center of the camera image
 *
 */
Point Camera::getCenter()
{
	if (!_capture.isOpened()) { return Point(-1, -1); }
	return Point(_capture.get(CV_CAP_PROP_FRAME_WIDTH)/2, _capture.get(CV_CAP_PROP_FRAME_HEIGHT)/2);
}

/** \brief Convert the retrieved BRG frame in an HSV frame
 *
 */
void Camera::processFrame()
{
	if (_bgrFrame.empty()) { return; }
	ImageProcessing::BGRtoHSV(_bgrFrame, _hsvFrame);
}